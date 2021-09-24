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

#ifndef _CTK_SPE_RUNTIME_H
#define _CTK_SPE_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ctk_common.h"
#include "ctk_spe_task.h"
#include "ctk_spe_thread_group.h"
#include "ctk_atomic.h"
#include "ctk_queue.h"
#include "ctk_list.h"

#define MAX_NR_SPE      CTK_SPE_THREAD_GROUP_MAX
#define MAX_NR_SPE_TASKS_PER_RUNTIME    64

typedef struct {
    ctk_queue_ea_t runq;
    volatile ctk_val64_t spe_task_bits;
    volatile ctk_val64_t spe_detached_task_bits;
    ea_addr_t spe_tasks_ea;
    ctk_spe_thread_group_t spe_group;
    char dummy[] __attribute__((aligned(128)));
} ctk_spe_runtime_body_t;

/**
 * The SPE task runtime
 */
typedef ctk_spe_runtime_body_t *ctk_spe_runtime_t;

#define CTK_SPE_RUNTIME_TASK_FIFO      0

/**
 * Creates a new runtime for SPE tasks.
 * @param[out] runtime - pointer to store the created runtime
 * @param[in] nspe - the number of SPE contexts to be occupied
 * @param[in] qsize - the number of SPE tasks the 'run queue' of the runtime
 *   can accomodate.  This number need not be an actual number of SPE tasks
 *   this runtime may execute, since the runtime queue only need to contain
 *   waiting SPE tasks to be scheduled.  Adding new tasks when the run queue
 *   is full would block until someone dequeues tasks.
 * @param[in] type - the runtime type.  '0' means the default type.
 *   Currently only #CTK_SPE_RUNTIME_TASK_FIFO (= default) is supported.
 * @param[in] options - the options for the runtime.  The valid options may
 *   vary depending on the specified runtime type.  (Currently no valid runtime
 *   options, so just ignore it for now.)
 */
int ctk_spe_runtime_create(ctk_spe_runtime_t *runtime, int nspe, int qsize, int type, int options);

/**
 * Waits until all tasks in this runtime exit and releases the resources
 * used by the runtime.  (Just to wait all the tasks, consider use of
 * #ctk_spe_runtime_wakt_all instead.)
 * @note This does not release (destroy) the tasks.  The app must explicitly
 *   destroy them by calling #ctk_spe_task_destroy.
 * @note this could block if there's any task failure.
 * @param[in] runtime - the runtime
 */
int ctk_spe_runtime_wait(ctk_spe_runtime_t runtime);

/**
 * Shutdown the SPE tasks, waits until all the tasks exit and releases the
 * resources used by the runtime. 
 * @note This does not release (destroy) the tasks.  The app must explicitly
 *   destroy them by calling #ctk_spe_task_destroy.
 * @note This will not return until all the tasks added to the runtime exit.
 * @param[in] runtime - the runtime
 */
int ctk_spe_runtime_shutdown(ctk_spe_runtime_t runtime);

/**
 * Enqueus a new SPE task (created by #ctk_spe_task_create) to the run
 * queue of the given runtime.
 * The added task will be queued and may start immediately if there's no
 * waiting tasks.
 * @param[in] runtime - the runtime
 * @param[in] task - the task to be added
 * @param[in] arg1 - the runtime argument for the task (passed as the 2nd arg
 *   of ctk_task_main of the SPE task).
 * @param[in] arg2 - the runtime argument for the task (passed as the 3rd arg
 *   of ctk_task_main of the SPE task).
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_spe_runtime_enq_task(ctk_spe_runtime_t runtime, ctk_spe_task_t task, void *arg1, void *arg2);

/**
 * Waits until the given task finishes.
 * @param[in] runtime - the runtime where the task is enqueued.
 * @param[in] task - the task to be waited.
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_spe_runtime_wait_task(ctk_spe_runtime_t runtime, ctk_spe_task_t task);

/* no-yield version */
int ctk_spe_runtime_wait_task_spin(ctk_spe_runtime_t runtime, ctk_spe_task_t task);

/**
 * Invokes a task in the given runtime (i.e., enqueues the task and wait it).
 * @param[in] runtime - the runtime where the task is invoked.
 * @param[in] task - the task to be enqueued and waited.
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
static inline int ctk_spe_runtime_invoke_task(ctk_spe_runtime_t runtime, ctk_spe_task_t task, void *arg1, void *arg2) __attribute__((always_inline));
static inline int ctk_spe_runtime_invoke_task(ctk_spe_runtime_t runtime, ctk_spe_task_t task, void *arg1, void *arg2)
{
    int ret = ctk_spe_runtime_enq_task(runtime, task, arg1, arg2);
    if (ret != CTK_SUCCESS)
        return ret;
    return ctk_spe_runtime_wait_task_spin(runtime, task);
}

/**
 * Waits until ALL of the tasks specified by #id_mask exit.
 * @param[in] runtime
 * @param[in] id_mask - a task ID mask to be waited (the task mask
 *  can be #CTK_SPE_TASK_MASK_ALL (for all tasks) or an arbitrary
 *  mask that can be created by #ctk_spe_task_mask_set, 
 *  #ctk_spe_task_mask_clear or #ctk_spe_task_mask_zero).
 * @return CTK_SUCCESS (0) on success, errno on failure.
 * @note this could block if there's any task failure.
 */
int ctk_spe_runtime_wait_all(ctk_spe_runtime_t runtime, ctk_uint64_t id_mask);

/**
 * Waits until ANY one of the tasks specified by #id_mask exits.
 * @param[in] runtime
 * @param[out] task - a finished task
 * @param[in] id_mask - a task ID mask to be waited (the task mask
 *  can be #CTK_SPE_TASK_MASK_ALL (for all tasks) or an arbitrary
 *  mask that can be created by #ctk_spe_task_mask_set, 
 *  #ctk_spe_task_mask_clear or #ctk_spe_task_mask_zero).
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_spe_runtime_wait_any(ctk_spe_runtime_t runtime, ctk_uint64_t id_mask,
    ctk_spe_task_t *task);

/**
 * Waits until ANY one of the tasks specified by #id_mask exits or
 *  the given #timeout expires.
 * @param[in] runtime
 * @param[out] task - a finished task
 * @param[in] id_mask - a task ID mask to be waited (the task mask
 *  can be #CTK_SPE_TASK_MASK_ALL (for all tasks) or an arbitrary
 *  mask that can be created by #ctk_spe_task_mask_set, 
 *  #ctk_spe_task_mask_clear or #ctk_spe_task_mask_zero).
 * @param[in] timeout - the timeout in milli second
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_spe_runtime_wait_any_timeout(ctk_spe_runtime_t runtime,
    ctk_uint64_t id_mask, ctk_spe_task_t *task, int timeout);

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SPE_RUNTIME_H */
