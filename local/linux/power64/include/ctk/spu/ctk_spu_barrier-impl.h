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

#ifndef _CTK_SPU_BARRIER_IMPL_H
#define _CTK_SPU_BARRIER_IMPL_H

#include "ctk_spu_common.h"

#ifndef _CTK_SPU_BARRIER_H
# error do not include this file directly!
#endif

static inline int _ctk_barrier_init(ctk_barrier_ea_t barrier, unsigned int count) {
    char *ls;
    ctk_barrier_t *b;
    volatile unsigned int status = 1;
    int tag = 0;
    ea_addr_t ea = ALIGN_DOWN(barrier, 128);
    unsigned int offset = barrier - ea;

    ctk_alloca_align_typed(ls, char *, 128, 128);

    if (count == 0)
        return CTK_ERROR_BAD_VALUE;

    do {
        /* load with reservation */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);

        b = (ctk_barrier_t*)&ls[offset];
        b->total = count;
        b->count = 0;

        /* store conditional */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);
    } while (unlikely(status != 0));
    return CTK_SUCCESS;
}

static inline void _ctk_barrier_wait(ctk_barrier_ea_t barrier) {
    char *ls;
    volatile ctk_barrier_t *b;
    volatile unsigned int status = 1;
    int tag = 0;
    ea_addr_t ea = ALIGN_DOWN(barrier, 128);
    unsigned int offset = barrier - ea;

    ctk_alloca_align_typed(ls, char *, 128, 128);

    do {
        /* load with reservation */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);

        b = (ctk_barrier_t*)&ls[offset];
        b->count++;

        /* store conditional */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);
    } while (unlikely(status != 0));

    while (b->count < b->total) {
        ctk_dma_get_small_block(ls, ea, 128);
        b = (ctk_barrier_t*)&ls[offset];
        DELAY();
    }
}

/* non-blocking one */
static inline int _ctk_barrier_trywait(ctk_barrier_ea_t barrier) {
    char *ls;
    volatile ctk_barrier_t *b;
    volatile unsigned int status = 1;
    int tag = 0;
    ea_addr_t ea = ALIGN_DOWN(barrier, 128);
    unsigned int offset = barrier - ea;

    ctk_alloca_align_typed(ls, char *, 128, 128);

    do {
        /* load with reservation */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);

        b = (ctk_barrier_t*)&ls[offset];
        b->count++;

        /* store conditional */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);
    } while (unlikely(status != 0));

    if (b->count < b->total) {
        return CTK_ERROR_RETRY;
    }
    return CTK_SUCCESS;
}

/* non-blocking one - wait only; 
   ctk_barrier_trywait must have been called in prior to this one */
static inline int _ctk_barrier_waitonly(ctk_barrier_ea_t barrier) {
    char *ls;
    volatile ctk_barrier_t *b;
    ea_addr_t ea = ALIGN_DOWN(barrier, 128);
    unsigned int offset = barrier - ea;
    ctk_alloca_align_typed(ls, char *, 128, 128);

    ctk_dma_getb_small_block(ls, ea, 128);
    b = (ctk_barrier_t*)&ls[offset];
    if (b->count < b->total) {
        return CTK_ERROR_RETRY;
    }
    return CTK_SUCCESS;
}

#endif /* _CTK_SPU_BARRIER_IMPL_H */
