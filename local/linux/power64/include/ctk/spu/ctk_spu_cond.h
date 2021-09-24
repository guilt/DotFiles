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

#ifndef _CTK_SPU_COND_H
#define _CTK_SPU_COND_H

/**
 * @file ctk_spu_cond.h
 * @brief Definitions for simple conditional variable operations
 * @note This incurs number of DMA transfers and could be inefficient.
 */
#include "ctk_spu_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/** conditional variable structure */
typedef struct {
    unsigned int lock;
    unsigned int nwaiters;
    unsigned int bcast_seq;
    unsigned long total_seq;
    unsigned long wakeup_seq;
    unsigned long woken_seq;
    unsigned char dummy;
    unsigned char pad[0] __attribute__((aligned(128)));
} ctk_cond_t __attribute__((aligned(128)));

/** Conditional-variable reference type */
typedef ea_addr_t ctk_cond_ea_t;

#include "ctk_spu_cond-impl.h"

/**
 * Initializes a conditional variable.
 * @param[in] cond - a conditional variable to be initialized.
 */
void ctk_cond_init(ctk_cond_ea_t cond);

/**
 * Waits for a signal or broadcast to wait the condition.
 * @param[in] cond - a conditional variable for which the caller waits.
 * @param[in] mutex - a mutex lock; which must be held before calling this.
 */
void ctk_cond_wait(ctk_cond_ea_t cond, ctk_mutex_ea_t mutex);

/**
 * Signals on a conditional variable.
 * @param[in] cond - a conditional variable to be signaled.
 */
void ctk_cond_signal(ctk_cond_ea_t cond);

/**
 * Signals a broadcast on a conditional variable.
 * @param[in] cond - a conditional variable to be signaled.
 */
void ctk_cond_broadcast(ctk_cond_ea_t cond);

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SPU_COND_H */
