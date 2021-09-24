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

#ifndef _CTK_COND_IMPL_H
#define _CTK_COND_IMPL_H 

#include <string.h>
#include "ctk_error.h"
#include "ctk_sync_alloc.h"

#ifndef _CTK_COND_H
# error do not include this file directly!
#endif

static inline int _ctk_cond_create(ctk_cond_ea_t *condp)
{
    ctk_cond_t *cond;
    cond = (ctk_cond_t*)ctk_sync_alloc_align(CTK_SYNC_COND, 128, 128);
    if (cond == NULL)
        return CTK_ERROR_NO_MEMORY;
    memset((void*)cond, 0, sizeof(ctk_cond_t));
    *condp = CTK_COND_EA(*cond);
    return CTK_SUCCESS;
}

static inline void _ctk_cond_destroy(ctk_cond_ea_t cond)
{
    CTK_DEBUG_ASSERT(cond != NULL);
    ctk_sync_free_align((void*)CTK_COND_PTR(cond));
}

static inline void _ctk_cond_init(ctk_cond_ea_t cond)
{
    memset(CTK_COND_PTR(cond), 0, sizeof(ctk_cond_t));
}

static inline void _ctk_cond_wait(ctk_cond_ea_t cond, ctk_mutex_ea_t mutex)
{
    volatile ctk_cond_t *var = CTK_COND_PTR(cond);
    _ctk_mutex_lock(cond);

    /* release the mutex */
    _ctk_mutex_unlock(mutex);

    /* increment nwaiters */
    var->total_seq++;
    var->nwaiters++;

    unsigned long val;
    unsigned long seq;
    unsigned long bcast;
    val = seq = var->wakeup_seq;
    bcast = var->bcast_seq;

    do {
        _ctk_mutex_unlock(cond);

        /* simple wait; just yield here */
        sched_yield();

        _ctk_mutex_lock(cond);

        if (bcast != var->bcast_seq)
            goto broadcasted;

        val = var->wakeup_seq;
    } while (val == seq || var->woken_seq == val);

    var->woken_seq++;

broadcasted:
    /* decrement nwaiters */
    var->nwaiters--;

    _ctk_mutex_unlock(cond);

    /* reacquire the mutex */
    _ctk_mutex_lock(mutex);
}

static inline void _ctk_cond_signal(ctk_cond_ea_t cond)
{
    ctk_cond_t *var = CTK_COND_PTR(cond);
    _ctk_mutex_lock(cond);

    /* Are there any waiters? */
    if (var->total_seq > var->wakeup_seq) {
        var->wakeup_seq++;
    }

    _ctk_mutex_unlock(cond);
}

static inline void _ctk_cond_broadcast(ctk_cond_ea_t cond)
{
    ctk_cond_t *var = CTK_COND_PTR(cond);
    _ctk_mutex_lock(cond);

    /* Are there any waiters? */
    if (var->total_seq > var->wakeup_seq) {
        /* Wake all of them up */
        var->wakeup_seq = var->total_seq;
        var->woken_seq = var->total_seq;
        var->bcast_seq++;
    }

    _ctk_mutex_unlock(cond);
}

#endif /* _CTK_COND_IMPL_H */
