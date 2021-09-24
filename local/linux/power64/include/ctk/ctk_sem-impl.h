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

#ifndef _CTK_SEM_IMPL_H
#define _CTK_SEM_IMPL_H

#include "ctk_error.h"
#include "ctk_sync_alloc.h"

#ifndef _CTK_SEM_H
# error do not include this file directly!
#endif

static inline int _ctk_sem_create(ctk_sem_ea_t *semp, unsigned int value)
{
    ctk_sem_t *sem;
    sem = (ctk_sem_t*)ctk_sync_alloc_align(CTK_SYNC_SEM, 128, 128);
    if (sem == NULL)
        return CTK_ERROR_NO_MEMORY;
    /*
    if (value == 0)
        return CTK_ERROR_BAD_VALUE;
    */
    *semp = CTK_SEM_EA(*sem);
    _ctk_atomic_set(*semp, value);
    return CTK_SUCCESS;
}

static inline void _ctk_sem_destroy(ctk_sem_ea_t sem)
{
    ctk_sync_free_align((void*)CTK_SEM_PTR(sem));
}

static inline int
_ctk_sem_init(ctk_sem_ea_t sem, unsigned int value)
{
    /*
    if (value == 0)
        return CTK_ERROR_BAD_VALUE;
    */
    _ctk_atomic_set(sem, value);
    return CTK_SUCCESS;
}

static inline void
_ctk_sem_post(ctk_sem_ea_t sem)
{
    _ctk_atomic_inc(sem);
}

static inline int
_ctk_sem_trywait(ctk_sem_ea_t sem)
{
    volatile int ret;
    volatile int tmp;
    ctk_sem_t *semp = CTK_SEM_PTR(sem);

    __asm__ __volatile__ ("# ctk_sem_trywait \n"
"1:	lwarx   %2, 0, %1       # load with reservation \n\t"
        "addi    %0, %2, -1      # ret = val - 1 \n\t"
        "cmpwi   %2, 0           # compare loaded val with 0 \n\t"
        "bgt     2f              # branch if greater than 0 \n\t"
        "stwcx.  %2, 0, %1       # attempt to store old value \n\t"
        "bne-    1b              # spin if failed \n\t"
        "b       3f              # branch to finish \n\t"
"2:	stwcx.  %0, 0, %1       # attempt to store new val \n\t"
        "bne-    1b              # spin if failed \n\t"
        "li      %0,0            # return 0 if OK \n\t"
"3:	                        # done\n"
        : "=&r" (ret)
        : "b" (semp), "b" (tmp)
        : "cr0"
    );

    return ret;
}

static inline void
_ctk_sem_wait(ctk_sem_ea_t sem)
{
    while (_ctk_sem_trywait(sem))
        sched_yield();
}

static inline int _ctk_sem_get_value(ctk_sem_ea_t sem) {
    return CTK_SEM_VAR(sem);
}

#endif /* _CTK_SEM_IMPL_H */
