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

#ifndef _CTK_SPE_TASK_H
#define _CTK_SPE_TASK_H

#include "ctk_version.h"
#include "ctk_common.h"
#include "ctk_spe.h"
#include "ctk_spe_task_common.h"
#include "ctk_spe_task_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CTK_SPE_TASK_BAD_ID     ((ctk_uint16_t)-1);

/**
 * SPE task type 
 */
typedef ctk_spe_task_info_t *ctk_spe_task_t;
// typedef ea_addr_t ctk_spe_task_t;

/** SPE task mask type (for #ctk_spe_runtime_wait_all and 
  * #ctk_spe_runtime_wait_any) */
typedef ctk_uint64_t ctk_spe_task_mask_t;

/** SPE task mask for all tasks */
#define CTK_SPE_TASK_MASK_ALL       ((ctk_spe_task_mask_t)-1)

/** SPE task mask for 0 task */
#define CTK_SPE_TASK_MASK_ZERO      ((ctk_spe_task_mask_t)0)

/**
 * Sets (adds) a task ID to the given task mask
 */
static inline void ctk_spe_task_mask_set(ctk_spe_task_mask_t *mask, int task_id) {
    *mask |= (1ULL << (63 - task_id));
}

/**
 * Clears (removes) a task ID from the given task mask
 */
static inline void ctk_spe_task_mask_clear(ctk_spe_task_mask_t *mask, int task_id) {
    *mask &= ~(1ULL << (63 - task_id));
}

/**
 * All zeros the given task mask
 */
static inline void ctk_spe_task_mask_zero(ctk_spe_task_mask_t *mask) {
    *mask = CTK_SPE_TASK_MASK_ZERO;
}

/**
 * Creates a new SPE task by the given SPE program image.
 * @param[out] task - the created task
 * @param[in] program - the SPE program image from which the task is created
 * @param[in] flag - a bit-wise OR of modifiers that are applied when the
 *    new task is created.  0 (no flag) or #CTK_SPE_TASK_SAVE can be 
 *    specified.  If the flag #CTK_SPE_TASK_SAVE is specified, an extra
 *    memory space is allocated for the created task so that the task
 *    can be swapped in/out to/from the SPE's LS.
 * @return CTK_SUCCESS on success, ERRNO on failure.
 *
 * @note Consider using #ctk_spe_task_create_by_image instead of using
 *    this API.  #ctk_spe_task_create_by_image allow you to save task
 *    image creation overhead in terms of both time and space. 
 *    To create a task image from a SPE program image, use 
 *    #ctk_spe_task_image_create.
 */
int ctk_spe_task_create(ctk_spe_task_t *task, ctk_spe_program_t *program, int flag);

/**
 * Destroys the given task.
 * @param[in] task - the task to be destroyed.
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_task_destroy(ctk_spe_task_t task);

/**
 * Returns the exit code of the given task.  The task must have exited
 * by #ctk_task_exit to properly return the exit code to the PPE.
 */
static inline int ctk_spe_task_get_exit_code(ctk_spe_task_t task, int *rc) __attribute__((always_inline));
static inline int ctk_spe_task_get_exit_code(ctk_spe_task_t task, int *rc)
{
    if (task == NULL)
        return CTK_ERROR_NULL_OBJECT;
    if (task->status < CTK_SPE_TASK_STATUS_FINISHED) 
        return CTK_ERROR_BAD_SPE_STATUS;        /* not yet finished */
    *rc = task->rc.val32[1];
    return CTK_SUCCESS;
}

/**
 * Re-initializes the given SPE task.  This is necessary only if
 * the task is reused.
 */
static inline int ctk_spe_task_init(ctk_spe_task_t task) __attribute__((always_inline));
static inline int ctk_spe_task_init(ctk_spe_task_t task)
{
    if (task == NULL)
        return CTK_ERROR_NULL_OBJECT;
    task->status = CTK_SPE_TASK_STATUS_INITIALIZED;
    return CTK_SUCCESS;
}

/**
 * Returns the task ID.  (The task must have been enqueued to a 
 * SPE runtime by #ctk_spe_runtime_enq_task)
 * @param[in] task - the task for which the info is retrieved.
 */
