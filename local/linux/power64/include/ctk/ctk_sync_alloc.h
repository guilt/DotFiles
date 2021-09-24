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

#ifndef _CTK_SYNC_ALLOC_H
#define _CTK_SYNC_ALLOC_H

/**
 * @file ctk_sync_alloc.h
 * @brief Definitions of memory allocator for sync objects.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Sync object types */
enum {
    CTK_SYNC_OBJECT = 10, /**< Sync object type - Any type */
    CTK_SYNC_ATOMIC,      /**< Sync object type - atomic */
    CTK_SYNC_MUTEX,       /**< Sync object type - mutex */
    CTK_SYNC_COND,        /**< Sync object type - cond */
    CTK_SYNC_SEM,         /**< Sync object type - semaphore */
    CTK_SYNC_QUEUE,       /**< Sync object type - queue */
    CTK_SYNC_GENERICQ,    /**< Sync object type - generic queue */
    CTK_SYNC_BARRIER,     /**< Sync object type - barrier */
    CTK_SYNC_LIST,        /**< Sync object type - list */
    CTK_SYNC_NUM_TYPES,   /* dummy */
};
/** The maximum number of the sync type ID */
#define CTK_SYNC_MAX    (CTK_SYNC_NUM_TYPES - 1)

/** Sync module memory allocator function type
    (takes sync object type and memory size to allocate,
      and returns the allocated pointer) */
typedef void *(*ctk_sync_alloc_func_t)(int obj_type, size_t size);

/** Sync module memory free function type
    (takes memory pointer and frees it) */
typedef void (*ctk_sync_free_func_t)(void *ptr);

/** Aligned memory allocator function type for sync modules
    (Takes sync object type, memory size to allocate and
    align size. Returns the allocated pointer) */
typedef void *(*ctk_sync_alloc_align_func_t)(int obj_type, size_t size,
    int align);

/** Aligned memory free function type for sync modules
    (Takes memory pointer and frees it) */
typedef void (*ctk_sync_free_align_func_t)(void *ptr);

#ifndef UNIT_TEST
/** A memory allocator for sync objects; you can change the default
    memory allocator by #ctk_sync_set_alloc_func.  */
extern ctk_sync_alloc_func_t ctk_sync_alloc;
/** A memory free function for sync objects; you can change the default
    memory free routine by #ctk_sync_set_free_func. */
extern ctk_sync_free_func_t ctk_sync_free;
/** An aligned memory allocator for sync objects; you can change the default
    memory allocator by #ctk_sync_set_alloc_align_func.  */
extern ctk_sync_alloc_align_func_t ctk_sync_alloc_align;
/** An aligned memory free function for sync objects; you can change the
    default memory free routine by #ctk_sync_set_free_align_func. */
extern ctk_sync_free_align_func_t ctk_sync_free_align;
#else /* UNIT_TEST */
static void *ctk_sync_alloc_default_static(int obj_type, size_t size) {
    return malloc(size);
}
static void ctk_sync_free_default_static(void *ptr) {
    free(ptr);
}
static void *ctk_sync_alloc_align_default_static(int obj_type, size_t size, int align) {
    return ctk_malloc_align(size, align);
}
static void ctk_sync_free_align_default_static(void *ptr) {
    ctk_free_align(ptr);
}

static ctk_sync_alloc_func_t ctk_sync_alloc = ctk_sync_alloc_default_static;
static ctk_sync_free_func_t ctk_sync_free = ctk_sync_free_default_static;
static ctk_sync_alloc_align_func_t ctk_sync_alloc_align = ctk_sync_alloc_align_default_static;
static ctk_sync_free_align_func_t ctk_sync_free_align = ctk_sync_free_align_default_static;
#endif /* UNIT_TEST */

/**
 * Sets a new memory allocator for sync objects (e.g., mutex, semaphore).
 * The allocator routine should be of ctk_sync_alloc_func_t, i.e., takes
 * two params sync_type and size and returns a pointer to the allocated region.
 * @param[in] allocator - the new memory allocator
 * @return the old memory allocator
 */
ctk_sync_alloc_func_t ctk_set_sync_alloc_func(ctk_sync_alloc_func_t allocator);

/**
 * Returns the current memory allocator for sync objects
 * (e.g., mutex, semaphore).
 * @return the current memory allocator
 */
ctk_sync_alloc_func_t ctk_get_sync_alloc_func();

/**
 * Sets a new memory free_func for sync objects (e.g., mutex, semaphore).
 * The free_func routine should be of ctk_sync_free_func_t, i.e., takes
 * two params sync_type and size and returns a pointer to the freeated region.
 * @param[in] free_func - the new memory free_func
 * @return the old memory free_func
 */
ctk_sync_free_func_t ctk_set_sync_free_func(ctk_sync_free_func_t free_func);

/**
 * Returns the current memory free_func for sync objects
 * (e.g., mutex, semaphore).
 * @return the current memory free_func
 */
ctk_sync_free_func_t ctk_get_sync_free_func();

/**
 * Sets a new aligned-memory allocator for sync objects.
 * The allocator routine should be of #ctk_sync_alloc_align_func_t.
 * @param[in] allocator - the new memory allocator
 * @return the old memory allocator
 */
ctk_sync_alloc_align_func_t ctk_sync_set_alloc_align_func(ctk_sync_alloc_align_func_t allocator);

/**
 * Returns the current aligned-memory allocator for sync objects.
 * @return the current memory allocator
 */
ctk_sync_alloc_align_func_t ctk_sync_get_alloc_align_func();

/**
 * Sets a new aligned memory free_func for sync objects.
 * The free_func routine should be of ctk_sync_free_align_func_t.
 * @param[in] free_func - the new memory free_func
 * @return the old memory free_func
 */
ctk_sync_free_align_func_t ctk_sync_set_free_align_func(ctk_sync_free_align_func_t free_func);

/**
 * Returns the current aligned-memory free_func for sync objects.
 * @return the current memory free_func
 */
ctk_sync_free_align_func_t ctk_sync_get_free_align_func();
#ifdef __cplusplus
}
#endif

#endif /* _CTK_SYNC_ALLOC_H */
