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

#ifndef _CTK_COND_H
#define _CTK_COND_H

/**
 * @file ctk_cond.h
 * @brief Definitions for simple conditional variable operations
 */
#include "ctk_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * conditional variable structure
 */
typedef struct {
    unsigned int lock;
    unsigned int nwaiters;
    unsigned int bcast_seq;
    unsigned long total_seq;
    unsigned long wakeup_seq;
    unsigned long woken_seq;
} ctk_cond_t __attribute__((aligned(128)));

#if 0 /* libsync implementation - buggy; did not work in some cases */
typedef struct {
    short nsignal;
    short nwaiting;
} ctk_cond_t __attribute__((aligned(16)));
#endif

/** Conditional-variable reference type */
typedef ea_addr_t ctk_cond_ea_t;

/** A macro to get a cond-var reference for the given condition-variable.
 * @param[in] cond_var - a cond variable (ctk_cond_t)
 */
#define CTK_COND_EA(cond_var)  ((ctk_cond_ea_t)(unsigned long)(&(cond_var)))

/** A macro to get a cond-var variable from the given cond reference.
 * @param[in] cond_ea - a cond reference (ctk_cond_ea_t)
 */
#define CTK_COND_VAR(cond_ea)  (*(ctk_cond_t*)(unsigned long)(cond_ea))

/** A macro to get a cond-var pointer from the given cond reference.
 * @param[in] cond_ea - a cond reference (ctk_cond_ea_t)
 */
#define CTK_COND_PTR(cond_ea)  ((ctk_cond_t*)(unsigned long)(cond_ea))

#include "ctk_cond-impl.h"

/**
 * Creates a new cond-var reference.  A new cond-var
 * is allocated (by #ctk_sync_alloc) and its reference
 * is returned.  (You can also directly declares
 * a #ctk_cond_t static variable and gets
 * a reference for it using #CTK_COND_EA macro.)
 * @param[out] cond - a created cond-var reference.
 */
int ctk_cond_create(ctk_cond_ea_t *cond);

/**
 * Destroys the given cond-var reference.  Cond-var references
 * created by #ctk_cond_create must be destroyed by this routine.
 * @param[in] cond - a cond reference to be destroyed.
 */
void ctk_cond_destroy(ctk_cond_ea_t cond);

/**
 * Initializes a conditional variable.
 * @param[in, out] cond - a conditional variable to be initialized.
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

#endif /* _CTK_COND_H */
