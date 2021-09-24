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

#ifndef _CTK_BASE_ALLOC_H
#define _CTK_BASE_ALLOC_H

/**
 * @file ctk_base_alloc.h
 * @brief Definitions of memory allocator for base objects.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Base object types */
enum {
    CTK_BASE_OBJECT = 0,    /**< Base object type - Any type */
    CTK_BASE_SPE,           /**< Base object type - SPE */
    CTK_BASE_ELF,           /**< Base object type - Elf */
    CTK_BASE_THREAD,        /**< Base object type - thread */
    CTK_BASE_THREAD_GROUP,  /**< Base object type - thread group */
    CTK_BASE_ENV,           /**< Base object type - Env */
    CTK_BASE_HASH,          /**< Base object type - hash */
    CTK_BASE_LIST,          /**< Base object type - list */
    CTK_BASE_NUM_TYPES      /* dummy */
};
/** The maximum number of the base type ID */
#define CTK_BASE_MAX    (CTK_BASE_NUM_TYPES - 1)

/** Memory allocator function type for base modules
    (Takes base object type and memory size to allocate,
      and returns the allocated pointer) */
typedef void *(*ctk_base_alloc_func_t)(int obj_type, size_t size);

/** Memory free function type for base modules
    (Takes memory pointer and frees it) */
typedef void (*ctk_base_free_func_t)(void *ptr);

/** Aligned memory allocator function type for base modules
    (Takes base object type, memory size to allocate and
    align size. Returns the allocated pointer) */
typedef void *(*ctk_base_alloc_align_func_t)(int obj_type, size_t size,
    int align);

/** Aligned memory free function type for base modules
    (Takes memory pointer and frees it) */
typedef void (*ctk_base_free_align_func_t)(void *ptr);

/** A memory allocator for base objects; you can change the default
    memory allocator by #ctk_base_set_alloc_func.  */
extern ctk_base_alloc_func_t ctk_base_alloc;
/** A memory free function for base objects; you can change the default
    memory free routine by #ctk_base_set_free_func. */
extern ctk_base_free_func_t ctk_base_free;

/** An aligned memory allocator for base objects; you can change the default
    memory allocator by #ctk_base_set_alloc_align_func.  */
extern ctk_base_alloc_align_func_t ctk_base_alloc_align;
/** An aligned memory free function for base objects; you can change the
    default memory free routine by #ctk_base_set_free_align_func. */
extern ctk_base_free_align_func_t ctk_base_free_align;

/**
 * Sets a new memory allocator for base objects (e.g., hash, thread, etc.).
 * The allocator routine should be of ctk_base_alloc_func_t, i.e., takes
 * two params base_type and size and returns a pointer to the allocated region.
 * @param[in] allocator - the new memory allocator
 * @return the old memory allocator
 */
ctk_base_alloc_func_t ctk_base_set_alloc_func(ctk_base_alloc_func_t allocator);

/**
 * Returns the current memory allocator for base objects
 * (e.g., mutex, semaphore).
 * @return the current memory allocator
 */
ctk_base_alloc_func_t ctk_base_get_alloc_func();

/**
 * Sets a new memory free_func for base objects.
 * The free_func routine should be of ctk_base_free_func_t, i.e., takes
 * two params base_type and size and returns a pointer to the freeated region.
 * @param[in] free_func - the new memory free_func
 * @return the old memory free_func
 */
ctk_base_free_func_t ctk_base_set_free_func(ctk_base_free_func_t free_func);

/**
 * Returns the current memory free_func for base objects.
 * @return the current memory free_func
 */
ctk_base_free_func_t ctk_base_get_free_func();

/**
 * Sets a new aligned-memory allocator for base objects.
 * The allocator routine should be of #ctk_base_alloc_align_func_t.
 * @param[in] allocator - the new memory allocator
 * @return the old memory allocator
 */
ctk_base_alloc_align_func_t ctk_base_set_alloc_align_func(ctk_base_alloc_align_func_t allocator);

/**
 * Returns the current aligned-memory allocator for base objects.
 * @return the current memory allocator
 */
ctk_base_alloc_align_func_t ctk_base_get_alloc_align_func();

/**
 * Sets a new aligned memory free_func for base objects.
 * The free_func routine should be of ctk_base_free_align_func_t.
 * @param[in] free_func - the new memory free_func
 * @return the old memory free_func
 */
ctk_base_free_align_func_t ctk_base_set_free_align_func(ctk_base_free_align_func_t free_func);

/**
 * Returns the current aligned-memory free_func for base objects.
 * @return the current memory free_func
 */
ctk_base_free_align_func_t ctk_base_get_free_align_func();

#ifdef __cplusplus
}
#endif

#endif /* _CTK_BASE_ALLOC_H */

