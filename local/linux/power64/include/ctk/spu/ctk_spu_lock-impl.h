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

#ifndef _CTK_SPU_LOCK_H
#define _CTK_SPU_LOCK_H

#include "ctk_spu_event.h"
/*
 * Definitions for low-level locks (lower routines for mutexes)
 */

#define CTK_LOCK_SIZE   128

static inline void _ctk_raw_lock_init(ea_addr_t lock_ea)
{
    char *ls;
    ea_addr_t ea = ALIGN_DOWN(lock_ea, CTK_LOCK_SIZE);
    unsigned int offset = lock_ea - ea;
    ctk_alloca_align_typed(ls, char *, sizeof(unsigned int), CTK_LOCK_SIZE);

    ctk_dma_get_small_block(ls, ea, CTK_LOCK_SIZE);

    /* initializes the lock to 0 (unlocked). */
    *(int*)&ls[offset] = 0;

    ctk_dma_put_small_block(ls, ea, CTK_LOCK_SIZE);
}

static inline void _ctk_raw_lock(ea_addr_t lock_ea)
{
    char *ls;
    int status, tag = 0;
    ea_addr_t ea = ALIGN_DOWN(lock_ea, CTK_LOCK_SIZE);
    unsigned int offset = lock_ea - ea;
    unsigned int oldmask;
    ctk_alloca_align_typed(ls, char *, sizeof(unsigned int), CTK_LOCK_SIZE);
    
    oldmask = _ctk_sync_init_event();

    do {
        /* load with reservation */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), CTK_LOCK_SIZE, tag, 
            MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);

        if (likely(*(int*)&ls[offset] == 0)) {
            /* if the lock is not held */
            *(int*)&ls[offset] = 1;
            spu_mfcdma64(ls, ea_hi32(ea),ea_lo32(ea), CTK_LOCK_SIZE, tag, 
                MFC_PUTLLC_CMD);
            status = spu_readch(MFC_RdAtomicStat);
        } else {
            /* the lock has been held by someone; wait an event */
            _ctk_sync_wait_event();
            status = 1; /* spin */
        }
    } while (status != 0);

    _ctk_sync_finish_event(oldmask);
}

static inline void _ctk_raw_unlock(ea_addr_t lock_ea)
{
    _ctk_atomic_set(lock_ea, 0);
}

static inline int _ctk_raw_trylock(ea_addr_t lock_ea)
{
    char *ls;
    int ret, status, tag = 0;
    ea_addr_t ea = ALIGN_DOWN(lock_ea, CTK_LOCK_SIZE);
    unsigned int offset = lock_ea - ea;

    ctk_alloca_align_typed(ls, char *, sizeof(unsigned int), CTK_LOCK_SIZE);
    
    do {
        /* load with reservation */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), CTK_LOCK_SIZE,
            tag, MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);

        if (likely(*(int*)&ls[offset] == 0)) {
            /* if the lock is not held; lock it and return 0 */
            *(int*)&ls[offset] = 1;
            spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), CTK_LOCK_SIZE,
                tag, MFC_PUTLLC_CMD);
            status = spu_readch(MFC_RdAtomicStat);
            ret = 0;
        } else {
            /* the lock has been held by someone; release the reservation and
               return BUSY immediately */
            spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), CTK_LOCK_SIZE,
                tag, MFC_PUTLLC_CMD);
            status = spu_readch(MFC_RdAtomicStat);
            status = 0;
            return CTK_ERROR_BUSY;
        }
    } while (status != 0);

    return 0;
}


#endif /* _CTK_SPU_LOCK_H */
