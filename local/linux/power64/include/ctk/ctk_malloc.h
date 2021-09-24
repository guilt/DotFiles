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

#ifndef _CTK_MALLOC_H
#define _CTK_MALLOC_H

/**
 * @file ctk_malloc.h
 * @brief Definitions for CTK error numbers and error routines.
 */

#include <errno.h>
#include <stdlib.h>
#include "ctk_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocates `size' bytes of a memory buffer and returns a pointer to the
 * allocated memory in `*memptr'.  The memory address will be a multiple of
 * `alignment'.  The allocated memory must be freed by #ctk_free_align.
 * @param[out] memptr - Pointer to the pointer for the allocated buffer.
 * @param[in] alignment - alignment
 * @param[in] size - size
 * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure.
 */
static int ctk_memalign(void **memptr, size_t alignment, size_t size);

/**
 * Frees the given pointer.  The pointer had to be allocated by #ctk_memalign
 * or #ctk_malloc_align.
 * @param[in] ptr - Pointer to the allocated memory to be freed.
 */
static void ctk_free_align(void *ptr);

static inline int ctk_memalign(void **memptr, size_t alignment, size_t size)
{
    void *p = (void *)malloc(size + sizeof(void*) + alignment - 1);
#if !defined(__SPU__) || defined(DEBUG)
    if (! IS_POWEROF2(alignment))
        return CTK_ERROR_BAD_ALIGNMENT;
#endif /* !SPU || DEBUG */
    if (p) {
        *memptr = int2ptr(ALIGN_UP(ptr2int(p) + sizeof(void*), alignment));
        *((void **)(*memptr)-1) = (void *)(p);
    } else {
        return CTK_ERROR_NO_MEMORY;
    }
    return CTK_SUCCESS;
}

static inline int ctk_memalign_realloc(void **memptr, size_t alignment, size_t size)
{
    void * real = NULL;
    if (*memptr) {
        real = *((void **)(*memptr)-1);
    }
    void *p = (void *)realloc(real, size + sizeof(void*) + alignment - 1);
#if !defined(__SPU__) || defined(DEBUG)
    if (! IS_POWEROF2(alignment))
        return CTK_ERROR_BAD_ALIGNMENT;
#endif /* !SPU || DEBUG */
    if (p) {
        *memptr = int2ptr(ALIGN_UP(ptr2int(p) + sizeof(void*), alignment));
        *((void **)(*memptr)-1) = (void *)(p);
    } else {
        return CTK_ERROR_NO_MEMORY;
    }
    return CTK_SUCCESS;
}

static inline void ctk_free_align(void *ptr)
{
    void * real;

    if (ptr) {
        real = *((void **)(ptr)-1);
        free(real);
    }
}

#if 0 /* HAVE_POSIX_MEMALIGN */
static inline int ctk_memalign(void **memptr, size_t alignment, size_t size)
{
    int err = posix_memalign(memptr, alignment, size);
    if (err == EINVAL)
        return CTK_ERROR_BAD_ALIGNMENT;
    if (err == ENOMEM)
        return CTK_ERROR_NO_MEMORY;
    return err;
}

static inline void ctk_free_align(void *ptr)
{
    free(ptr);
}
#endif /* 0 */

#ifdef __cplusplus
}
#endif

/**
 * Allocates 'size' bytes of a memory buffer and returns a pointer to the
 * allocated memory.  The memory address and allocated buffer size will
 * be a multiple of 'alignment'.
 * @param[in] size - size
 * @param[in] alignment - alignment
 * @return a pointer to the buffer on success, NULL on failure.
 */
static inline void *ctk_malloc_align(size_t size, size_t alignment) __attribute__((always_inline));
static inline void *ctk_malloc_align(size_t size, size_t alignment)
{
    void *p = NULL;
    int ret = ctk_memalign(&p, alignment, size);
    if (ret == 0)
        return p;
    return NULL;
}

/**
 * Reallocates 'size' bytes of a memory buffer and returns a pointer to the
 * allocated memory.
 * @param[in] ptr - pointer to be realloced
 * @param[in] size - size
 * @param[in] alignment - alignment
 * @return a pointer to the buffer on success, NULL on failure.
 */
static inline void *ctk_realloc_align(void *p, size_t size, size_t alignment) __attribute__((always_inline));
static inline void *ctk_realloc_align(void *p, size_t size, size_t alignment)
{
    int ret = ctk_memalign_realloc(&p, alignment, size);
    if (ret == 0)
        return p;
    return NULL;
}

/**
 * Allocates an aligned buffer on the stack (i.e. alloca).
 * @param[out] _retptr_ - the returned pointer
 * @param[in] _size_ - allocation size
 * @param[in] _alignment_ - alignment size
 */
#define ctk_alloca_align(_retptr_, _size_, _alignment_)  \
    char CTK_UNIQ_NAME(_tmp_ ## _retptr_) [ALIGN_UP((_size_), (_alignment_)) + (_alignment_)]; \
    _retptr_ = (void*)ALIGN_UP_PTR(CTK_UNIQ_NAME(_tmp_ ## _retptr_), (_alignment_));
#if 0
static inline void *ctk_alloca_align(int _size_, int _alignment_) {
    void *ptr = alloca(ALIGN_UP((_size_), (_alignment_)) + (_alignment_)); 
    return (void*)ALIGN_UP_PTR(ptr, (_alignment_));
}
#endif

#define ctk_alloca_align_typed(_retptr_, _type_, _size_, _alignment_)  \
    char CTK_UNIQ_NAME(_tmp_ ## _retptr_) [ALIGN_UP((_size_), (_alignment_)) + (_alignment_)]; \
    _retptr_ = (_type_)ALIGN_UP_PTR(CTK_UNIQ_NAME(_tmp_ ## _retptr_), (_alignment_));

#endif /* _CTK_MALLOC_H */
