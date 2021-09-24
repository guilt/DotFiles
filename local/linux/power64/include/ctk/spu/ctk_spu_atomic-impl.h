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

#ifndef _CTK_SPU_RAW_ATOMIC_H
#define _CTK_SPU_RAW_ATOMIC_H

#ifndef _CTK_SPU_ATOMIC_H
# error do not include this file directly!
#endif /* _CTK_SPU_ATOMIC_H */

#define CTK_SYNC_EVENT_ENABLED

/* internal routine */
static inline unsigned int _ctk_sync_init_event()
{
#ifdef CTK_SYNC_EVENT_ENABLED
    /* make sure we can hear the MFC_LLR_LOST_EVENT */
    return ctk_event_set_mask(MFC_LLR_LOST_EVENT);
#else
    ctk_init_rand();
    return 0;
#endif
}

/* internal routine */
static inline void _ctk_sync_wait_event()
{
#ifdef CTK_SYNC_EVENT_ENABLED
    unsigned int event = spu_readch(SPU_RdEventStat);
    spu_writech(SPU_WrEventAck, event);
#else
    DELAY();
#endif
}

/* internal routine */
static inline void _ctk_sync_finish_event(unsigned int mask)
{
#ifdef CTK_SYNC_EVENT_ENABLED
    ctk_event_set_mask(mask);
#endif
}

static inline int
_ctk_atomic_set(ctk_atomic_ea_t atomic, int val)
{
    char *ls;
    int old, status, tag = 0;
    ea_addr_t ea = ALIGN_DOWN(atomic, 128);
    unsigned int offset = atomic - ea;
    unsigned int oldmask;

    ctk_alloca_align_typed(ls, char *, sizeof(unsigned int), 128);

    oldmask = _ctk_sync_init_event();

    while (1) {
        /* load with reservation */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_GETLLAR_CMD);
        status = spu_readch(MFC_RdAtomicStat);

        old = *(int*)&ls[offset];
        *(int*)&ls[offset] = (int)val;

        /* attempt to store */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);

        if (likely(status == 0)) 
            break;
        else {
            _ctk_sync_wait_event();
        }
    }

    _ctk_sync_finish_event(oldmask);

    return old;
}

/**
 * Atomically add the value #val and return the old value
 */
static inline int
_ctk_atomic_add(ctk_atomic_ea_t atomic, int val)
{
    char *ls;
    unsigned int status;
    int tag = ctk_dma_new_tag();
    ea_addr_t ea = ALIGN_DOWN(atomic, 128);
    unsigned int offset = atomic - ea;
    int old;

    ctk_alloca_align_typed(ls, char *, sizeof(unsigned int), 128);
#if 1
    /* make sure we can hear the MFC_LLR_LOST_EVENT */
    unsigned int oldmask;
    oldmask = ctk_event_set_mask(MFC_LLR_LOST_EVENT);
#else
    ctk_init_rand();
#endif

    while (1) {
        /* load with reservation */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_GETLLAR_CMD);
        status = spu_readch(MFC_RdAtomicStat);

        old = *(int*)&ls[offset];
        *(int*)&ls[offset] = (int)(old + val);

        /* attempt to store */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);

        if (likely(status == 0))
            break;
        else {
            _ctk_sync_wait_event();
        }
    }

    return old;
}

/**
 * Atomically add the value #val and return the old value
 */
static inline int
_ctk_atomic_sub(ctk_atomic_ea_t v, int val)
{
    return _ctk_atomic_add(v, -val);
}

/**
 * Atomically increment the value and return the old value
 */
static inline int 
_ctk_atomic_inc(ctk_atomic_ea_t v) {
    return _ctk_atomic_add(v, 1);
}

/**
 * Atomically decrement the value and return the old value
 */
static inline int 
_ctk_atomic_dec(ctk_atomic_ea_t v) {
    return _ctk_atomic_sub(v, 1);
}

/**
 * Atomically read the value, clear the memory and return the old value
 */
static inline int 
_ctk_atomic_read_and_clear(ctk_atomic_ea_t v) {
    return _ctk_atomic_set(v, 0);
}

/** Reads the value */
static inline signed int 
_ctk_atomic_get_value(ctk_atomic_ea_t atomic) {
    char *ls;
    ea_addr_t ea = ALIGN_DOWN(atomic, 128);
    unsigned int offset = atomic - ea;
    ctk_alloca_align_typed(ls, char *, sizeof(unsigned int), 128);

    ctk_dma_get_small_block(ls, ea, 128);

    return *(int*)&ls[offset];
}

#endif /* _CTK_SPU_RAW_ATOMIC_H */
