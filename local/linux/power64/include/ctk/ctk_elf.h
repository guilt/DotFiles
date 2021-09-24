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

#ifndef _CTK_ELF_H
#define _CTK_ELF_H

/**
 * @file ctk_elf.h
 * @brief Definitions for SPE Elf handling 
 */

#include <elf.h>
#include <sys/types.h>
#include <stdio.h>
#include "ctk_base_alloc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Libspe's SPE program handle */
typedef struct {
    unsigned int handle_size;
    void *elf_image;
    void *toe_shadow;
} ctk_elf_cesof_handle_t;

/** Internal Elf mapped image structure */
typedef struct {
    unsigned int magic;
    void *elf_image;
    size_t elf_map_size;
} ctk_elf_mapped_handle_t;

/** Elf image structure */
typedef struct {
    void *elf_image;
} ctk_elf_raw_handle_t;

/** Elf handle (for type mangling) */
typedef union {
    ctk_elf_raw_handle_t    raw;
    ctk_elf_mapped_handle_t mapped;
    ctk_elf_cesof_handle_t  cesof;
} ctk_elf_handle_t;

#define CTK_ELF(handle)         ((ctk_elf_handle_t*)(handle))
#define CTK_ELF_RAW(handle)     (((ctk_elf_handle_t*)(handle))->raw)
#define CTK_ELF_MAPPED(handle)  (((ctk_elf_handle_t*)(handle))->mapped)
#define CTK_ELF_CESOF(handle)   (((ctk_elf_handle_t*)(handle))->cesof)

#define CTK_ELF_MAPPED_MAGIC        0x0e1fcafe
#define CTK_ELF_CESOF_HANDLE_SIZE   (sizeof(ctk_elf_cesof_handle_t))

#ifndef IS_ELF
#define IS_ELF(ehdr)    ((ehdr).e_ident[EI_MAG0] == ELFMAG0 && \
                         (ehdr).e_ident[EI_MAG1] == ELFMAG1 && \
                         (ehdr).e_ident[EI_MAG2] == ELFMAG2 && \
                         (ehdr).e_ident[EI_MAG3] == ELFMAG3)
#endif /* IS_ELF */

#define IS_CTK_ELF_RAW(handle)   \
    (IS_ELF(*(Elf32_Ehdr*)(CTK_ELF_RAW(handle).elf_image)))

#define IS_CTK_ELF_MAPPED(handle)   \
    (CTK_ELF_MAPPED(handle).magic == CTK_ELF_MAPPED_MAGIC)

#define IS_CTK_ELF_CESOF(handle)    \
    (CTK_ELF_CESOF(handle).handle_size == CTK_ELF_CESOF_HANDLE_SIZE && \
     IS_ELF(*(Elf32_Ehdr*)(CTK_ELF_CESOF(handle).elf_image)))

/** Elf image types */
typedef enum {
    CTK_ELF_TYPE_CESOF = 1,     /**< CESOF Elf program handle */
    CTK_ELF_TYPE_MAPPED = 2,    /**< Mapped Elf handle */
    CTK_ELF_TYPE_RAW = 3,       /**< Raw Elf image */
    CTK_ELF_TYPE_INVALID = -1,  /**< Invalid Elf type */
} ctk_elf_type_t;

/** Elf load image type */
/**
 * Quick-checks the elf image type
 * @param[in] elf_handle - Elf image handle
 * @return one of either #CTK_ELF_TYPE_CESOF, #CTK_ELF_TYPE_RAW, or
 *    #CTK_ELF_TYPE_MAPPED or #CTK_ELF_TYPE_INVALID.
 */
static inline int ctk_elf_check_type(ctk_elf_handle_t *elf_handle) {
    if (IS_CTK_ELF_MAPPED(elf_handle))
        return CTK_ELF_TYPE_MAPPED;
    if (IS_CTK_ELF_CESOF(elf_handle))
        return CTK_ELF_TYPE_CESOF;
    if (IS_CTK_ELF_RAW(elf_handle))
        return CTK_ELF_TYPE_RAW;
    return CTK_ELF_TYPE_INVALID;
}

/**
 * Returns pointer to the actual Elf raw image
 * @param[in] elf_handle - Elf image handle
 * @return pointer to the Elf image.  NULL if the given image is invalid.
 */
static inline void *ctk_elf_get_raw_image(void *elf_handle) {
    if (IS_CTK_ELF_MAPPED(elf_handle))
        return CTK_ELF_MAPPED(elf_handle).elf_image;
    if (IS_CTK_ELF_CESOF(elf_handle))
        return CTK_ELF_CESOF(elf_handle).elf_image;
    if (IS_CTK_ELF_RAW(elf_handle))
        return CTK_ELF_RAW(elf_handle).elf_image;
    return NULL;
}

/** Initialize CESOF handle
 * @param[out] handle - pointer to the CESOF program handle
 * @param[in] elf_hande - the Elf handle
 * @return CTK_SUCCESS (0) on success, errno (non-zero) otherwise.
 */
static inline int
ctk_elf_initialize_cesof(ctk_elf_handle_t *handle, void *elf_handle)
{
    handle->cesof.handle_size = CTK_ELF_CESOF_HANDLE_SIZE;
    handle->cesof.elf_image = ctk_elf_get_raw_image(elf_handle);
    handle->cesof.toe_shadow = NULL;
    if (handle->cesof.elf_image == NULL)
        return CTK_ERROR_BAD_OBJECT;
    return CTK_SUCCESS;
}

/**
 * Verifies the SPE Elf image.
 * @param[in] elf_image - pointer to the raw Elf image
 * @return 0 (CTK_SUCCESS) if the given ELF is valid, errno (non-zero)
 *    otherwise.
 */
int ctk_elf_verify_spe_image(void *elf_image);

/**
 * Opens Elf file and returns the loaded Elf image.
 * @param[out] elf_hande - Elf image handle
 * @param[in] filename - file name
 * @return 0 (CTK_SUCCESS) on success, errno (non-zero) on failure.
 */
int ctk_elf_open_handle(ctk_elf_handle_t **elf_handle, const char *filename);

/**
 * Closes Elf program and frees the allocated resources.
 * @param[in] elf_handle - Elf image handle
 * @return 0 (CTK_SUCCESS) on success, errno (non-zero) on failure.
 */
int ctk_elf_close_handle(ctk_elf_handle_t *elf_handle);

/**
 * Allocates buffer and loads the given ELF image into the buffer
 * @param[in] elf_handle - ELF handle (CESOF program handle, Raw Elf image
 *                         or Elf handle)
 * @param[in,out] load_image - load image (allocated by the callee if the 
 *                         given pointer is NULL. Be careful not to pass an
 *                         invalid non-null pointer value)
 * @param[out] load_size - load image size
 * @param[out] load_addr - load start addr
 * @param[out] entry_point - program entry point
 * @return 0 (CTK_SUCCESS) on success, errno (non-zero) on failure.
 */
int ctk_elf_load_image(ctk_elf_handle_t *elf_handle, void **load_image, ctk_uint32_t *load_size, ctk_uint32_t *load_addr, ctk_uint32_t *entry_point);

static inline int ctk_elf_free_load_image(void *load_image)
{
    if (load_image == NULL)
        return CTK_ERROR_NULL_OBJECT;
    ctk_base_free_align(load_image);
    return CTK_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif /* _CTK_ELF_H */
