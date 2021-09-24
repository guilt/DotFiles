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

#ifndef _CTK_BARRIER_IMPL_H
#define _CTK_BARRIER_IMPL_H

#ifndef _CTK_BARRIER_H
# error do not include this file directly!
#endif

#include "ctk_error.h"
#include "ctk_atomic.h"
#include "ctk_profile.h"

static inline int _ctk_barrier_destroy(ctk_barrier_ea_t barrier)
{
    if (barrier == 0)
        return CTK_ERROR_NULL_OBJECT;
    ctk_sync_free_align(CTK_BARRIER_PTR(barrier));
    return CTK_SUCCESS;
}

static inline int _ctk_barrier_create(ctk_barrier_ea_t *barrierp, unsigned int count)
{
    ctk_barrier_t *b;
    b = (ctk_barrier_t*)ctk_sync_alloc_align(CTK_SYNC_BARRIER, 128, 128);
    if (count == 0)
        return CTK_ERROR_BAD_VALUE;
    b->total = count;
    b->count = 0;
    *barrierp = EAADDR(b);
    return CTK_SUCCESS;
}

static inline int _ctk_barrier_init(ctk_barrier_ea_t barrier, unsigned int count)
{
    ctk_barrier_t *b = CTK_BARRIER_PTR(barrier);
    if (b == NULL)
        return CTK_ERROR_NULL_OBJECT;
    b->total = count;
    b->count = 0;
    return CTK_SUCCESS;
}

static inline int _ctk_barrier_wait(ctk_barrier_ea_t barrier)
{
    volatile ctk_barrier_t *b = CTK_BARRIER_PTR(barrier);
    volatile unsigned int status = 0;
    unsigned int bval, count;
    if (b == NULL)
        return CTK_ERROR_NULL_OBJECT;

    do {
        /* load with reservation */
        __asm__ __volatile__ ("# ctk_barrier_wait load \n"
            "lwarx %0, 0, %1 \n\t"
            : "=r" (bval)
            : "r" (b)
        );

        count = (bval & 0xffff);
        count++;
        bval = (bval & 0xffff0000) | count;

        /* store conditional */
        __asm__ __volatile__ ("# ctk_barrier_wait store\n\t"
            "li %0, 0\n"
            "stwcx. %2, 0, %1 \n\t"
            "bne 1f \n\t"
            "li %0, 1 # store 1 to status \n"
        "1:	\n"
            : "=&r" (status)
            : "r" (b), "r" (bval)
            : "cr0", "memory");
    } while (status == 0);

    while (b->count < b->total) {
        sched_yield();
    }
    return CTK_SUCCESS;
}

/* non-blocking one */
static inline int _ctk_barrier_trywait(ctk_barrier_ea_t barrier)
{
    volatile ctk_barrier_t *b = CTK_BARRIER_PTR(barrier);
    volatile unsigned int status = 0;
    unsigned int bval, count;
    if (b == NULL)
        return CTK_ERROR_NULL_OBJECT;

    do {
        /* load with reservation */
        __asm__ __volatile__ ("# ctk_barrier_wait load \n"
            "lwarx %0, 0, %1 \n\t"
            : "=r" (bval)
            : "r" (b)
        );

        count = (bval & 0xffff);
        count++;
        bval = (bval & 0xffff0000) | count;

        /* store conditional */
        __asm__ __volatile__ ("# ctk_barrier_wait store\n\t"
            "li %0, 0\n"
            "stwcx. %2, 0, %1 \n\t"
            "bne 1f \n\t"
            "li %0, 1 # store 1 to status \n"
        "1:	\n"
            : "=&r" (status)
            : "r" (b), "r" (bval)
            : "cr0", "memory");
    } while (status == 0);

    if (b->count < b->total) 
        return CTK_ERROR_RETRY;
    return CTK_SUCCESS;
}

/* non-blocking one - wait only; 
   ctk_barrier_trywait must have been called in prior to this one */
static inline int _ctk_barrier_waitonly(ctk_barrier_ea_t barrier)
{
    volatile ctk_barrier_t *b = CTK_BARRIER_PTR(barrier);
    __asm__ __volatile__ ("isync":::"memory");
    if (b->count < b->total) {
        return CTK_ERROR_RETRY;
    }
    return CTK_SUCCESS;
}

#endif /* _CTK_BARRIER_IMPL_H */