static inline int ctk_spe_task_get_id(ctk_spe_task_t task) __attribute__((always_inline));
static inline int ctk_spe_task_get_id(ctk_spe_task_t task)
{
    return task->id;
}

/**
 * Returns the arguments passed to the task. (Can be used
 * to check the task's given parameters after it finishes)
 * @param[in] task - the task for which the info is retrieved
 * @param[in] index - 0 or 1
 */
static inline void *ctk_spe_task_get_arg(ctk_spe_task_t task, int index) __attribute__((always_inline));
static inline void *ctk_spe_task_get_arg(ctk_spe_task_t task, int index)
{
    return (void*)(unsigned long)task->arg[index];
}

/**
 * Returns the task status.
 * The task status can be one of either #CTK_SPE_TASK_STATUS_INITIALIZED,
 *  #CTK_SPE_TASK_STATUS_QUEUED, #CTK_SPE_TASK_STATUS_RUNNABLE or
 *  #CTK_SPE_TASK_STATUS_FINISHED.
 */
static inline int ctk_spe_task_get_status(ctk_spe_task_t task) __attribute__((always_inline));
static inline int ctk_spe_task_get_status(ctk_spe_task_t task)
{
    return task->status;
}

/* Task image APIs ----------------------------------------------------- */
/**
 * Creates a new SPE task image
 * @param[out] image - the SPE task image to be created
 * @param[in] program - the SPE program image  (elf image)
 * @return CTK_SUCCESS (0) on success, errno on failure
 *
 * @note Tasks can be created directly from a SPE program image,
 *    but if you're creating multiple SPE tasks from the same
 *    SPE program, you should consider creating the SPE task image 
 *    for the SPE program and call #ctk_spe_task_create_by_image 
 *    instead of #ctk_spe_task_create.  (Creating tasks from task 
 *    images is much more efficient than creating tasks from SPE
 *    program images.)
 */
int ctk_spe_task_image_create(ctk_spe_task_image_t *image, ctk_spe_program_t *program);

/**
 * Opens a new SPE task image
 * @param[out] image - the SPE task image to be created
 * @param[in] fname - the SPE program file name which contains 
 *      SPE task elf image 
 * @return CTK_SUCCESS (0) on success, errno on failure
 */
int ctk_spe_task_image_open(ctk_spe_task_image_t *image, const char *fname);

/**
 * Destroyes the SPE task image
 * @param[in] image - the SPE task image to be destroyed
 * @return CTK_SUCCESS (0) on success, errno on failure
 */
int ctk_spe_task_image_destroy(ctk_spe_task_image_t image);

/**
 * Closes the SPE task image (equivalent to ctk_spe_task_image_destroy)
 * @param[in] image - the SPE task image to be destroyed
 * @return CTK_SUCCESS (0) on success, errno on failure
 */
#define ctk_spe_task_image_close(image) ctk_spe_task_image_destroy(image)

#define ctk_spe_task_image_add_ref(image)  _ctk_atomic_inc(EAADDR(image))
#define ctk_spe_task_image_release(image)  _ctk_atomic_dec(EAADDR(image))

/* advanced APIs ------------------------------------------------------- */
/**
 * Creates a new SPE task by the given task image.
 * @param[out] task - the created task
 * @param[in] image - the SPE task image from which the task is created
 * @param[in] flag - a bit-wise OR of modifiers that are applied when the
 *    new task is created.  0 (no flag) or #CTK_SPE_TASK_SAVE can be 
 *    specified.  If the flag #CTK_SPE_TASK_SAVE is specified, an extra
 *    memory space is allocated for the created task so that the task
 *    can be swapped in/out to/from the SPE's LS.
 * @return CTK_SUCCESS on success, ERRNO on failure.
 * @see also #ctk_spe_task_image_create, #ctk_spe_task_create
 */
int ctk_spe_task_create_by_image(ctk_spe_task_t *task, ctk_spe_task_image_t image, int flag);

#if 0
/**
 * Creates a new task from another task (i.e. copy constructor)
 */
int ctk_spe_task_create_by_task(ctk_spe_task_t *task, ctk_spe_task_t another);
#endif

#ifdef __cplusplus
};
#endif

#endif /* _CTK_SPE_TASK_H */
