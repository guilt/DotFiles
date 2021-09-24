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

#ifndef _CTK_SPE_THREAD_GROUP_H
#define _CTK_SPE_THREAD_GROUP_H

/**
 * @file ctk_spe_thread_group.h
 * @brief Definitions for CTK SPE thread group macros and routines.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "ctk_types.h"
#include "ctk_env.h"
#include "ctk_simple_list.h"
#include "ctk_spe_thread.h"

/**
 * The system limit of the max number of SPE threads one group can accomodate
 */
#define CTK_SPE_THREAD_GROUP_MAX        32

typedef struct ctk_spe_thread_group_body {
    unsigned int m_type;
    ctk_simple_list_t m_threads;
    ctk_uint32_t m_count;
    ctk_uint32_t m_max;
    ctk_env_t *m_env[CTK_SPE_THREAD_GROUP_MAX];
} ctk_spe_thread_group_body_t;

/** SPE thread group handle */
typedef ctk_spe_thread_group_body_t *ctk_spe_thread_group_t;

/**
 * Creates a new SPE thread group.
 * @param[out] group - pointer to store the created SPE thread group
 * @param[in] max - The max number of SPE threads the group can accomodate
 * @param[in] options - A bit-wise OR of modifiers that are applied
 *   when the new thread group is created.  (Currently there's no
 *   valid modifier is specified)
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_thread_group_create(ctk_spe_thread_group_t *group, int max, int options);

/**
 * Adds a SPE thread to the given SPE thread group.
 * @param[in] group - A SPE thread group
 * @param[in] thread - A SPE thread to be added (NOTE: the given thread
 *   must not be started; i.e., the thread must have been created with
 *   #CTK_SPE_DONT_START flag.)
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_thread_group_add(ctk_spe_thread_group_t group, ctk_spe_thread_t thread);

/**
 * Starts the given SPE thread group.
 * @param[in] group - A SPE thread group
 * @param[out] nstarted - the number of SPE thread actually started
 * @return CTK_SUCCESS if all of the threads in the given group started
 *    successfully, ERRNO otherwise.
 */
int ctk_spe_thread_group_start(ctk_spe_thread_group_t group, int *nstarted);

/**
 * Waits until all of the SPE threads in the given SPE thread group exit
 * and frees the resources.
 * @param[in] group - the SPE thread group
 * @param[out] statuses - a pointer to an array of the SPE exit statuses.
 *   The storage for the array must be allocated by the caller and it
 *   must be large enough to accomodate every exit code of the SPE threads
 *   in the group.
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_thread_group_wait(ctk_spe_thread_group_t group, int *statuses);

/**
 * Terminates all of the SPE threads in the given SPE thread group
 * and frees the resources. 
 * @deprecated
 * @note This may not actually stop the SPE execution until it voluntarily
 *  releases the physical SPEs.  !!!Do not cleanup PPE resources while there
 *  could be any DMA or synchronization activities!!!
 * @param[in] group - the SPE thread group
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_thread_group_terminate(ctk_spe_thread_group_t group);

/**
 * Returns the number of SPE threads in the given group.
 */
static inline int ctk_spe_thread_group_get_threads_count(ctk_spe_thread_group_t group) {
    if (group == NULL || ctk_simple_list_is_empty(&group->m_threads)) return 0;
    return group->m_count;
}

/**
 * Returns an array of SPE threads in the given group. (The array must be
 * allocated by the caller.)
 */
int ctk_spe_thread_group_get_threads(ctk_spe_thread_group_t group, ctk_spe_thread_t *spes);

/**
 * Creates and starts a new SPE thread group which contains a set of
 * SPMD (Single Program Multiple Data) type threads.
 * @param[out] group - pointer to store the created SPE thread group
 * @param[in] nthreads - the number of SPE threads
 * @param[in] program - SPE program handle
 * @param[in] argp - Argument pointer for the SPE thread
 * @param[in] envp - pointer to environment specific data for the SPE thread
 * @param[in] flags - A bit-wise OR of modifiers that are applied
 *   when the new thread is created.  0 (no flag) or any combination of
 *   #CTK_SPE_USER_REGS, #CTK_SPE_CFG_SIGNOTIFY1_OR, 
 *   #CTK_SPE_CFG_SIGNOTIFY2_OR and #CTK_SPE_DONT_START can be specified.
 * @return CTK_SUCCESS if all of the threads in the given group started
 *    successfully, ERRNO otherwise. 
 *    Call #ctk_spe_thread_group_get_threads_count to get the current
 *    number of active (i.e., actually created) SPE threads.
 */
