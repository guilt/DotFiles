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

#ifndef _CTK_SPU_TASK_SYSCALLS_H
#define _CTK_SPU_TASK_SYSCALLS_H

/**
 * @file ctk_spu_task_syscalls.h - Definitions of task syscalls
 */

#if defined(UNIT_TEST) || defined(_CTKLIB)
# include "ctk_version.h"
# include "../ctk_spe_task_syscalls.h"
# include "../ctk_spe_task_common.h"
# include "../../sync/spu/ctk_spu_sync_common.h"
#else
# include "ctk/ctk_version.h"
# include "ctk/ctk_spe_task_syscalls.h"
# include "ctk/ctk_spe_task_common.h"
# include "ctk_spu_sync.h"
#endif

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __SPU__

typedef int (*ctk_spe_task_call_t)(qword, qword, qword);

#define CTK_SPE_TASK_CALLTABLE  ((ctk_spe_task_call_t*)CTK_SPE_TASK_CALL_ADDR)

/**
 * Returns a pointer of my task info
 */
#define ctk_task_get_task_info() \
    ((ctk_spe_task_info_t*)(ctk_env.env[CTK_ENV_ID_TASKINFO].val32[0]))

/**
 * Returns my task ID
 */
#define ctk_task_get_task_id() ctk_task_get_task_info()->id

/**
 * Yield the SPE ask and may switch to the new task
 * @return 0 on success, -1 on error
 */
static inline int ctk_task_yield() __attribute__((always_inline));
static inline int ctk_task_yield() {
    if (ctk_task_get_task_info() != 0 &&
        (ctk_task_get_task_info()->flag & CTK_SPE_TASK_SAVE) == 0) {
        // We cannot save this task!
        return -1;
    }
    return CTK_SPE_TASK_CALLTABLE[CTK_SPE_TASK_YIELD_NR]
            (si_from_uint((int)sbrk(0)), si_from_int(0), si_from_int(0));
}

/**
 * Exit the current SPE task (note: if the application directly calls
 * exit() instead of #ctk_task_exit, the SPE task runtime itself exits.
 * Do not call exit() directly unless you are aware of what you are doing.)
 * @param[in] ret - the return code
 */
static inline int ctk_task_exit(unsigned long long ret) __attribute__((always_inline));
static inline int ctk_task_exit(unsigned long long ret) {
    return CTK_SPE_TASK_CALLTABLE[CTK_SPE_TASK_EXIT_NR](
            si_from_ullong(ret), si_from_int(0), si_from_int(0));
}

/**
 * Execs a new SPE task on this SPE.  The current task will be overwritten
 * and never returns from the call.
 * @param[in] image - the task image (created by #ctk_task_image_create
 *   or #ctk_task_image_open)
 * @param[in] arg1 - the 2nd arg for the new task
 * @param[in] arg2 - the 3rd arg for the new task
 */
static inline int ctk_task_exec(ea_addr_t image, unsigned long long arg1, unsigned long long arg2) {
    return CTK_SPE_TASK_CALLTABLE[CTK_SPE_TASK_EXEC_NR](
            si_from_ullong(image), si_from_ullong(arg1), si_from_ullong(arg2));
}

/**
 * Spawns a new SPE task on this SPE.  The new created task will be enqueued
 * to the run queue and eventually called.
 * @param[in] task - the task reference, which should not already be running
 *  or in the run queue.
 * @param[in] arg1 - the 2nd arg for the new task
 * @param[in] arg2 - the 3rd arg for the new task
 */
static inline int ctk_task_spawn(ea_addr_t task, unsigned long long arg1, unsigned long long arg2) __attribute__((always_inline));
static inline int ctk_task_spawn(ea_addr_t task, unsigned long long arg1, unsigned long long arg2) {
    return CTK_SPE_TASK_CALLTABLE[CTK_SPE_TASK_SPAWN_NR](
            si_from_ullong(task), si_from_ullong(arg1), si_from_ullong(arg2));
}

/**
 * Waits at a barrier - please use this one instead of ctk_barrier_wait
 * if synchronizing SPE tasks may not run parallelly on the multiple
 * SPEs (thus needs yield before blocking at the barrier).
 */
