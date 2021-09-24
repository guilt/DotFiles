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

#ifndef _CTK_SPU_SEM_H
#define _CTK_SPU_SEM_H

/**
 * @file ctk_spu_sem.h
 * @brief Definitions for SPU-side semaphore operations 
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "ctk_spu_atomic.h"

/** 
 * semaphore reference type 
 */
typedef ea_addr_t ctk_sem_ea_t;

#include "ctk_spu_sem-impl.h"

/**
 * Initializes the semaphore with the given value.
 * @param[in,out] sem - semaphore to be initialized.
 * @param[in] value - initial value for the semaphore.
 */
void ctk_sem_init(ctk_sem_ea_t sem, unsigned int value);

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

/** Returns the semaphore value */
static inline int ctk_sem_get_value(ctk_sem_ea_t sem) {
    return _ctk_sem_get_value(sem);
}

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SPU_SEM_H */