int ctk_spe_thread_group_create_spmd(ctk_spe_thread_group_t *group, int nthreads, ctk_spe_program_t *program, void *argp, void *envp, int flags);

/**
 * Adds access hints to the thread group
 */
int ctk_spe_thread_group_add_access_hint(ctk_spe_thread_group_t group, void *addr, ea_size_t size);

/**
 * Removes access hints from the thread group
 */
int ctk_spe_thread_group_remove_access_hint(ctk_spe_thread_group_t group, void *addr);

/*-----------------------------------------------------------------------
 * group read/write routines
 */
#include "ctk_signal.h"
#include "ctk_mbox.h"

/**
 * Writes the #data to the signal notification register specified by
 * #reg for the threads in the given SPE thread group.
 * @param[in] spe - the SPE thread group
 * @param[in] reg - the signal register. This can be one of either
 *   #CTK_SIGNAL_REG1 or #CTK_SIGNAL_REG2.
 * @param[in] data - the 32-bit data to be written
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_signal_group_write(ctk_spe_thread_group_t spe, unsigned int reg, unsigned int data);

/**
 * Writes the #data to the signal notification register 1
 * for the threads in the given SPE thread group.
 * @param[in] spe - the SPE whose outbound mailbox is to be read.
 * @param[in] data - the 32-bit data to be written
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
static inline int ctk_signal_group_write_reg1(ctk_spe_thread_group_t spe, unsigned int data) __attribute__((always_inline)); 
static inline int
ctk_signal_group_write_reg1(ctk_spe_thread_group_t spe, unsigned int data)
{
    return ctk_signal_group_write(spe, CTK_SIGNAL_REG1, data);
}

/**
 * Writes the #data to the signal notification register 2
 * for the threads in the given SPE thread group.
 * @param[in] spe - the SPE whose outbound mailbox is to be read.
 * @param[in] data - the 32-bit data to be written
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
static inline int ctk_signal_group_write_reg2(ctk_spe_thread_group_t spe, unsigned int data) __attribute__((always_inline)); 
static inline int
ctk_signal_group_write_reg2(ctk_spe_thread_group_t spe, unsigned int data)
{
    return ctk_signal_group_write(spe, CTK_SIGNAL_REG2, data);
}

/**
 * Writes the 32-bit data specified by #data into the SPU inbound
 * mailbox for the threads in the given SPE thread group. [Non-blocking]
 * If the mailbox is full, then ctk_mbox_write may overwrite the last
 * entry in the mailbox. The ctk_mbox_write_count function can be called to
 * ensure the space is available prior to writing to the inbound mailbox.
 * @param[in] group - the SPE thread group
 * @param[in] data - the value to be written.
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_mbox_group_write(ctk_spe_thread_group_t group, ctk_uint32_t data);

/**
 * Reads the contents of the SPU outbound mailbox for the threads
 * in the given SPE thread group.
 * This blocks if the mailbox is empty. [Blocking]
 * @param[in] group - the SPE thread group
 * @param[out] data - array of the value of the Mailbox.
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_mbox_group_read_block(ctk_spe_thread_group_t group, ctk_uint32_t data[]);

/**
 * Reads the contents of the SPU outbound interrupt mailbox for the threads
 * in the given SPE thread group.
 * This blocks if the mailbox is empty. [Blocking]
 * @param[in] group - the SPE thread group
 * @param[out] data - array of the value of the Mailbox.
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_intr_mbox_group_read_block(ctk_spe_thread_group_t group, ctk_uint32_t data[]);

/**
 * Writes the 32-bit data specified by #data into the SPU inbound
 * mailbox for the threads in the given SPE thread group. [Blocking]
 * The call blocks if the mailbox is full.
 * @param[in] group - the SPE thread group
 * @param[in] data - the value to be written.
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_mbox_group_write_block(ctk_spe_thread_group_t group, ctk_uint32_t data);

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SPE_THREAD_GROUP_H */