static inline int ctk_task_barrier_wait(ctk_barrier_ea_t barrier) __attribute__((always_inline));
static inline int ctk_task_barrier_wait(ctk_barrier_ea_t barrier)
{
    if (_ctk_barrier_trywait(barrier) != CTK_SUCCESS) {
        ctk_task_yield();
    }
    while (_ctk_barrier_waitonly(barrier) != CTK_SUCCESS) {
        ctk_task_yield();
    }
    return CTK_SUCCESS;
}

/**
 * Enqueues on a queue with yield - if enqueue fails, it yields the SPE
 * resource to other SPE tasks
 */
static inline int ctk_task_queue_enq(ctk_queue_ea_t queue, ctk_queue_item_t item) __attribute__((always_inline));
static inline int ctk_task_queue_enq(ctk_queue_ea_t queue, ctk_queue_item_t item)
{
    int ret;
    while ((ret = _ctk_queue_tryenq(queue, item)) == CTK_ERROR_RETRY) {
        ctk_task_yield();
    }
    return ret;
}

/**
 * Deques on a queue with yield - if dequeue fails, it yields the SPE 
 * resource to other SPE tasks
 */
static inline int ctk_task_queue_deq(ctk_queue_ea_t queue, ctk_queue_item_t *item) __attribute__((always_inline));
static inline int ctk_task_queue_deq(ctk_queue_ea_t queue, ctk_queue_item_t *item)
{
    int ret;
    while ((ret = _ctk_queue_trydeq(queue, item)) == CTK_ERROR_RETRY) {
        ctk_task_yield();
    }
    return ret;
}

/**
 * Enqueues on a generic queue with yield - if enqueue fails, it yields
 * the SPE resource to other SPE tasks
 */
static inline int ctk_task_genericq_enq(ctk_genericq_ea_t queue, void *item) __attribute__((always_inline));
static inline int ctk_task_genericq_enq(ctk_genericq_ea_t queue, void *item)
{
    int ret;
    while ((ret = _ctk_genericq_tryenq(queue, item)) == CTK_ERROR_RETRY) {
        ctk_task_yield();
    }
    return ret;
}

/**
 * Deques on a generic queue with yield - if dequeue fails, it yields 
 * the SPE resource to other SPE tasks
 */
static inline int ctk_task_genericq_deq(ctk_genericq_ea_t queue, void *item) __attribute__((always_inline));
static inline int ctk_task_genericq_deq(ctk_genericq_ea_t queue, void *item)
{
    int ret;
    while ((ret = _ctk_genericq_trydeq(queue, item)) == CTK_ERROR_RETRY) {
        ctk_task_yield();
    }
    return ret;
}

/**
 * Initializes the task environment and returns my task ID
 */
static inline int ctk_task_init_env(unsigned long long runtime_envp) __attribute__((always_inline));
static inline int ctk_task_init_env(unsigned long long runtime_envp)
{
    // arg1: runtime_env addr
    ctk_env_value_t *envp = (ctk_env_value_t*)(ls_addr_t)runtime_envp;
    ctk_env.env[CTK_ENV_ID_RANK].val32[0] = envp[CTK_ENV_ID_RANK].val32[0];
    ctk_env.env[CTK_ENV_ID_TASKINFO].val32[0]
        = envp[CTK_ENV_ID_TASKINFO].val32[0];
    ctk_timebase_clock = envp[CTK_ENV_ID_TIMEBASE].val32[0];
    return ctk_task_get_task_info()->id;
}

/* to return with 128-bits value */
static inline int ctk_task_exit_128(qword ret) __attribute__((always_inline));
static inline int ctk_task_exit_128(qword ret) {
    return CTK_SPE_TASK_CALLTABLE[CTK_SPE_TASK_EXIT_NR](
            ret, si_from_int(0), si_from_int(0));
}

#endif /* __SPU__ */

#ifdef __cplusplus
};
#endif

#endif /* _CTK_SPU_TASK_SYSCALLS_H */
