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

#ifndef _CTK_MUTEX_IMPL_H
#define _CTK_MUTEX_IMPL_H

#include "ctk_error.h"
#include "ctk_sync_alloc.h"

#ifndef _CTK_MUTEX_H
# error do not include this file directly!
#endif

static inline int _ctk_mutex_create(ctk_mutex_ea_t *lockp)
{
    ctk_mutex_t *lock;
    lock = (ctk_mutex_t*)ctk_sync_alloc_align(CTK_SYNC_MUTEX, 
                sizeof(ctk_mutex_t), 128);
    if (lock == NULL)
        return CTK_ERROR_NO_MEMORY;
    *lock = 0;
    *lockp = CTK_MUTEX_EA(*lock);
    return CTK_SUCCESS;
}

static inline void _ctk_mutex_destroy(ctk_mutex_ea_t lock)
{
    CTK_DEBUG_ASSERT(lock != NULL);
    ctk_sync_free_align((void*)CTK_MUTEX_PTR(lock));
}

static inline void _ctk_mutex_init(ctk_mutex_ea_t lock)
{
    CTK_MUTEX_VAR(lock) = 0;
}

static inline void _ctk_mutex_lock(ctk_mutex_ea_t lock)
{
    /* for now simplest implementation */
    while (_ctk_spin_trylock(lock) != 0) {
        sched_yield();
    }
    CTK_DEBUG_ASSERT(CTK_MUTEX_VAR(lock) != 0);
}

static inline int _ctk_mutex_trylock(ctk_mutex_ea_t lock)
{
    return _ctk_spin_trylock(lock);
}

static inline void _ctk_mutex_unlock(ctk_mutex_ea_t lock)
{
    CTK_DEBUG_ASSERT(CTK_MUTEX_VAR(lock) != 0);
    _ctk_spin_unlock(lock);
}

static inline unsigned int _ctk_mutex_get_value(ctk_mutex_ea_t lock) {
    return CTK_MUTEX_VAR(lock);
}

#endif /* _CTK_MUTEX_IMPL_H */
