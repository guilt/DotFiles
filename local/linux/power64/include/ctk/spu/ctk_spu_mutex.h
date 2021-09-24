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

#ifndef _CTK_SPU_MUTEX_H
#define _CTK_SPU_MUTEX_H

/** 
 * @file ctk_spu_mutex.h
 * @brief Definitions for SPU-side mutex operations
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "../ctk_common.h"
#include "ctk_spu_atomic.h"
#include "ctk_spu_lock-impl.h"

/** 
 * mutex reference type 
 */
typedef ea_addr_t ctk_mutex_ea_t;

#include "ctk_spu_mutex-impl.h"

#if 0 /* Not sure but functions do not work on tSDK (while inlines do.) */
/**
 * Initializes a mutex mutex.
 * @param[in] mutex - a mutex lock (EA ref) to be initialized.
 */
void ctk_mutex_init(ctk_mutex_ea_t mutex);

/**
 * Acquires a simple mutex lock.  It will wait if the lock is
 * held by any PPU/SPU. 
 * @param[in] mutex - a mutex lock (EA ref) to be acquired.  The lock must
 *  have been allocated on the PPE main memory.
 */
void ctk_mutex_lock(ctk_mutex_ea_t mutex);

/**
 * Releases the acquired mutex lock.
 * @param[in] mutex - a mutex lock (EA ref) to be released.
 */
void ctk_mutex_unlock(ctk_mutex_ea_t mutex);

/**
 * Tries to acquire a mutex lock.
 * @param[in] lock - a mutex lock (EA ref) to be tried.
 * @return 0 if successfully locked, CTK_ERROR_BUSY (non-zero) otherwise.
 */
int ctk_mutex_trylock(ctk_mutex_ea_t mutex);
#else /* 0 */
/**
 * Initializes a mutex lock.
 * @param[in] mutex - a mutex reference to be initialized.
 */
static inline void ctk_mutex_init(ctk_mutex_ea_t mutex)
{
    _ctk_mutex_init(mutex);
}

/**
 * Acquires a simple mutex lock.  It will wait if the lock is
 * held by any PPU/SPU. 
 * @param[in] mutex - a mutex reference to be acquired.
 */
static inline void ctk_mutex_lock(ctk_mutex_ea_t mutex)
{
    _ctk_mutex_lock(mutex);
}

/**
 * Releases the acquired mutex lock.
 * @param[in] mutex - a mutex reference to be released.
 */
static inline void ctk_mutex_unlock(ctk_mutex_ea_t mutex)
{
    _ctk_mutex_unlock(mutex);
}

/**
 * Tries to acquire a mutex lock.
 * @param[in] mutex - a mutex reference to be tried.
 * @return 0 if successfully locked, CTK_ERROR_BUSY (non-zero) otherwise.
 */
static inline int ctk_mutex_trylock(ctk_mutex_ea_t mutex)
{
    return _ctk_mutex_trylock(mutex);
}

/**
 * Reads out the value of the given mutex lock.
 * @param[in] mutex - a 
 */
static inline unsigned int ctk_mutex_get_value(ctk_mutex_ea_t mutex) {
    return _ctk_mutex_get_value(mutex);
}

#endif /* 0 */

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SPU_MUTEX_H */
