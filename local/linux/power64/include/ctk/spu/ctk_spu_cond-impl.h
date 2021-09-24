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

#ifndef _CTK_SPU_COND_IMPL_H
#define _CTK_SPU_COND_IMPL_H

#ifndef _CTK_SPU_COND_H
# error do not include this file directly!
#endif /* _CTK_SPU_COND_H */

#include <string.h>

#ifdef CELL_ENV_TSDK
void *memset(void *s, int c, size_t n);
#endif

#define CTK_COND_SIZE             (sizeof(ctk_cond_t))

static inline void _ctk_cond_init(ctk_cond_ea_t cond)
{
    char *ls;
    ea_addr_t ea = ALIGN_DOWN(cond, 128);
    unsigned int offset = cond - ea;
    ctk_cond_t *cond_var;

    ctk_alloca_align_typed(ls, char *, CTK_COND_SIZE, 128);
    ctk_dma_get_small_block(ls, ea, CTK_COND_SIZE);

    /* initializes the condvar */
    memset((void*)&ls[offset], 0, CTK_COND_SIZE);

    /* make sure we have lock EA addr in the EA's ctk_cond_t */
    cond_var = (ctk_cond_t*)&ls[offset];

    ctk_dma_put_small_block(ls, ea, CTK_COND_SIZE);
}

/** lower routine: lock and acquire the cond_var from EA */
static inline ctk_cond_t *
_ctk_cond_acquire(ctk_cond_ea_t cond, char *buf, ea_addr_t ea, int offset)
{
    _ctk_raw_lock(cond);
    ctk_dma_get_small_block(buf, ea, CTK_COND_SIZE);
    return (ctk_cond_t*)&buf[offset];
}

/** lower routine: release the cond_var from EA and unlock */
static inline void
_ctk_cond_release(ctk_cond_ea_t cond, char *buf, ea_addr_t ea, int offset)
{
    ctk_dma_put_small_block(buf, ea, CTK_COND_SIZE);
    _ctk_raw_unlock(cond);
}

static inline void _ctk_cond_wait(ctk_cond_ea_t cond, ctk_mutex_ea_t mutex)
{
    char *ls;
    ea_addr_t ea = ALIGN_DOWN(cond, 128);
    int offset = cond - ea;
    ctk_cond_t *cond_var;
    ctk_alloca_align_typed(ls, char *, CTK_COND_SIZE, 128);

    cond_var = _ctk_cond_acquire(cond, ls, ea, offset);

    /* release the mutex */
    _ctk_mutex_unlock(mutex);

    /* increment waiters */
    cond_var->total_seq++;
    cond_var->nwaiters++;

    unsigned long val;
    unsigned long seq;
    unsigned long bcast;
    val = seq = cond_var->wakeup_seq;
    bcast = cond_var->bcast_seq;

    ctk_init_rand();
    do {
        _ctk_cond_release(cond, ls, ea, offset);

        /* simple wait */
        DELAY();

        cond_var = _ctk_cond_acquire(cond, ls, ea, offset);

        if (bcast != cond_var->bcast_seq)
            goto broadcasted;

        val = cond_var->wakeup_seq;
    } while (likely(val == seq || cond_var->woken_seq == val));

    cond_var->woken_seq++;

broadcasted:
    /* decrement nwaiters */
    cond_var->nwaiters--;

    _ctk_cond_release(cond, ls, ea, offset);

    /* reacquire the mutex */
    _ctk_mutex_lock(mutex);
}

static inline void _ctk_cond_signal(ctk_cond_ea_t cond)
{
    char *ls;
    ea_addr_t ea = ALIGN_DOWN(cond, 128);
    int offset = cond - ea;
    ctk_cond_t *cond_var;
    ctk_alloca_align_typed(ls, char *, CTK_COND_SIZE, 128);
    
    cond_var = _ctk_cond_acquire(cond, ls, ea, offset);

    /* Are there any waiters? */
    if (cond_var->total_seq > cond_var->wakeup_seq) {
        cond_var->wakeup_seq++;
    }

    _ctk_cond_release(cond, ls, ea, offset);
}

static inline void _ctk_cond_broadcast(ctk_cond_ea_t cond)
{
    char *ls;
    ea_addr_t ea = ALIGN_DOWN(cond, 128);
    int offset = cond - ea;
    ctk_cond_t *cond_var;
    ctk_alloca_align_typed(ls, char *, CTK_COND_SIZE, 128);
    
    cond_var = _ctk_cond_acquire(cond, ls, ea, offset);

    /* Are there any waiters? */
    if (cond_var->total_seq > cond_var->wakeup_seq) {
        /* Wake all of them up */
        cond_var->wakeup_seq = cond_var->total_seq;
        cond_var->woken_seq = cond_var->total_seq;
        cond_var->bcast_seq++;
    }

    _ctk_cond_release(cond, ls, ea, offset);
}

#endif /* _CTK_SPU_COND_IMPL_H */
