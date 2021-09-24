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

#ifndef _CTK_ATOMIC_IMPL_H
#define _CTK_ATOMIC_IMPL_H

#ifndef _CTK_ATOMIC_H
# error do not include this file directly!
#endif

#include "ctk_error.h"
#include "ctk_sync_alloc.h"

static inline int _ctk_atomic_create(ctk_atomic_ea_t *atomicp)
{
    ctk_atomic_t *atomic;
    atomic = (ctk_atomic_t*)ctk_sync_alloc_align(CTK_SYNC_ATOMIC, 
                sizeof(ctk_atomic_t), 128);
    if (atomic == NULL)
        return CTK_ERROR_NO_MEMORY;
    *atomic = 0;
    *atomicp = CTK_ATOMIC_EA(*atomic);
    return CTK_SUCCESS;
}

static inline void _ctk_atomic_destroy(ctk_atomic_ea_t ea)
{
    CTK_DEBUG_ASSERT(ea != NULL);
    ctk_sync_free_align((void*)CTK_ATOMIC_PTR(ea));
}

static inline int _ctk_atomic_set(ctk_atomic_ea_t v, int val)
{
    volatile int old;
    ctk_atomic_t *ptr = CTK_ATOMIC_PTR(v);

    __asm__ __volatile__ (
        "# ctk_atomic_set\n"
 "1:	lwarx %0, 0, %1         # load old value  \n\t"
        "stwcx. %2, 0, %1       # attempt to store \n\t"
        "bne- 1b                # spin if failed\n\t"
        : "=&r" (old)
        : "r" (ptr), "r" (val)
        : "cr0"
    );

    return old;
}

static inline int _ctk_atomic_add(ctk_atomic_ea_t v, int val)
{
    volatile int tmp, old;
    ctk_atomic_t *ptr = CTK_ATOMIC_PTR(v);

    __asm__ __volatile__ (
        "# ctk_atomic_add\n"
"1:	lwarx %0, 0, %2         # load old value \n\t"
        "add %1, %3, %0         # calculate new value \n\t"
        "stwcx. %1, 0, %2       # attempt to store \n\t"
        "bne- 1b                # spin if failed \n\t"
        : "=&r" (old), "=&r" (tmp)
        : "r" (ptr), "r" (val)
        : "cr0", "memory"
    );

    return old;
}

static inline int _ctk_atomic_sub(ctk_atomic_ea_t v, int val) {
    volatile int tmp, old;
    ctk_atomic_t *ptr = CTK_ATOMIC_PTR(v);

    __asm__ __volatile__ (
        "# ctk_atomic_sub\n"
"1:	lwarx %0, 0, %2         # load old value \n\t"
        "subf %1, %3, %0        # calculate new value \n\t"
        "stwcx. %1, 0, %2       # attempt to store \n\t"
        "bne- 1b                # spin if failed \n\t"
        : "=&r" (old), "=&r" (tmp)
        : "r" (ptr), "r" (val), "m" (*ptr)
        : "cr0", "memory"
    );

    return old;
}

static inline int _ctk_atomic_read_and_clear(ctk_atomic_ea_t v) {
    volatile int old, tmp;
    ctk_atomic_t *ptr = CTK_ATOMIC_PTR(v);

    __asm__ __volatile__ (
        "# ctk_atomic_read_and_clear\n"
        "\tsync\n"
"1:	lwarx %0, 0, %3         # load old value \n\t"
        "li %1, 0               # load new value \n\t"
        "stwcx. %1, 0, %3       # attempt to store \n\t"
        "bne- 1b                # spin if failed \n\t"
        : "=&r" (old), "=&r" (tmp), "=m" (*ptr)
        : "r" (ptr), "m" (*ptr)
        : "cr0", "memory"
    );

    return old;
}

