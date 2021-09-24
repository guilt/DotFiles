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

#ifndef _CTK_SPU_MUTEX_IMPL_H
#define _CTK_SPU_MUTEX_IMPL_H

#ifndef _CTK_SPU_MUTEX_H
# error do not include this file directly!
#endif /* _CTK_SPU_MUTEX_H */

static inline void _ctk_mutex_init(ctk_mutex_ea_t mutex)
{
    _ctk_raw_lock_init(mutex);
}

static inline void _ctk_mutex_lock(ctk_mutex_ea_t mutex)
{
    _ctk_raw_lock(mutex);
}

static inline void _ctk_mutex_unlock(ctk_mutex_ea_t mutex)
{
    _ctk_raw_unlock(mutex);
}

static inline int _ctk_mutex_trylock(ctk_mutex_ea_t mutex)
{
    return _ctk_raw_trylock(mutex);
}

static inline unsigned int _ctk_mutex_get_value(ctk_mutex_ea_t mutex)
{
    return (unsigned int)_ctk_atomic_get_value(mutex);
}

#endif /* _CTK_SPU_MUTEX_IMPL_H */
