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

#ifndef _CTK_SPINLOCK_H
#define _CTK_SPINLOCK_H

/**
 * @file ctk_spinlock.h
 * @brief Definitions for spinlock operations
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "ctk_common.h"
#include "ctk_error.h"
#include "ctk_atomic.h"

/** Spin lock */
typedef volatile unsigned int ctk_spinlock_t __attribute__((aligned(128)));

/** Spin lock reference type */
typedef ea_addr_t ctk_spinlock_ea_t;

/** The ctk spinlock initializer */
#define CTK_SPINLOCK_INITIALIZER        (0)

/** A macro to get a lock reference for the given lock variable.
 * @param[in] lock_var - a lock variable (ctk_spinlock_t)
 */
#define CTK_SPINLOCK_EA(lock_var)  \
    ((ctk_spinlock_ea_t)(unsigned long)(&(lock_var)))

/** A macro to get a lock variable from the given lock reference.
 * @param[in] lock_ea - a lock reference (ctk_spinlock_ea_t)
 */
#define CTK_SPINLOCK_VAR(lock_ea)  \
    (*(ctk_spinlock_t*)(unsigned long)(lock_ea))

/** A macro to get a pointer to the lock variable from the
 * given lock reference.
 * @param[in] lock_ea - a lock reference (ctk_lock_ea_t)
 */
#define CTK_SPINLOCK_PTR(lock_ea)  \
    ((ctk_spinlock_t*)(unsigned long)(lock_ea))

#include "ctk_spinlock-impl.h"

/** Initializes a spinlock */
void ctk_spinlock_init(ctk_spinlock_ea_t lock);

/** Acquires a spin lock. Spins while the lock is busy.  */
int ctk_spin_lock(ctk_spinlock_ea_t lock);

/**
 * Tries to acquire a spin lock.
 * @return 0 if successfully locked, CTK_ERROR_BUSY (nonzero) otherwise.
 */
int ctk_spin_trylock(ctk_spinlock_ea_t lock);

/**
 * Releases the acquired lock.
 */
void ctk_spin_unlock(ctk_spinlock_ea_t lock);

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SPINLOCK_H */