/* 64-bit version */
static inline long _ctk_atomic_set64(ctk_atomic_ea_t v, long val)
{
    volatile long old;
    ctk_atomic_t *ptr = CTK_ATOMIC_PTR(v);

    __asm__ __volatile__ (
        "# ctk_atomic_set64\n"
 "1:	ldarx %0, 0, %1         # load old value  \n\t"
        "stdcx. %2, 0, %1       # attempt to store \n\t"
        "bne- 1b                # spin if failed\n\t"
        : "=&r" (old)
        : "r" (ptr), "r" (val)
        : "cr0"
    );

    return old;
}

static inline int _ctk_atomic_inc(ctk_atomic_ea_t v) {
    return _ctk_atomic_add(v, 1);
}

static inline int _ctk_atomic_dec(ctk_atomic_ea_t v) {
    return _ctk_atomic_sub(v, 1);
}

static inline int _ctk_atomic_get_value(ctk_atomic_ea_t v) {
    return CTK_ATOMIC_VAR(v);
}

/* 64-bit version */
static inline long _ctk_atomic_add64(ctk_atomic_ea_t v, long val)
{
    volatile long tmp, old;
    ctk_atomic64_t *ptr = CTK_ATOMIC_PTR64(v);

    __asm__ __volatile__ (
        "# ctk_atomic_add64\n"
"1:	ldarx %0, 0, %2         # load old value \n\t"
        "add %1, %3, %0         # calculate new value \n\t"
        "stdcx. %1, 0, %2       # attempt to store \n\t"
        "bne- 1b                # spin if failed \n\t"
        : "=&r" (old), "=&r" (tmp)
        : "r" (ptr), "r" (val)
        : "cr0", "memory"
    );

    return old;
}

/* 64-bit version */
static inline long _ctk_atomic_sub64(ctk_atomic_ea_t v, long val) {
    volatile long tmp, old;
    ctk_atomic64_t *ptr = CTK_ATOMIC_PTR64(v);

    __asm__ __volatile__ (
        "# ctk_atomic_sub\n"
"1:	ldarx %0, 0, %2         # load old value \n\t"
        "subf %1, %3, %0        # calculate new value \n\t"
        "stdcx. %1, 0, %2       # attempt to store \n\t"
        "bne- 1b                # spin if failed \n\t"
        : "=&r" (old), "=&r" (tmp)
        : "r" (ptr), "r" (val), "m" (*ptr)
        : "cr0", "memory"
    );

    return old;
}

/* 64-bit version */
static inline long _ctk_atomic_read_and_clear64(ctk_atomic_ea_t v) {
    volatile long old, tmp;
    ctk_atomic64_t *ptr = CTK_ATOMIC_PTR64(v);

    __asm__ __volatile__ (
        "# ctk_atomic_read_and_clear64\n"
        "\tsync\n"
"1:	ldarx %0, 0, %3         # load old value \n\t"
        "li %1, 0               # load new value \n\t"
        "stdcx. %1, 0, %3       # attempt to store \n\t"
        "bne- 1b                # spin if failed \n\t"
        : "=&r" (old), "=&r" (tmp), "=m" (*ptr)
        : "r" (ptr), "m" (*ptr)
        : "cr0", "memory"
    );

    return old;
}

/* 64-bit version */
static inline long _ctk_atomic_inc64(ctk_atomic_ea_t v) {
    return _ctk_atomic_add64(v, 1);
}

/* 64-bit version */
static inline long _ctk_atomic_dec64(ctk_atomic_ea_t v) {
    return _ctk_atomic_sub64(v, 1);
}

/* 64-bit version */
static inline long _ctk_atomic_get_value64(ctk_atomic_ea_t v) {
    return *(long*)(unsigned long)(v);
}

/* 64-bit CAS operation */
static inline int __ctk_atomic_compare_and_swap(long int *p, long int oldval, long int newval)
{
    long int ret;
    __asm__ __volatile__ ( "# ctk_atomic_compare_and_swap \n"
        "0:	ldarx %0, 0, %1 \n\t"
                "xor.  %0, %3, %0 \n\t"
                "bne 1f \n\t"
                "stdcx. %2, 0, %1 \n\t"
                "bne- 0b \n"
        "1:"
            : "=&r"(ret)
            : "r"(p), "r"(newval), "r"(oldval)
            : "cr0", "memory");
    __asm__ __volatile__ ("sync" ::: "memory");
    return (int)(ret == 0);
}

