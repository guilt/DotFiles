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

#ifndef _CTK_SPU_BARRIER_H
#define _CTK_SPU_BARRIER_H

/**
 * @file ctk_spu_barrier.h
 * @brief Definitions for SPU-side barrier operations 
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "ctk_spu_atomic.h"

/** barrier structure */
typedef struct {
    unsigned int total:16;
    unsigned int count:16;
} ctk_barrier_t __attribute__((aligned(128)));

/** 
 * barrier reference type 
 */
typedef ea_addr_t ctk_barrier_ea_t;

#include "ctk_spu_barrier-impl.h"

/**
 * Initializes the given barrier.
 * @param[in, out] barrier - barrier to be initialized.
 * @param[in] value - initial value for the barrier.
 * @param[in] count - the number of threads that must call
 *  #ctk_barrier_wait before any of them successfully return from
 *  the call.
 * @return CTK_SUCCESS (0) on success, errno otherwise.
 */
int ctk_barrier_init(ctk_barrier_ea_t barrier, unsigned int count);

/**
 * Blocks until all the participating threads at the #barrier have
 * called #ctk_barrier_wait.
 * @param[in] barrier - the barrier reference
 */
void ctk_barrier_wait(ctk_barrier_ea_t barrier);

#ifdef __cplusplus
};
#endif

#endif /* _CTK_SPU_BARRIER_H */
