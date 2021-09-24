/*
 * CTK - The Cell Tool Kit Library
 * http://ctk-dev.sourceforge.net/
 * http://cell.fixstars.com/ctk/
 *
 * Copyright (C) 2006-2008 Fixstars Corp.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#if !defined(_CTK_SPU_QUEUE_H) && !defined(_CTK_QUEUE_TEMPLATE)
# error do not include this file directly!
#endif

/* lower routine */
static void _ctk_queue_put_enq(ea_addr_t items_ea, int where, ctk_queue_item_t item)
{
    ea_addr_t item_ea = items_ea + where * sizeof(ctk_queue_item_t);
#if ((_CTK_QITEM_SIZE % 16) != 0)
    volatile char *ls;
    ea_addr_t ea = ALIGN_DOWN(item_ea, 128);
    int offset = item_ea - ea;
    ctk_alloca_align_typed(ls, char*, 128, 128);
    DEBUG_PRINTF("GET items[%d] %llx -> %x (%x)\n", where, ea, (int)ls, 128);
    ctk_dma_get_small_block((void*)ls, ea, 128);
# if (_CTK_QITEM_SIZE > 8)
    memcpy(&ls[offset], item, sizeof(ctk_queue_item_t));
# else /* _CTK_QITEM_SIZE < 8 */
    *(ctk_queue_item_t*)&ls[offset] = item;
# endif /* _CTK_QITEM_SIZE < 8 */
    DEBUG_PRINTF("PUT items[%d] %x -> %llx (%x)\n", 
        where, (int)ls, ea, 128);
    ctk_dma_put_small_block((void*)ls, ea, 128);
#else /* (_CTK_QITEM_SIZE % 16) == 0 */
    /* assume the item is aligned */
    DEBUG_PRINTF("PUT items[%d] %x -> %llx (%x)\n", 
        where, (int)item, item_ea, sizeof(ctk_queue_item_t));
    ctk_dma_put_small_block((void*)item, item_ea, sizeof(ctk_queue_item_t));
#endif /* (_CTK_QITEM_SIZE % 16) == 0 */
}

/* lower routine */
static inline void
_ctk_queue_get_deq(ea_addr_t items_ea, int where, ctk_queue_item_t *item)
{
    char *ls;
    ea_addr_t item_ea = items_ea + where * sizeof(ctk_queue_item_t);
    ea_addr_t ea = ALIGN_DOWN(item_ea, 128);
    int offset = item_ea - ea;
    ctk_alloca_align_typed(ls, char*, 128, 128);
    DEBUG_PRINTF("GET items[%d] %llx -> %x (%x)\n", where, ea, (int)ls, 128);
    ctk_dma_get_small_block(ls, ea, 128);
#if (_CTK_QITEM_SIZE > 8)
    memcpy(*item, &ls[offset], _CTK_QITEM_SIZE);
#else
    *item = *(ctk_queue_item_t*)&ls[offset];
#endif
}

static inline void _ctk_queue_enq(ctk_queue_ea_t queue, ctk_queue_item_t item)
{
    volatile unsigned int status = 1;
    volatile int tag = 0, wp = 0;
    volatile ctk_queue_t *q;

    ctk_alloca_align_typed(q, ctk_queue_t*, sizeof(ctk_queue_t), 128);
    do {
        /* load with reservation */
        spu_mfcdma64(q, ea_hi32(queue), ea_lo32(queue), 128, tag, 
            MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);

        if (q->wcount >= q->capacity || q->wcount != q->rcount) {
            continue;
        }
        /* remember where we should put the item */
        wp = q->wp;

        /* update the (local) counter values */
        if (++q->wp == q->capacity) q->wp = 0;
        q->wcount++;

        /* store conditional */
        spu_mfcdma64(q, ea_hi32(queue), ea_lo32(queue), 128, tag, 
            MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);
    } while (status != 0);

    /* store the value */
    _ctk_queue_put_enq(q->items_ea, wp, item);

    DEBUG_PRINTF("[SPU ENQ %d] (%d:%lld) rp:%d rcount:%d / wp:%d wcount:%d\n", 
        ctk_env_get_rank(), wp, item,
        q->rp, q->rcount,
        q->wp, q->wcount);

    status = 1;
    /* increment the counter for readers */
    do {
        /* load with reservation */
        spu_mfcdma64(q, ea_hi32(queue), ea_lo32(queue), 128, tag, 
            MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);
        if (q->wcount - q->rcount > 1) {
            continue;
        }

        q->rcount++;

        /* store conditional */
        spu_mfcdma64(q, ea_hi32(queue), ea_lo32(queue), 128, tag, 
            MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);
    } while (status != 0);
}

