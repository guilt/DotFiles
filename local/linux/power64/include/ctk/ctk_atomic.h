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

#ifndef _CTK_ATOMIC_H
#define _CTK_ATOMIC_H

/**
 * @file ctk_atomic.h
 * @brief Definitions for atomic operations 
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "ctk_types.h"

/** atomic type (must be aligned in a 128-bit line) */
typedef volatile signed int ctk_atomic_t __attribute__((aligned(128)));

/** 64-bit atomic type (must be aligned in a 128-bit line) */
typedef volatile signed long ctk_atomic64_t __attribute__((aligned(128)));

/** atomic EA type */
typedef ea_addr_t ctk_atomic_ea_t;

/** A macro to get an atomic EA for the given atomic variable.
 * @param[in] atomic_var - an atomic variable (ctk_atomic_t)
 */
#define CTK_ATOMIC_EA(atomic_var) \
    ((ctk_atomic_ea_t)(unsigned long)(&(atomic_var)))

/** A macro to get an atomic variable from the given atomic reference.
 * @param[in] atomic_ea - an atomic reference (ctk_atomic_ea_t)
 */
#define CTK_ATOMIC_VAR(atomic_ea)  \
    (*(ctk_atomic_t*)(unsigned long)(atomic_ea))

/** A macro to get a pointer to the atomic variable from the
 * given atomic reference.
 * @param[in] atomic_ea - an atomic reference (ctk_atomic_ea_t)
 */
#define CTK_ATOMIC_PTR(atomic_ea)  \
    ((ctk_atomic_t*)(unsigned long)(atomic_ea))

/** A macro to get a 64-bit atomic variable from the given atomic reference.
 * @param[in] atomic_ea - an atomic reference (ctk_atomic_ea_t)
 */
#define CTK_ATOMIC_VAR64(atomic_ea)  \
    (*(ctk_atomic64_t*)(unsigned long)(atomic_ea))

/** A macro to get a pointer to a 64-bit atomic variable from the
 * given atomic reference.
 * @param[in] atomic_ea - an atomic reference (ctk_atomic_ea_t)
 */
#define CTK_ATOMIC_PTR64(atomic_ea)  \
    ((ctk_atomic64_t*)(unsigned long)(atomic_ea))

#include "ctk_atomic-impl.h"

/** Atomically set the value #val and return the old value */
int ctk_atomic_set(ctk_atomic_ea_t v, int val);

/** Atomically add the value and return the old value */
int ctk_atomic_add(ctk_atomic_ea_t v, int val);

/** Atomically subtract the value and return the old value */
int ctk_atomic_sub(ctk_atomic_ea_t v, int val);

/** Atomically read the value, clear the memory and return the old value */
int ctk_atomic_read_and_clear(ctk_atomic_ea_t v);

/** Atomically increment the value and return the old value */
int ctk_atomic_inc(ctk_atomic_ea_t v);

/** Atomically decrement the value and return the old value */
int ctk_atomic_dec(ctk_atomic_ea_t v);

/** Reads and returns the value */
inline static int ctk_atomic_get_value(ctk_atomic_ea_t v) {
    return _ctk_atomic_get_value(v);
}

/** 32 or 64-bit ('addr' size) CAS (compare-and-swap) operation 
    (if (*p == oldval) *p = newval) */
static inline int ctk_atomic_compare_and_swap(long int *p, long int oldval, long int newval) {
    return _ctk_atomic_compare_and_swap(p, oldval, newval);
}

/** 32-bit CAS (compare-and-swap) operation (if (*p == oldval) *p = newval) */
static inline int ctk_atomic_compare_and_swap32(int *p, int oldval, int newval) {
    return _ctk_atomic_compare_and_swap32(p, oldval, newval);
}

#ifdef __cplusplus
}
#endif

#endif /* _CTK_ATOMIC_H */