/* 32-bit CAS operation */
static inline int __ctk_atomic_compare_and_swap32(int *p, int oldval, int newval)
{
    int ret;
    __asm__ __volatile__ ( "# ctk_atomic_compare_and_swap32 \n"
        "0:	lwarx %0, 0, %1 \n\t"
                "xor.  %0, %3, %0 \n\t"
                "bne 1f \n\t"
                "stwcx. %2, 0, %1 \n\t"
                "bne- 0b \n"
        "1:"
            : "=&r"(ret)
            : "r"(p), "r"(newval), "r"(oldval)
            : "cr0", "memory");
    __asm__ __volatile__ ("sync" ::: "memory");
    return (int)(ret == 0);
}

#ifdef _LP64
# define _ctk_atomic_compare_and_swap(p,o,n)  __ctk_atomic_compare_and_swap(p,o,n)
# define _ctk_atomic_compare_and_swap32(p,o,n)  __ctk_atomic_compare_and_swap32(p,o,n)
#else /* !_LP64 */
# define _ctk_atomic_compare_and_swap(p,o,n)  __ctk_atomic_compare_and_swap32((int*)p,(int)o,(int)n)
# define _ctk_atomic_compare_and_swap32(p,o,n)  __ctk_atomic_compare_and_swap32(p,o,n)
#endif /* !_LP64 */

#ifdef _LP64
typedef ctk_atomic64_t ctk_atomic_long_t;
# define _ctk_atomic_set_long(ea, val)  _ctk_atomic_set64(ea, val)
# define _ctk_atomic_add_long(ea, val)  _ctk_atomic_add64(ea, val)
# define _ctk_atomic_sub_long(ea, val)  _ctk_atomic_sub64(ea, val)
# define _ctk_atomic_dec_long(ea)       _ctk_atomic_dec64(ea)
# define _ctk_atomic_inc_long(ea)       _ctk_atomic_inc64(ea)
# define _ctk_atomic_get_value_long(ea) _ctk_atomic_get_value64(ea)
#else /* !_LP64 */
typedef ctk_atomic_t ctk_atomic_long_t;
# define _ctk_atomic_set_long(ea, val)  _ctk_atomic_set(ea, (int)val)
# define _ctk_atomic_add_long(ea, val)  _ctk_atomic_add(ea, (int)val)
# define _ctk_atomic_sub_long(ea, val)  _ctk_atomic_sub(ea, (int)val)
# define _ctk_atomic_dec_long(ea)       _ctk_atomic_dec(ea)
# define _ctk_atomic_inc_long(ea)       _ctk_atomic_inc(ea)
# define _ctk_atomic_get_value_long(ea) _ctk_atomic_get_value(ea)
#endif /* !_LP64 */

/* for PPU local use */
static inline int _ctk_atomic_create_local(ctk_atomic_ea_t *atomicp)
{
    ctk_atomic_t *atomic;
    atomic = (ctk_atomic_t*)ctk_sync_alloc(CTK_SYNC_ATOMIC,
                sizeof(ctk_atomic_t));
    if (atomic == NULL)
        return CTK_ERROR_NO_MEMORY;
    *atomic = 0;
    *atomicp = CTK_ATOMIC_EA(*atomic);
    return CTK_SUCCESS;
}

/* for PPU local use */
static inline void _ctk_atomic_destroy_local(ctk_atomic_ea_t ea)
{
    CTK_DEBUG_ASSERT(ea != NULL);
    ctk_sync_free((void*)CTK_ATOMIC_PTR(ea));
}

#endif /* _CTK_ATOMIC_IMPL_H */