static inline void _ctk_queue_deq(ctk_queue_ea_t queue, ctk_queue_item_t *item)
{
    volatile ctk_queue_t *q;
    volatile int rp, tag = 0, expect_empty = 0;
    volatile int status = 1;
    ctk_alloca_align_typed(q, ctk_queue_t*, sizeof(ctk_queue_t), 128);

    do {
        /* load with reservation */
        spu_mfcdma64(q, ea_hi32(queue), ea_lo32(queue), 128, tag, 
            MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);

        if (__builtin_expect(q->rcount <= 0, expect_empty)) {
            expect_empty = 1;
            continue;
        }

        _ctk_queue_get_deq(q->items_ea, q->rp, item);

        /* update the (local) counter values */
        rp = q->rp;
        if (++q->rp == q->capacity) q->rp = 0;
        q->rcount--;
        q->wcount--;

        /* store conditional */
        spu_mfcdma64(q, ea_hi32(queue), ea_lo32(queue), 128, tag, 
            MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);

        DEBUG_PRINTF("[SPU DEQ %d] (%d:%lld) rp:%d rcount:%d / wp:%d wcount:%d "
            "status:%d\n", 
            ctk_env_get_rank(), rp, *item, 
            q->rp, q->rcount, q->wp, q->wcount, status);

    } while (status != 0);
}

static inline int
_ctk_queue_tryenq(ctk_queue_ea_t queue, ctk_queue_item_t item)
{
    volatile unsigned int status = 1;
    int tag = 0, wp;
    ctk_queue_t *q;
    ctk_alloca_align_typed(q, ctk_queue_t*, sizeof(ctk_queue_t), 128);

    do {
        /* load with reservation */
        spu_mfcdma64(q, ea_hi32(queue), ea_lo32(queue), 128, tag, 
            MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);

        if (q->wcount >= q->capacity || q->wcount != q->rcount) {
            return CTK_ERROR_RETRY;
        }
        /* remember where we should put the item */
        wp = q->wp;

        /* update the (local) counter values */
        if (++q->wp == q->capacity) q->wp = 0;
        q->wcount++;

        /* store conditional */
        spu_mfcdma64(q, ea_hi32(queue), ea_lo32(queue), 128, tag, 
            MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);
    } while (status != 0);

    /* store the value */
    _ctk_queue_put_enq(q->items_ea, wp, item);

    DEBUG_PRINTF("[SPU ENQ %d] (%d:%lld) rp:%d rcount:%d / wp:%d wcount:%d)\n", 
        ctk_env_get_rank(), wp, item,
        q->rp, q->rcount,
        q->wp, q->wcount);

    status = 1;
    /* increment the counter for readers */
    do {
        /* load with reservation */
        spu_mfcdma64(q, ea_hi32(queue), ea_lo32(queue), 128, tag, 
            MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);
        if (q->wcount - q->rcount > 1) {
            continue;
        }

        q->rcount++;

        /* store conditional */
        spu_mfcdma64(q, ea_hi32(queue), ea_lo32(queue), 128, tag, 
            MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);
    } while (status != 0);
    return CTK_SUCCESS;
}

static inline int
_ctk_queue_trydeq(ctk_queue_ea_t queue, ctk_queue_item_t *item)
{
    ctk_queue_t *q;
    int rp, tag = 0, expect_empty = 0;
    volatile int status = 1;
    ctk_alloca_align_typed(q, ctk_queue_t*, sizeof(ctk_queue_t), 128);

    do {
        /* load with reservation */
        spu_mfcdma64(q, ea_hi32(queue), ea_lo32(queue), 128, tag, 
            MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);

        if (__builtin_expect(q->rcount <= 0, expect_empty)) {
            return CTK_ERROR_RETRY;
        }

        _ctk_queue_get_deq(q->items_ea, q->rp, item);

        /* update the (local) counter values */
        rp = q->rp;
        if (++q->rp == q->capacity) q->rp = 0;
        q->rcount--;
        q->wcount--;

        /* store conditional */
        spu_mfcdma64(q, ea_hi32(queue), ea_lo32(queue), 128, tag, 
            MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);

        DEBUG_PRINTF("[SPU DEQ %d] (%d:%lld) rp:%d rcount:%d / wp:%d wcount:%d "
            "status:%d\n", 
            ctk_env_get_rank(), rp, *item, 
            q->rp, q->rcount, q->wp, q->wcount, status);

    } while (status != 0);
    return CTK_SUCCESS;
}

static inline int
_ctk_queue_peek(ctk_queue_ea_t queue, void *item)
{
    ctk_queue_t *q;
    ctk_alloca_align_typed(q, ctk_queue_t*, sizeof(ctk_queue_t), 128);

    ctk_dma_get_small_block(q, queue, 128);
    if (q->rcount <= 0) 
        return CTK_ERROR_NO_DATA;

    _ctk_queue_get_deq(q->items_ea, q->rp, (ctk_queue_item_t*)item);

    return CTK_SUCCESS;
}

static inline int _ctk_queue_get_capacity(ctk_queue_ea_t queue) {
    ctk_queue_t *q;
    ctk_alloca_align_typed(q, ctk_queue_t*, sizeof(ctk_queue_t), 128);
    ctk_dma_get_small_block(q, queue, 128);
    return q->capacity;
}

static inline int _ctk_queue_get_remaining_capacity(ctk_queue_ea_t queue) {
    ctk_queue_t *q;
    ctk_alloca_align_typed(q, ctk_queue_t*, sizeof(ctk_queue_t), 128);
    ctk_dma_get_small_block(q, queue, 128);
    return (q->capacity - q->wcount);
}

static inline int _ctk_queue_get_remaining_count(ctk_queue_ea_t queue) {
    ctk_queue_t *q;
    ctk_alloca_align_typed(q, ctk_queue_t*, sizeof(ctk_queue_t), 128);
    ctk_dma_get_small_block(q, queue, 128);
    return (q->rcount);
}

