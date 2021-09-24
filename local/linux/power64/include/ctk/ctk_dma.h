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

#ifndef _CTK_DMA_H
#define _CTK_DMA_H

/**
 * @file ctk_dma.h
 * @brief Definitions for PPU-initiated DMA transfer helpers.
 *
 * @note The helpers declared in this file do not provide any alignment
 * support; the caller should take care of alignment both for the 
 * transfer size and addresses. 
 *
 * @note Currently for libspe and libspe2 it requires privileged access
 *   (i.e., root access)
 * @note Transfer direction might be confusing; note that GET means
 *   "GET from SPU" and PUT means "PUT from PPU".
 */

#include <stdio.h>
#include <errno.h>
#include "ctk_common.h"
#include "ctk_misc.h"
#include "ctk_trace.h"
#include "ctk_dma_common.h"
#include "ctk_spe.h"
#include "ctk_spe_base.h"

/* MFC transfer ID (TODO: correct me) */
#define CTK_MFC_DEFAULT_TRANSFER_ID 0x1

/* MFC replacement ID (TODO: correct me) */
#define CTK_MFC_DEFAULT_REPLACEMENT_ID 0x0

#ifdef __cplusplus
extern "C" {
#endif

static int s_tag_assigner = 0;
/**
  * ctk_dma_new_tag: get a new DMA tag 
  * @return a new tag group ID
  */
#define ctk_dma_new_tag() \
    (s_tag_assigner = (s_tag_assigner & 0xf), s_tag_assigner++)

#ifndef INLINE_CTK_DMA_WAIT_ALL
/**
 * ctk_dma_wait_all: blocks until <b>ALL</b> of the DMA transfers specified b
 *   the given tag_mask have done.  Tag masks can be constructed by calling
 *   macros #CTK_DMA_TAG_SET and #CTK_DMA_TAG_CLR.
 * @see #ctk_dma_wait_any, #ctk_dma_read_tag_status
 * @param[in] spe - target or source SPE
 * @param[in] tag_mask - DMA tag(s) mask to be waited.
 * @return CTK_SUCCESS (0) on success, errno (non zero) on failure
 */
int
ctk_dma_wait_all(ctk_spe_t spe, int tag_mask);
#endif /* INLINE_CTK_DMA_WAIT_ALL */

#ifndef INLINE_CTK_DMA_WAIT_ANY
/**
 * ctk_dma_wait_any: blocks until <b>ANY</b> of the DMA transfers specified by 
 *   the given tag_mask have done.
 * @see #ctk_dma_wait_all, #ctk_dma_read_tag_status
 * @param[in] spe - target or source SPE
 * @param[in] tag_mask - DMA tag(s) mask to be waited.
 * @return CTK_SUCCESS (0) on success, errno (non zero) on failure
 */
int
ctk_dma_wait_any(ctk_spe_t spe, int tag_mask);
#endif /* INLINE_CTK_DMA_WAIT_ANY */

#ifndef INLINE_CTK_DMA_READ_TAG_STATUS
/**
 * ctk_dma_read_tag_status: Checks the status of DMA transfers specified 
 * by the given #tag_mask.
 * @see #ctk_dma_wait_all, #ctk_dma_wait_any
 * @param[in] spe - target or source SPE
 * @param[in] tag_mask - DMA tag(s) mask to be checked.
 * @return non-zero (>0) if <b>ANY</b> of requested DMA transfers specified by
 *   #tag_mask completes, 0 otherwise.
 *   Returns -errno (<0) value on error.
 */
int
ctk_dma_read_tag_status(ctk_spe_t spe, int tag_mask);
#endif /* INLINE_CTK_DMA_READ_TAG_STATUS */

#ifndef INLINE_CTK_DMA_WAIT_ALL
/**
 * ctk_dma_wait_all: blocks until all of the DMA transfers specified by the
 *   given tag_mask have done.  Tag masks can be constructed by calling
 *   macros #CTK_DMA_TAG_SET and #CTK_DMA_TAG_CLR.
 * @param[in] spe - target or source SPE
 * @param[in] tag_mask - DMA tag(s) mask to be waited.
 * @return CTK_SUCCESS (0) on success, errno (non zero) on failure
 */
int
ctk_dma_wait_all(ctk_spe_t spe, int tag_mask);
#endif /* INLINE_CTK_DMA_WAIT_ALL */

/**
 * ctk_dma_wait: blocks until the DMA transfer specified by the
 *   given tag has done.
 * @param[in] spe - target or source SPE
 * @param[in] tag - DMA tag to be waited.
 * @return CTK_SUCCESS (0) on success, errno (non zero) on failure
 */
static inline int ctk_dma_wait(ctk_spe_t spe, int tag) __attribute__((always_inline)); 
static inline int
ctk_dma_wait(ctk_spe_t spe, int tag)
{
    return ctk_dma_wait_all(spe, 1 << tag);
}

/*-------------------------------------------------------------------*/
#ifndef INLINE_CTK_DMA_GET
/**
 * ctk_dma_get: DMA get operation (from EA to LS)
 * @param[in] spe - target or source SPE
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @param[in] tag - DMA tag to associate the initiating transfer
 * @return CTK_SUCCESS (0) on success, errno (non-zero) otherwise
 */
int
ctk_dma_get(ctk_spe_t spe, ls_addr_t ls_addr, void *ea_addr, dma_size_t size, int tag);
#endif /* INLINE_CTK_DMA_GET */

#ifndef INLINE_CTK_DMA_GETF
/**
 * ctk_dma_getf: DMA get operation (from EA to LS) with fence
 * @param[in] spe - target or source SPE
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @param[in] tag - DMA tag to associate the initiating transfer
 * @return CTK_SUCCESS (0) on success, errno (non-zero) otherwise
 */
int
ctk_dma_getf(ctk_spe_t spe, ls_addr_t ls_addr, void *ea_addr, dma_size_t size, int tag);
#endif /* INLINE_CTK_DMA_GETF */

#ifndef INLINE_CTK_DMA_GETB
/**
 * ctk_dma_getb: DMA get operation (from EA to LS) with barrier
 * @param[in] spe - target or source SPE
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @param[in] tag - DMA tag to associate the initiating transfer
 * @return CTK_SUCCESS (0) on success, errno (non-zero) otherwise
 */
int
ctk_dma_getb(ctk_spe_t spe, ls_addr_t ls_addr, void *ea_addr, dma_size_t size, int tag);
#endif /* INLINE_CTK_DMA_GETB */

/**
 * ctk_dma_get_block: blocking DMA get operation (from EA to LS)
 * @param[in] spe - target or source SPE
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @return CTK_SUCCESS (0) on success, errno (non-zero) otherwise
 */
static inline int
ctk_dma_get_block(ctk_spe_t spe, ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    int ret = ctk_dma_get(spe, ls_addr, ea_addr, size, tag);
    if (ret == CTK_ERROR_SPE_NOT_RUNNING &&
        ctk_spe_get_status(spe) == CTK_SPE_STATUS_READY)
    {
        while (ctk_spe_get_status(spe) == CTK_SPE_STATUS_READY);
        ret = ctk_dma_get(spe, ls_addr, ea_addr, size, tag);
    }
    if (ret != 0) {
        trace_debug("ctk_dma_get failed (errno:%d)\n", errno);
        return ret;
    }
    return ctk_dma_wait(spe, tag);
}

/**
 * ctk_dma_getf_block: blocking DMA get operation (from EA to LS) with fence
 * @param[in] spe - target or source SPE
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @return CTK_SUCCESS (0) on success, errno (non-zero) otherwise
 */
static inline int
ctk_dma_getf_block(ctk_spe_t spe, ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    int ret = ctk_dma_getf(spe, ls_addr, ea_addr, size, tag);
    if (ret == CTK_ERROR_SPE_NOT_RUNNING &&
        ctk_spe_get_status(spe) == CTK_SPE_STATUS_READY)
    {
        while (ctk_spe_get_status(spe) == CTK_SPE_STATUS_READY);
        ret = ctk_dma_getf(spe, ls_addr, ea_addr, size, tag);
    }
    if (ret != 0) {
        trace_debug("ctk_dma_getf failed (errno:%d)\n", errno);
        return ret;
    }
    return ctk_dma_wait(spe, tag);
}

/**
 * ctk_dma_getb_block: blocking DMA get operation (from EA to LS) with barrier
 * @param[in] spe - target or source SPE
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
 */
static inline int
ctk_dma_getb_block(ctk_spe_t spe, ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    int ret = ctk_dma_getb(spe, ls_addr, ea_addr, size, tag);
    if (ret == CTK_ERROR_SPE_NOT_RUNNING &&
        ctk_spe_get_status(spe) == CTK_SPE_STATUS_READY)
    {
        while (ctk_spe_get_status(spe) == CTK_SPE_STATUS_READY);
        ret = ctk_dma_getb(spe, ls_addr, ea_addr, size, tag);
    }
    if (ret != 0) {
        trace_debug("ctk_dma_getb failed (errno:%d)\n", errno);
        return ret;
    }
    return ctk_dma_wait(spe, tag);
}

/*-------------------------------------------------------------------*/
#ifndef INLINE_CTK_DMA_PUT
/**
 * ctk_dma_put: DMA put operation (from LS to EA)
 * @param[in] spe - target or source SPE
 * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @param[in] tag - DMA tag to associate the initiating transfer
 * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
 */
int
ctk_dma_put(ctk_spe_t spe, ls_addr_t ls_addr, void *ea_addr, dma_size_t size, int tag);
#endif /* INLINE_CTK_DMA_PUT */

#ifndef INLINE_CTK_DMA_PUTF
/**
 * ctk_dma_putf: DMA put operation (from LS to EA) with fence
 * @param[in] spe - target or source SPE
 * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @param[in] tag - DMA tag to associate the initiating transfer
 * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
 */
int
ctk_dma_putf(ctk_spe_t spe, ls_addr_t ls_addr, void *ea_addr, dma_size_t size, int tag);
#endif /* INLINE_CTK_DMA_PUTF */

#ifndef INLINE_CTK_DMA_PUTB
/**
 * ctk_dma_putb: DMA put operation (from LS to EA) with barrier
 * @param[in] spe - target or source SPE
 * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @param[in] tag - DMA tag to associate the initiating transfer
 * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
 */
int
ctk_dma_putb(ctk_spe_t spe, ls_addr_t ls_addr, void *ea_addr, dma_size_t size, int tag);
#endif /* INLINE_CTK_DMA_PUTB */

/**
 * ctk_dma_put_block: blocking DMA put operation (from EA to LS).
 *     This routine won't return until the DMA transfer finishes.
 * @param[in] spe - target or source SPE
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
 */
static inline int
ctk_dma_put_block(ctk_spe_t spe, ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    int ret = ctk_dma_put(spe, ls_addr, ea_addr, size, tag);
    if (ret == CTK_ERROR_SPE_NOT_RUNNING &&
        ctk_spe_get_status(spe) == CTK_SPE_STATUS_READY)
    {
        while (ctk_spe_get_status(spe) == CTK_SPE_STATUS_READY);
        ret = ctk_dma_put(spe, ls_addr, ea_addr, size, tag);
    }
    if (ret != 0) {
        trace_debug("ctk_dma_put failed (errno:%d)\n", errno);
        return ret;
    }
    return ctk_dma_wait(spe, tag);
}

/**
 * ctk_dma_putf_block: blocking DMA put operation (from LS to EA) with fence
 * @param[in] spe - target or source SPE
 * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
 */
static inline int
ctk_dma_putf_block(ctk_spe_t spe, ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    int ret = ctk_dma_putf(spe, ls_addr, ea_addr, size, tag);
    if (ret == CTK_ERROR_SPE_NOT_RUNNING &&
        ctk_spe_get_status(spe) == CTK_SPE_STATUS_READY)
    {
        while (ctk_spe_get_status(spe) == CTK_SPE_STATUS_READY);
        ret = ctk_dma_putf(spe, ls_addr, ea_addr, size, tag);
    }
    if (ret != 0) {
        trace_debug("ctk_dma_putf failed (errno:%d)\n", errno);
        return ret;
    }
    return ctk_dma_wait(spe, tag);
}

/**
 * ctk_dma_putb_block: blocking DMA put operation (from LS to EA) with barrier
 * @param[in] spe - target or source SPE
 * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @return CTK_SUCCESS (0) on success, errno (non-zero) on failure
 */
static inline int
ctk_dma_putb_block(ctk_spe_t spe, ls_addr_t ls_addr, void *ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    int ret = ctk_dma_putb(spe, ls_addr, ea_addr, size, tag);
    if (ret == CTK_ERROR_SPE_NOT_RUNNING &&
        ctk_spe_get_status(spe) == CTK_SPE_STATUS_READY)
    {
        while (ctk_spe_get_status(spe) == CTK_SPE_STATUS_READY);
        ret = ctk_dma_putb(spe, ls_addr, ea_addr, size, tag);
    }
    if (ret != 0) {
        trace_debug("ctk_dma_putb failed (errno:%d)\n", errno);
        return ret;
    }
    return ctk_dma_wait(spe, tag);
}

#ifdef __cplusplus
}
#endif

#endif /* CTK_DMA_H */
