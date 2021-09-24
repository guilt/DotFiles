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

#ifndef _CTK_SPU_GENERICQ_IML_H
#define _CTK_SPU_GENERICQ_IML_H

#ifndef _CTK_SPU_GENERICQ_H
# error do not include this file directly!
#endif

#include <stddef.h>
#include <string.h>

#ifndef offsetof
# define offsetof(s,m)  (size_t)&(((s *)0)->m)
#endif

/* lower routine */
static inline void
_ctk_genericq_put_enq(ea_addr_t items_ea, int where, void *item, int size)
{
    char *ls;
    ea_addr_t item_ea = items_ea + where * size;
    ea_addr_t ea = ALIGN_DOWN(item_ea, 128);
    int offset = item_ea - ea;
    ctk_alloca_align_typed(ls, char*, 128, 128);
    DEBUG_PRINTF("GET items[%d] %llx -> %x (%x)\n", where, ea, (int)ls, 128);
    ctk_dma_get_small_block(ls, ea, 128);
    memcpy(&ls[offset], item, size);
    DEBUG_PRINTF("PUT items[%d] %x -> %llx (%x)\n", where, (int)ls, ea, 128);
    ctk_dma_put_small_block(ls, ea, 128);
}

/* lower routine */
static inline void
_ctk_genericq_get_deq(ea_addr_t items_ea, int where, void *item, int size)
{
    char *ls;
    ea_addr_t item_ea = items_ea + where * size;
    ea_addr_t ea = ALIGN_DOWN(item_ea, 128);
    int offset = item_ea - ea;
    ctk_alloca_align_typed(ls, char*, 128, 128);
    DEBUG_PRINTF("GET items[%d] %llx -> %x (%x)\n", where, ea, (int)ls, 128);
    ctk_dma_get_small_block(ls, ea, 128);
    memcpy(item, &ls[offset], size);
}

static inline void
_ctk_genericq_enq(ctk_genericq_ea_t queue, void *item)
{
    int expect_full = 0;
    ctk_genericq_t *q;
    ctk_alloca_align_typed(q, ctk_genericq_t*, sizeof(ctk_genericq_t), 128);

    _ctk_mutex_lock(queue);
    ctk_dma_get_small_block((void*)q, queue, sizeof(ctk_genericq_t));
    while (__builtin_expect(q->count == q->capacity, expect_full)) {
        _ctk_mutex_unlock(queue);
        //DELAY();
        _ctk_mutex_lock(queue);
        ctk_dma_get_small_block((void*)q, queue, sizeof(ctk_genericq_t));
        expect_full = 1;
    }
    _ctk_genericq_put_enq(q->items_ea, q->wp, item, q->item_size);
    q->wp++;
    q->count++;
    if (q->wp == q->capacity) q->wp = 0;
    ctk_dma_put_small_block((void*)q, queue, sizeof(ctk_genericq_t));
    _ctk_mutex_unlock(queue);
}

static inline void
_ctk_genericq_deq(ctk_genericq_ea_t queue, void *item)
{
    ctk_genericq_t *q;
    int expect_empty = 0;
    ctk_alloca_align_typed(q, ctk_genericq_t*, sizeof(ctk_genericq_t), 128);

    _ctk_mutex_lock(queue);
    ctk_dma_get_small_block((void*)q, queue, sizeof(ctk_genericq_t));
    while (__builtin_expect(q->count == 0, expect_empty)) {
        _ctk_mutex_unlock(queue);
        //DELAY();
        _ctk_mutex_lock(queue);
        ctk_dma_get_small_block((void*)q, queue, sizeof(ctk_genericq_t));
        expect_empty = 1;
    }

    _ctk_genericq_get_deq(q->items_ea, q->rp, item, q->item_size);
    q->rp++;
    if (q->rp == q->capacity) q->rp = 0;
    q->count--;
    ctk_dma_put_small_block((void*)q, queue, sizeof(ctk_genericq_t));
    _ctk_mutex_unlock(queue);
}

static inline int
_ctk_genericq_peek(ctk_genericq_ea_t queue, void *item)
{
    ctk_genericq_t *q;
    ctk_alloca_align_typed(q, ctk_genericq_t*, sizeof(ctk_genericq_t), 128);

    _ctk_mutex_lock(queue);
    ctk_dma_get_small_block(q, queue, sizeof(ctk_genericq_t));
    if (q->count == 0) {
        _ctk_mutex_unlock(queue);
        return CTK_ERROR_NO_DATA;
    }

    _ctk_genericq_get_deq(q->items_ea, q->rp, item, q->item_size);

    _ctk_mutex_unlock(queue);
    return CTK_SUCCESS;
}

static inline int
_ctk_genericq_tryenq(ctk_genericq_ea_t queue, void *item)
{
    int expect_full = 0;
    ctk_genericq_t *q;
    ctk_alloca_align_typed(q, ctk_genericq_t*, sizeof(ctk_genericq_t), 128);

    _ctk_mutex_lock(queue);
    ctk_dma_get_small_block((void*)q, queue, sizeof(ctk_genericq_t));
    while (__builtin_expect(q->count == q->capacity, expect_full)) {
        _ctk_mutex_unlock(queue);
        return CTK_ERROR_RETRY;
    }
    _ctk_genericq_put_enq(q->items_ea, q->wp, item, q->item_size);
    q->wp++;
    q->count++;
    if (q->wp == q->capacity) q->wp = 0;
    ctk_dma_put_small_block((void*)q, queue, sizeof(ctk_genericq_t));
    _ctk_mutex_unlock(queue);
    return CTK_SUCCESS;
}

static inline int
_ctk_genericq_trydeq(ctk_genericq_ea_t queue, void *item)
{
    ctk_genericq_t *q;
    int expect_empty = 0;
    ctk_alloca_align_typed(q, ctk_genericq_t*, sizeof(ctk_genericq_t), 128);

    _ctk_mutex_lock(queue);
    ctk_dma_get_small_block((void*)q, queue, sizeof(ctk_genericq_t));
    while (__builtin_expect(q->count == 0, expect_empty)) {
        _ctk_mutex_unlock(queue);
        return CTK_ERROR_RETRY;
    }

    _ctk_genericq_get_deq(q->items_ea, q->rp, item, q->item_size);
    q->rp++;
    if (q->rp == q->capacity) q->rp = 0;
    q->count--;
    ctk_dma_put_small_block((void*)q, queue, sizeof(ctk_genericq_t));
    _ctk_mutex_unlock(queue);
    return CTK_SUCCESS;
}

static inline int _ctk_genericq_get_remaining_capacity(ctk_genericq_ea_t queue) {
    ctk_genericq_t *q;
    ctk_alloca_align_typed(q, ctk_genericq_t*, sizeof(ctk_genericq_t), 128);
    ctk_dma_get_small_block(q, queue, 128);
    return (q->capacity - q->count);
}

static inline int _ctk_genericq_get_remaining_count(ctk_genericq_ea_t queue) {
    ctk_genericq_t *q;
    ctk_alloca_align_typed(q, ctk_genericq_t*, sizeof(ctk_genericq_t), 128);
    ctk_dma_get_small_block(q, queue, 128);
    return (q->count);
}

static inline int _ctk_genericq_get_capacity(ctk_genericq_ea_t queue) {
    ctk_genericq_t *q;
    ctk_alloca_align_typed(q, ctk_genericq_t*, sizeof(ctk_genericq_t), 128);
    ctk_dma_get_small_block(q, queue, 128);
    return (q->capacity);
}

#endif /* _CTK_SPU_GENERICQ_IML_H */
