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

#ifndef _CTK_SPE_ANY_H
#define _CTK_SPE_ANY_H

/**
 * @file ctk_spe_base.h
 * @brief Definitions for SPE any-type operations
 */
#include "ctk_common.h"
#include "ctk_spe.h"
#include "ctk_spe_thread.h"

/** CTK SPE object any data type */
typedef struct _ctk_spe_base {
    unsigned int m_type;
} ctk_spe_base_body_t;

/** CTK SPE object any pointer type (SPE context, thread or thread group) */
typedef void *ctk_spe_t;

#define CTK_SPE_TYPE(obj)       (((ctk_spe_base_body_t*)obj)->m_type)

/** A macro to cast SPE thread from SPE any object */
#define CTK_SPE_THREAD(spe)   ((ctk_spe_thread_t)spe)

/** A macro to cast SPE thread group from SPE any object */
#define CTK_SPE_THREAD_GROUP(spe)   ((ctk_spe_thread_group_t)spe)

/** A macro to get SPE context from SPE any object */
#define CTK_SPE_CONTEXT(spe)  \
        (CTK_SPE_TYPE(spe) == CTK_SPE_CONTEXT_T) \
            ? ((ctk_spe_context_t)spe) \
            : ctk_spe_thread_get_context(CTK_SPE_THREAD(spe))

/**
 * Returns SPE status of the given SPE object.
 * @param[in] spe - the SPE any object (thread or context)
 * @return either one of #CTK_SPE_STATUS_CREATED, #CTK_SPE_STATUS_LOADED,
 *   #CTK_SPE_STATUS_READY, #CTK_SPE_STATUS_RUNNING, #CTK_SPE_STATUS_STOPPED or
 *   #CTK_SPE_STATUS_UNKNOWN.
 */
static inline int ctk_spe_get_status(ctk_spe_t spe) __attribute__((always_inline)); 
static inline int ctk_spe_get_status(ctk_spe_t spe)
{
    if (spe == NULL)
        return CTK_SPE_STATUS_UNKNOWN;
    return ctk_spe_context_get_status(CTK_SPE_CONTEXT(spe));
}

/** Macro to obtain internal(native) SPE structure for the given SPE object.
 * @param[in] spe - SPE 
 * @return native SPE
 * @note vendor-library-dependent modules must provide a macro _CTK_SPE_NATIVE
 *   to support this.
 */
#define CTK_SPE_NATIVE(spe)      _CTK_SPE_NATIVE(CTK_SPE_CONTEXT(spe))


/*--------------------------------------------------------------------*/
/* SPE context & thread common functions */

/**
 * Maps the LS (local storage) to the EA addr and returns the mapped addr.
 * @param[in] spe - the SPE context or thread whose LS is mapped
 * @param[out] map_addr - the mapped address
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
static inline int
ctk_spe_map_ls(ctk_spe_t spe, ea_addr_t *map_addr) {
    return ctk_spe_context_map_ls(CTK_SPE_CONTEXT(spe), map_addr);
}

/**
 * Maps the signal notification register 1 for the SPE specified
 * by #spe and returns the mapped addr.
 * @note The address is not DMA aligned (mod 16 = 12).
 * @param[in] spe - the SPE (context or thread) whose signal 
 *      notification register is mapped
 * @param[out] map_addr - the mapped address
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
static inline int
ctk_spe_map_signal1(ctk_spe_context_t spe, ea_addr_t *map_addr) {
    return ctk_spe_context_map_signal1(CTK_SPE_CONTEXT(spe), map_addr);
}

/**
 * Maps the signal notification register 2 for the SPE specified by
 * #spe and returns the mapped addr.
 * @note The address is not DMA aligned (mod 16 = 12).
 * @param[in] spe - the SPE (context or thread) whose signal 
 *      notification register is mapped
 * @param[out] map_addr - the mapped address 
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
static inline int
ctk_spe_map_signal2(ctk_spe_context_t spe, ea_addr_t *map_addr) {
    return ctk_spe_context_map_signal2(CTK_SPE_CONTEXT(spe), map_addr);
}

/**
 * Adds an memory access hint (significant only for tSDK env)
 * @note The added access hints will be automatically removed when 
 *     the target SPE context/thread is destroyed
 * @param[in] spe - the SPE context or thread
 * @param[in] addr - start addr to add the memory access hint
 * @param[in] size - size of the region to add the memory access hint
 */
static inline int ctk_spe_add_access_hint(ctk_spe_t spe, void *addr, ea_size_t size)
{
    return ctk_spe_context_add_access_hint(CTK_SPE_CONTEXT(spe), addr, size);
}

/**
 * Removes the memory access hint (only necessary if you want to
 *  remove the access hint before destroying the SPE context/thread)
 * @param[in] spe - the SPE context or thread
 * @param[in] hint_id - the access hint ID to be removed
 */
static inline int ctk_spe_remove_access_hint(ctk_spe_t spe, void *addr)
{
    return ctk_spe_context_remove_access_hint(CTK_SPE_CONTEXT(spe), addr);
}


#if !defined(DOXYGEN) && !defined(CELL_ENV_TSDK)
/* internal */
static inline int ctk_spe_map_ps(ctk_spe_t spe, void **ps, enum ps_area area) {
    return ctk_spe_context_map_ps(CTK_SPE_CONTEXT(spe), ps, area);
}
#endif

#endif /* _CTK_SPE_ANY_H */
