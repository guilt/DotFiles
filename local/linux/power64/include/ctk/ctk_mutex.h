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

#ifndef _CTK_MUTEX_H
#define _CTK_MUTEX_H

/**
 * @file ctk_mutex.h
 * @brief Definitions for mutex operations
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include "ctk_common.h"
#include "ctk_spinlock.h"

/** Mutex type (must be aligned in a 128-bit line) */
typedef volatile unsigned int ctk_mutex_t __attribute__((aligned(128)));

/* unaligned version of mutex type (for internal use) */
typedef volatile unsigned int ctk_raw_mutex_t;

/** Mutex reference type */
typedef ea_addr_t ctk_mutex_ea_t;

/** A static initializer for a mutex lock. */
#define CTK_MUTEX_INITIALIZER   { 0 }

/** A macro to get a mutex reference for the given mutex variable.
 * @param[in] mutex_var - a mutex variable (ctk_mutex_t)
 */
#define CTK_MUTEX_EA(mutex_var)  ((ctk_mutex_ea_t)(unsigned long)(&(mutex_var)))

/** A macro to get a mutex variable from the given mutex reference.
 * @param[in] mutex_ea - a mutex reference (ctk_mutex_ea_t)
 */
#define CTK_MUTEX_VAR(mutex_ea)  (*(ctk_mutex_t*)(unsigned long)(mutex_ea))

/** A macro to get a pointer to the mutex variable from the
 * given mutex reference.
 * @param[in] mutex_ea - a mutex reference (ctk_mutex_ea_t)
 */
#define CTK_MUTEX_PTR(mutex_ea)  ((ctk_mutex_t*)(unsigned long)(mutex_ea))

#include "ctk_mutex-impl.h"

/**
 * Creates and initializes a new mutex lock reference.  A new mutex lock
 * is allocated (by #ctk_sync_alloc) and its reference
 * is returned.  (You can also directly declares a
 * #ctk_mutex_t static variable and gets
 * a reference for it using #CTK_MUTEX_EA macro.)
 * @param[out] mutex - a created mutex lock reference.
 */
int ctk_mutex_create(ctk_mutex_ea_t *mutex);

/**
 * Destroys the given mutex lock reference.  Any mutex lock
 * created by #ctk_mutex_create must be destroyed by this routine.
 * @param[in] mutex - a mutex reference to be destroyed.
 */
void ctk_mutex_destroy(ctk_mutex_ea_t mutex);

/**
 * Initializes a mutex lock.
 * @param[in, out] lock - a mutex lock to be initialized.
 */
void ctk_mutex_init(ctk_mutex_ea_t lock);

/**
 * Acquires a simple mutex lock.  It will wait if the lock is
 * held by any PPU/SPU. 
 * @param[in] lock - a mutex lock to be acquired.  The lock must
 *  have been initialized by CTK_MUTEX_INITIALIZER or
 *  ctk_mutex_init.
 * @note This mutex_lock just tries to acquire a spin lock and
 *  does yield if failed.  Use ctk_spin_lock instead if the
 *  competing threads are SPU threads and yield does not help
 *  the program to run faster.
 */
void ctk_mutex_lock(ctk_mutex_ea_t lock);

/**
 * Tries to acquire a mutex lock.
 * @param[in] lock - a mutex lock to be tried.
 * @return 0 if successfully locked, CTK_ERROR_BUSY (non-zero) otherwise.
 */
int ctk_mutex_trylock(ctk_mutex_ea_t lock);

/**
 * Releases the acquired mutex lock.
 * @param[in] lock - a mutex lock to be released.
 */
void ctk_mutex_unlock(ctk_mutex_ea_t lock);

/**
 * Returns a mutex lock value (0 or 1).
 * @param[in] mutex - a mutex lock to be examined.
 */
static inline unsigned int ctk_mutex_get_value(ctk_mutex_ea_t mutex) {
    return _ctk_mutex_get_value(mutex);
}

#ifdef __cplusplus
}
#endif

#endif /* _CTK_MUTEX_H */
