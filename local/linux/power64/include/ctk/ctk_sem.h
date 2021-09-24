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

#ifndef _CTK_SEM_H
#define _CTK_SEM_H

/**
 * @file ctk_sem.h
 * @brief Definitions for semaphore operations 
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include "ctk_common.h"
#include "ctk_atomic.h"

/** semaphore type (must be aligned in a 128-bit line) */
typedef volatile signed int ctk_sem_t __attribute__((aligned(128)));

/** semaphore reference type */
typedef ea_addr_t ctk_sem_ea_t;

/** A macro to get a semaphore reference for the given semaphore variable.
 * @param[in] sem_var - a semaphore variable (ctk_sem_t)
 */
#define CTK_SEM_EA(sem_var)  ((ctk_sem_ea_t)(unsigned long)(&(sem_var)))

/** A macro to get a semaphore variable from the given semaphore reference.
 * @param[in] sem_ea - a semaphore reference (ctk_sem_ea_t)
 */
#define CTK_SEM_VAR(sem_ea)  (*(ctk_sem_t*)(unsigned long)(sem_ea))

/** A macro to get a pointer to the semaphore variable from the
 * given semaphore reference.
 * @param[in] sem_ea - a semaphore reference (ctk_sem_ea_t)
 */
#define CTK_SEM_PTR(sem_ea)  ((ctk_sem_t*)(unsigned long)(sem_ea))

#include "ctk_sem-impl.h"

/**
 * Creates a new semaphore reference.  A new semaphore
 * is allocated (by #ctk_sync_alloc) and its reference
 * is returned.  (You can also directly declares
 * a #ctk_sem_t static variable and gets
 * a reference for it using #CTK_SEM_EA macro.)
 * @param[out] sem - a created semaphore reference.
 * @param[in] value - the initial value
 * @return 0 on success, errno on error
 */
int ctk_sem_create(ctk_sem_ea_t *sem, unsigned int value);

/**
 * Destroys the given semaphore reference.  Cond-var references
 * created by #ctk_sem_create must be destroyed by this routine.
 * @param[in] sem - a sem reference to be destroyed.
 */
void ctk_sem_destroy(ctk_sem_ea_t sem);

/**
 * Initializes the semaphore with the given value.
 * @param[in, out] sem - semaphore to be initialized.
 * @param[in] value - initial value for the semaphore.
 * @return 0 on success, errno on error
 */
int ctk_sem_init(ctk_sem_ea_t sem, unsigned int value);

/**
 * Increment the given semaphore.
 * @param[in,out] sem - semaphore to be incremented.
 */
void ctk_sem_post(ctk_sem_ea_t sem);

/**
 * Decrement the given semaphore if the semaphore is greater than zero.
 * If the semaphore's value is zero, the calling thread will block
 * until the value becomes greater than zero.
 * @param[in,out] sem - semaphore to be decremented.
 */
void ctk_sem_wait(ctk_sem_ea_t sem);

/**
 * Decrement the given semaphore only if the semaphore is greater than zero.
 * @param[in,out] sem - semaphore to be decremented.
 * @return 0 if the semaphore is decremented, non-zero otherwise.
 */
int ctk_sem_trywait(ctk_sem_ea_t sem);

/**
 * Returns the current semaphore counter value
 * @param[in] sem - semaphore whose value is returned.
 */
static inline int ctk_sem_get_value(ctk_sem_ea_t sem) {
    return _ctk_sem_get_value(sem);
}

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SEM_H */
