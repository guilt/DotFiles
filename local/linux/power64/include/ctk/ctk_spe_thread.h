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

#ifndef _CTK_SPE_THREAD_H
#define _CTK_SPE_THREAD_H

/**
 * @file ctk_spe_thread.h
 * @brief Definitions for CTK SPE thread macros and routines.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "ctk_types.h"
#include "ctk_elf.h"
#include "ctk_spe.h"

#include <pthread.h>
#include <stdbool.h>

#define CTK_SPE_THREAD_MAX_ACCESS_HINTS   CTK_SPE_MAX_ACCESS_HINTS

typedef struct ctk_spe_thread_body {
    unsigned int m_type;
    ea_addr_t next;
    ctk_spe_context_t m_context;
    pthread_t m_pthread;        /**< PPE-side pthread */
    pthread_mutex_t m_pthread_lock;
    void *m_argp, *m_envp;
    unsigned int m_runflags;
    volatile bool m_killed;
    volatile bool m_running;
    unsigned int m_counter;
} ctk_spe_thread_body_t;

/** SPE thread handle */
typedef ctk_spe_thread_body_t *ctk_spe_thread_t;

/*---------------------------------------------------------------------*/
/*
 * for backward-compat
 */
#ifdef SPE_USER_REGS
# define CTK_SPE_USER_REGS           SPE_USER_REGS
#else /* !SPE_USER_REGS */

/** Flag for ctk_spe_thread_create.  (Borrowed from libspe spec.)
 * Specifies that the SPE setup registers r3, r4, and r5 are
 * initialized with the 48 bytes pointed to by argp. */
# define CTK_SPE_USER_REGS           0x0002

#endif /* !SPE_USER_REGS */

/**
 * Flag for ctk_spe_thread_create.
 * Specifies that the created SPE thread should not be started.
 * If the flag is specified the caller should explicitly start
 * the created thread by #ctk_spe_thread_start.
 */
#define CTK_SPE_DONT_START      0x10000

/*---------------------------------------------------------------------*/
/**
 * Creates a new SPE thread from the given SPE program handle
 * and starts the thread with arguments argp (unless the caller specifies
 * #CTK_SPE_DONT_START flag.)
 * @param[out] thread - pointer to store the created SPE thread
 * @param[in] program - SPE program handle
 * @param[in] argp - Argument pointer for the SPE thread
 * @param[in] envp - pointer to environment specific data for the SPE thread
 * @param[in] flags - A bit-wise OR of modifiers that are applied
 *   when the new thread is created.  0 (no flag) or any combination of
 *   #CTK_SPE_USER_REGS, #CTK_SPE_CFG_SIGNOTIFY1_OR, 
 *   #CTK_SPE_CFG_SIGNOTIFY2_OR and #CTK_SPE_DONT_START can be specified.
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_thread_create(ctk_spe_thread_t *thread, ctk_spe_program_t *program, void *argp, void *envp, int flags);

/**
 * Starts the given SPE thread.
 * @param[in] thread - the SPE thread (must be created with 
 *  #CTK_SPE_DONT_START flag.)
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_thread_start(ctk_spe_thread_t thread);

/**
 * Waits until the given SPE thread exits and frees the resources.
 * @param[in] thread - the SPE thread
 * @param[out] status - the SPE exit status.
 *  The return code of the SPE thread can be obtained by a macro
 *  #CTK_SPE_EXIT_STATUS(status) only if #CTK_SPE_IF_EXITED(status) 
 *  returns true.
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_thread_wait(ctk_spe_thread_t thread, int *status);

/**
 * Terminates the given SPE thread and frees the resources. 
 * @note This may not actually stop the SPE execution until it voluntarily
 *  releases the physical SPE.  !!!Do not cleanup PPE resources while there
 *  could be any DMA or synchronization activities!!!
 * @deprecated
 * @param[in] thread - the SPE thread
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_thread_terminate(ctk_spe_thread_t thread);

/**
 * Returns the SPE context associated to the given SPE thread
 * @param[in] thread - the SPE thread
 * @return the SPE context
 */
static inline ctk_spe_context_t ctk_spe_thread_get_context(ctk_spe_thread_t thread) __attribute__((always_inline)); 
static inline ctk_spe_context_t ctk_spe_thread_get_context(ctk_spe_thread_t thread)
{
    return thread->m_context;
}

/**
 * Returns SPE status of the given SPE thread.
 * @param[in] spe - the SPE thread
 * @return either one of #CTK_SPE_STATUS_CREATED, #CTK_SPE_STATUS_LOADED,
 *   #CTK_SPE_STATUS_READY, #CTK_SPE_STATUS_RUNNING, #CTK_SPE_STATUS_STOPPED or
 *   #CTK_SPE_STATUS_UNKNOWN.
 */
static inline int ctk_spe_thread_get_status(ctk_spe_thread_t spe) __attribute__((always_inline)); 
static inline int
ctk_spe_thread_get_status(ctk_spe_thread_t spe)
{
    return ctk_spe_context_get_status(spe->m_context);
}

/*------------------------------------------------------------------------
 * for backward compatibility
 */
/**
 * Maps the LS (local storage) to the EA addr and returns the mapped addr.
 * (equivalent to ctk_spe_map_ls; provided for backward compat)
 * @deprecated
 * @param[in] spe - the SPE thread whose LS is mapped
 * @param[out] map_addr - the mapped address
 * @return CTK_SUCCESS on success, ERRNO on failure.
 * @see #ctk_spe_map_ls
 */
static inline int ctk_spe_thread_map_ls(ctk_spe_thread_t spe, ea_addr_t *map_addr) __attribute__((always_inline)); 
static inline int
ctk_spe_thread_map_ls(ctk_spe_thread_t spe, ea_addr_t *map_addr)
{
    return ctk_spe_context_map_ls(spe->m_context, map_addr);
}

/**
 * Maps the signal notification register 1 for the SPE specified
 * by #spe and returns the mapped addr.
 * (equivalent to ctk_spe_map_signal2; provided for backward compat)
 * @note The address is not DMA aligned (mod 16 = 12).
 * @deprecated
 * @param[in] spe - the SPE whose signal notification register is mapped
 * @param[out] map_addr - the mapped address
 * @return CTK_SUCCESS on success, ERRNO on failure.
 * @see #ctk_spe_map_signal1
 */
static inline int ctk_spe_thread_map_signal1(ctk_spe_thread_t spe, ea_addr_t *map_addr) __attribute__((always_inline)); 
static inline int
ctk_spe_thread_map_signal1(ctk_spe_thread_t spe, ea_addr_t *map_addr)
{
    return ctk_spe_context_map_signal1(spe->m_context, map_addr);
}

/**
 * Maps the signal notification register 2 for the SPE specified by
 * #spe and returns the mapped addr.
 * (equivalent to ctk_spe_map_signal2; provided for backward compat)
 * @note The address is not DMA aligned (mod 16 = 12).
 * @param[in] spe - the SPE thread whose signal notification register is mapped
 * @param[out] map_addr - the mapped address 
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
static inline int ctk_spe_thread_map_signal2(ctk_spe_thread_t spe, ea_addr_t *map_addr) __attribute__((always_inline)); 
static inline int
ctk_spe_thread_map_signal2(ctk_spe_thread_t spe, ea_addr_t *map_addr)
{
    return ctk_spe_context_map_signal2(spe->m_context, map_addr);
}

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SPE_THREAD_H */
