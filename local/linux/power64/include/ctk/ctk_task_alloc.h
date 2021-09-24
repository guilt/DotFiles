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

#ifndef _CTK_TASK_ALLOC_H
#define _CTK_TASK_ALLOC_H

/**
 * @file ctk_task_alloc.h
 * @brief Definitions of memory allocator for task objects.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Task object types */
enum {
    CTK_TASK_OBJECT = 50,    /**< Task object type - Any type */
    CTK_TASK_TASK,           /**< Task object type - SPE task */
    CTK_TASK_RUNTIME,        /**< Task object type - runtime */
    CTK_TASK_NUM_TYPES       /* dummy */
};
/** The maximum number of the task type ID */
#define CTK_TASK_MAX    (CTK_TASK_NUM_TYPES - 1)

/** Memory allocator function type for task modules
    (Takes task object type and memory size to allocate,
      and returns the allocated pointer) */
typedef void *(*ctk_task_alloc_func_t)(int obj_type, size_t size);

/** Memory free function type for task modules
    (Takes memory pointer and frees it) */
typedef void (*ctk_task_free_func_t)(void *ptr);

/** Aligned memory allocator function type for task modules
    (Takes task object type, memory size to allocate and
    align size. Returns the allocated pointer) */
typedef void *(*ctk_task_alloc_align_func_t)(int obj_type, size_t size,
    int align);

/** Aligned memory free function type for task modules
    (Takes memory pointer and frees it) */
typedef void (*ctk_task_free_align_func_t)(void *ptr);

/** A memory allocator for task objects; you can change the default
    memory allocator by #ctk_task_set_alloc_func.  */
extern ctk_task_alloc_func_t ctk_task_alloc;
/** A memory free function for task objects; you can change the default
    memory free routine by #ctk_task_set_free_func. */
extern ctk_task_free_func_t ctk_task_free;

/** An aligned memory allocator for task objects; you can change the default
    memory allocator by #ctk_task_set_alloc_align_func.  */
extern ctk_task_alloc_align_func_t ctk_task_alloc_align;
/** An aligned memory free function for task objects; you can change the
    default memory free routine by #ctk_task_set_free_align_func. */
extern ctk_task_free_align_func_t ctk_task_free_align;

/**
 * Sets a new memory allocator for task objects (e.g., hash, thread, etc.).
 * The allocator routine should be of ctk_task_alloc_func_t, i.e., takes
 * two params task_type and size and returns a pointer to the allocated region.
 * @param[in] allocator - the new memory allocator
 * @return the old memory allocator
 */
ctk_task_alloc_func_t ctk_task_set_alloc_func(ctk_task_alloc_func_t allocator);

/**
 * Returns the current memory allocator for task objects
 * (e.g., mutex, semaphore).
 * @return the current memory allocator
 */
ctk_task_alloc_func_t ctk_task_get_alloc_func();

/**
 * Sets a new memory free_func for task objects.
 * The free_func routine should be of ctk_task_free_func_t, i.e., takes
 * two params task_type and size and returns a pointer to the freeated region.
 * @param[in] free_func - the new memory free_func
 * @return the old memory free_func
 */
ctk_task_free_func_t ctk_task_set_free_func(ctk_task_free_func_t free_func);

/**
 * Returns the current memory free_func for task objects.
 * @return the current memory free_func
 */
ctk_task_free_func_t ctk_task_get_free_func();

/**
 * Sets a new aligned-memory allocator for task objects.
 * The allocator routine should be of #ctk_task_alloc_align_func_t.
 * @param[in] allocator - the new memory allocator
 * @return the old memory allocator
 */
ctk_task_alloc_align_func_t ctk_task_set_alloc_align_func(ctk_task_alloc_align_func_t allocator);

/**
 * Returns the current aligned-memory allocator for task objects.
 * @return the current memory allocator
 */
ctk_task_alloc_align_func_t ctk_task_get_alloc_align_func();

/**
 * Sets a new aligned memory free_func for task objects.
 * The free_func routine should be of ctk_task_free_align_func_t.
 * @param[in] free_func - the new memory free_func
 * @return the old memory free_func
 */
ctk_task_free_align_func_t ctk_task_set_free_align_func(ctk_task_free_align_func_t free_func);

/**
 * Returns the current aligned-memory free_func for task objects.
 * @return the current memory free_func
 */
ctk_task_free_align_func_t ctk_task_get_free_align_func();

#ifdef __cplusplus
}
#endif

#endif /* _CTK_TASK_ALLOC_H */
