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

#ifndef _CTK_SPU_ATOMIC_H
#define _CTK_SPU_ATOMIC_H

/** 
 * @file ctk_spu_atomic.h
 * @brief Definitions for SPU-side atomic operations
 * @note atomics are held on the main memory and PPE must prepare the buffer
 *  before SPUs use them.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "ctk_spu_common.h"
#include "ctk_spu_dma.h"

/** 
 * atomic reference type 
 */
typedef ea_addr_t ctk_atomic_ea_t;

#include "ctk_spu_atomic-impl.h"

/** Atomically set the value #val and return the old value */
int ctk_atomic_set(ctk_atomic_ea_t v, int val);

/** Atomically add the value #val and return the old value */
int ctk_atomic_add(ctk_atomic_ea_t v, int val);

/** Atomically add the value #val and return the old value */
int ctk_atomic_sub(ctk_atomic_ea_t v, int val);

/** Atomically increment the value and return the old value */
int ctk_atomic_inc(ctk_atomic_ea_t v);

/** Atomically decrement the value and return the old value */
int ctk_atomic_dec(ctk_atomic_ea_t v);

/** Atomically read the value, clear the memory and return the old value */
int ctk_atomic_read_and_clear(ctk_atomic_ea_t v);

/** Reads the value */
static inline int ctk_atomic_get_value(ctk_atomic_ea_t v) {
    return _ctk_atomic_get_value(v);
}

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SPU_ATOMIC_H */
