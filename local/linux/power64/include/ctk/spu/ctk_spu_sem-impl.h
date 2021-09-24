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

#ifndef _CTK_SPU_SEM_IMPL_H
#define _CTK_SPU_SEM_IMPL_H

#ifndef _CTK_SPU_SEM_H
# error do not include this file directly!
#endif

static inline void
_ctk_sem_init(ctk_sem_ea_t sem, unsigned int value)
{
    _ctk_atomic_set(sem, value);
}

static inline void
_ctk_sem_post(ctk_sem_ea_t sem)
{
    _ctk_atomic_inc(sem);
}

static inline void
_ctk_sem_wait(ctk_sem_ea_t sem)
{
    char *ls;
    int status, tag = 0;
    ea_addr_t ea = ALIGN_DOWN(sem, 128);
    unsigned int offset = sem - ea;
    unsigned int oldmask;
    volatile signed int tmp;

    ctk_alloca_align_typed(ls, char *, sizeof(unsigned int), 128);
    oldmask = _ctk_sync_init_event();
    
    do {
        /* load with reservation */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);

        tmp = *(int*)&ls[offset] - 1;

        if (likely(tmp >= 0)) {
            /* if we can decrement; store the decremented value */
            *(int*)&ls[offset] = tmp;
            spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128,
                tag, MFC_PUTLLC_CMD);
            status = spu_readch(MFC_RdAtomicStat);
        } else {
            _ctk_sync_wait_event();
            status = 1; /* we should spin here */
        }
    } while (status != 0);

    _ctk_sync_finish_event(oldmask);
}

static inline int
_ctk_sem_trywait(ctk_sem_ea_t sem)
{
    char *ls;
    int status, tag = 0;
    ea_addr_t ea = ALIGN_DOWN(sem, 128);
    unsigned int offset = sem - ea;
    volatile signed int tmp;

    ctk_alloca_align_typed(ls, char *, sizeof(unsigned int), 128);
    
    do {
        /* load with reservation */
        spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);

        tmp = *(int*)&ls[offset] - 1;

        if (likely(tmp >= 0)) {
            /* if we can decrement; store the decremented value */
            *(int*)&ls[offset] = tmp;
            spu_mfcdma64(ls, ea_hi32(ea),ea_lo32(ea), 128, tag, MFC_PUTLLC_CMD);
            status = spu_readch(MFC_RdAtomicStat);
        } else {
            /* we should fail here */
            spu_mfcdma64(ls, ea_hi32(ea),ea_lo32(ea), 128, tag, MFC_PUTLLC_CMD);
            spu_readch(MFC_RdAtomicStat);
            status = 0;
            return CTK_ERROR_RETRY;
        }
    } while (status != 0);

    return 0;
}

static inline int
_ctk_sem_get_value(ctk_sem_ea_t sem)
{
    char *ls;
    int tag = 0;
    ea_addr_t ea = ALIGN_DOWN(sem, 128);
    unsigned int offset = sem - ea;
    volatile signed int tmp;

    ctk_alloca_align_typed(ls, char *, sizeof(unsigned int), 128);
    
    /* load with reservation */
    spu_mfcdma64(ls, ea_hi32(ea), ea_lo32(ea), 128, tag, MFC_GETLLAR_CMD);
    spu_readch(MFC_RdAtomicStat);

    tmp = *(int*)&ls[offset];

    spu_mfcdma64(ls, ea_hi32(ea),ea_lo32(ea), 128, tag, MFC_PUTLLC_CMD);
    spu_readch(MFC_RdAtomicStat);

    return tmp;
}

#endif /* _CTK_SPU_SEM_IMPL_H */
