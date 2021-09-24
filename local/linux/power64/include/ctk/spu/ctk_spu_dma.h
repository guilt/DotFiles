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

#ifndef _CTK_SPU_DMA_H
#define _CTK_SPU_DMA_H

/**
 * @file ctk_spu_dma.h
 * @brief Definitions for SPU-initiated DMA transfer helpers
 *
 * @note the CTK's DMA transfer routines accept unaligned addr and sizes,
 *   but you should specify aligned ones to get reasonal perfornace.
 */
#ifdef __SPU__

#include "ctk_spu_common.h"

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

/**
 * ctk_dma_wait: DMA sync operation
 * @param[in] tag - DMA tag to be waited
 * @return void
 */
static inline void ctk_dma_wait(int tag) __attribute__((always_inline));
static inline void
ctk_dma_wait(int tag) {
    // wait finishing the transfer
    DEBUG_PRINTF("WAITING: tag:%d\n", tag);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);
}

/**
 * ctk_dma_wait_all: blocks until ALL of the DMA transfers specified by the
 *   given tag_mask have done.  Tag masks can be constructed by calling
 *   macros #CTK_DMA_TAG_SET and #CTK_DMA_TAG_CLR.
 * @param[in] tag_mask - DMA tag mask to be waited
 * @return void
 */
static inline void ctk_dma_wait_all(int tag_mask) __attribute__((always_inline));
static inline void
ctk_dma_wait_all(int tag_mask) {
    // wait finishing the transfer
    DEBUG_PRINTF("WAITING: tag mask:%d\n", tag_mask);
    spu_writech(MFC_WrTagMask, tag_mask);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);
}

/**
 * ctk_dma_wait_any: blocks until ANY of the DMA transfers specified by the
 *   given tag_mask have done.  Tag masks can be constructed by calling
 *   macros #CTK_DMA_TAG_SET and #CTK_DMA_TAG_CLR.
 * @param[in] tag_mask - DMA tag mask to be waited
 * @return void
 */
static inline void ctk_dma_wait_any(int tag_mask) __attribute__((always_inline));
static inline void
ctk_dma_wait_any(int tag_mask) {
    // wait finishing the transfer
    DEBUG_PRINTF("WAITING: tag mask:%d\n", tag_mask);
    spu_writech(MFC_WrTagMask, tag_mask);
    spu_mfcstat(MFC_TAG_UPDATE_ANY);
}

/**
 * ctk_dma_read_tag_status: Checks the status of DMA transfers specified 
 * by the given #tag_mask.
 * @see #ctk_dma_wait_all, #ctk_dma_wait_any
 * @param[in] tag_mask - DMA tag(s) mask to be checked.
 * @return non-zero (>0) if <b>ANY</b> of requested DMA transfers specified by
 *   #tag_mask completes, 0 otherwise.
 */
static inline int ctk_dma_read_tag_status(int tag_mask) __attribute__((always_inline));
static inline int
ctk_dma_read_tag_status(int tag_mask) {
    spu_writech(MFC_WrTagMask, tag_mask);
    return spu_mfcstat(MFC_TAG_UPDATE_IMMEDIATE);
}

#include "ctk_spu_dma_list.h"

/*-------------------------------------------------------------------*/
 /* internal routine - DMA transfer for unaligned region */
#ifdef CTK_SPU_RUNTIME
# define ctk_dma_transfer_unaligned(l,e,s,t,c)     /* dummy */
#else
void ctk_dma_transfer_unaligned(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag, int cmd);
#endif

/**
 * DMA transfer only for small size data(< #DMA_TRANSFER_SIZE).
 * @param[in] ls_addr - LS addr
 * @param[in] ea_addr - EA addr
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @param[in] cmd - DMA command to issue
 * @return void
 */
static inline void
ctk_dma_transfer_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag, int cmd)
{
    CTK_DEBUG_ASSERT(size < DMA_TRANSFER_SIZE);

    unsigned int ls_mod_15 = ptr2int(ls_addr) & 15;
    if (__builtin_expect(ls_mod_15 != (ea_addr & 15) || 
                         (ls_mod_15 & (size - 1)), 0))
     {
        /* unaligned transfer; could be slow */
        ctk_dma_transfer_unaligned(ls_addr, ea_addr, size, tag, cmd);
        return;
    }

    DEBUG_PRINTF("DMA[%x]: [LS]:%p, [EA]:%x %x, size:%u tag:%d\n", 
        cmd, ls_addr, ea_hi32(ea_addr), ea_lo32(ea_addr), size, tag);
    spu_mfcdma64((volatile void*)ls_addr,
            ea_hi32(ea_addr), ea_lo32(ea_addr), size, tag, cmd);
}

/**
 * DMA transfer (generic routine).
 * @param[in] ls_addr - LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size 
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @param[in] cmd - DMA command to issue
 * @return void
 */
static inline void
ctk_dma_transfer(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag, int cmd)
{
    unsigned int ls_mod_15 = ptr2int(ls_addr) & 15;
    if (__builtin_expect(ls_mod_15 != (ea_addr & 15) || 
                         (ls_mod_15 & (size - 1)), 0)) {
        /* unaligned transfer; could be slow */
        ctk_dma_transfer_unaligned(ls_addr, ea_addr, size, tag, cmd);
        return;
    }

    ctk_uint32_t bs = 0;
    ea_addr32_t hi32 = ea_hi32(ea_addr);
    ea_addr32_t lo32 = ea_lo32(ea_addr);
    for ( ; bs + DMA_TRANSFER_SIZE < size; bs += DMA_TRANSFER_SIZE) {
        DEBUG_PRINTF("DMA[%x]: [LS]:%p+%d, [EA]:%x %x, size:%u tag:%d\n",
            cmd, ls_addr, bs, hi32, lo32 + bs, DMA_TRANSFER_SIZE, tag);
        spu_mfcdma64((volatile void*)int2ptr(ptr2int(ls_addr) + bs),
            hi32, lo32 + bs, DMA_TRANSFER_SIZE, tag, cmd);
    } 
    DEBUG_PRINTF("DMA[%x]: [LS]:%p+%d, [EA]:%x %x, size:%u tag:%d\n",
        cmd, ls_addr, bs, hi32, lo32 + bs, size - bs, tag);
    spu_mfcdma64((volatile void*)int2ptr(ptr2int(ls_addr) + bs),
        hi32, lo32 + bs, size - bs, tag, cmd);
}

/*-------------------------------------------------------------------*/
/**
 * DMA get operation (from EA to LS)
 *     only for small size transfer (< #DMA_TRANSFER_SIZE).
 *     The transfer #size should be less than the DMA maximum
 *     transfer size (per one command).
 *     For smaller transfers this should be faster than the generic
 *     get routine #ctk_dma_get.
 * @param[in] ls_addr - destination LS addr
 * @param[in] ea_addr - source EA addr
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void ctk_dma_get_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag) __attribute__((always_inline));
static inline void
ctk_dma_get_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag)
{
    ctk_dma_transfer_small(ls_addr, ea_addr, size, tag, MFC_GET_CMD);
}

/**
 * DMA get operation (from EA to LS)
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void ctk_dma_get(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag) __attribute__((always_inline));
static inline void
ctk_dma_get(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag)
{
    ctk_dma_transfer(ls_addr, ea_addr, size, tag, MFC_GET_CMD);
}

/**
 * blocking DMA get operation (from EA to LS)
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @return void
 */
static inline void ctk_dma_get_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size) __attribute__((always_inline));
static inline void
ctk_dma_get_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_get(ls_addr, ea_addr, size, tag);
    ctk_dma_wait(tag);
}

/**
 * blocking DMA get operation (from LS to EA)
 *     only for small size transfer (< #DMA_TRANSFER_SIZE).
 *     This routine won't return until the DMA transfer finishes.
 *     The transfer #size should be less than the DMA maximum
 *     transfer size (per one command).
 * @param[in] ls_addr - source LS addr
 * @param[in] ea_addr - destination EA addr
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @return void
 */
static inline void ctk_dma_get_small_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size) __attribute__((always_inline));
static inline void
ctk_dma_get_small_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_get_small(ls_addr, ea_addr, size, tag);
    ctk_dma_wait(tag);
}

/**
 * DMA get operation <b>with fence</b> for small size data
 * @param[in] ls_addr - destination LS addr
 * @param[in] ea_addr - source EA addr
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void ctk_dma_getf_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag) __attribute__((always_inline));
static inline void
ctk_dma_getf_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag)
{
    ctk_dma_transfer_small(ls_addr, ea_addr, size, tag, MFC_GETF_CMD);
}

/**
 * DMA get operation <b>with fence</b> 
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void ctk_dma_getf(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag) __attribute__((always_inline));
static inline void
ctk_dma_getf(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag)
{
    ctk_dma_transfer(ls_addr, ea_addr, size, tag, MFC_GETF_CMD);
}

/**
 * blocking DMA get operation <b>with fence</b> 
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @return void
 */
static inline void ctk_dma_getf_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size) __attribute__((always_inline));
static inline void
ctk_dma_getf_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_getf(ls_addr, ea_addr, size, tag);
    ctk_dma_wait(tag);
}

/**
 * blocking DMA get operation <b>with fence</b> for small size data
 * @param[in] ls_addr - source LS addr
 * @param[in] ea_addr - destination EA addr 
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @return void
 */
static inline void ctk_dma_getf_small_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size) __attribute__((always_inline));
static inline void
ctk_dma_getf_small_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_getf_small(ls_addr, ea_addr, size, tag);
    ctk_dma_wait(tag);
}

/**
 * DMA get operation <b>with barrier</b> for small size data
 * @param[in] ls_addr - destination LS addr
 * @param[in] ea_addr - source EA addr
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void ctk_dma_getb_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag) __attribute__((always_inline));
static inline void
ctk_dma_getb_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag)
{
    ctk_dma_transfer_small(ls_addr, ea_addr, size, tag, MFC_GETB_CMD);
}

/**
 * DMA get operation <b>with barrier</b> 
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void ctk_dma_getb(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag) __attribute__((always_inline));
static inline void
ctk_dma_getb(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag)
{
    ctk_dma_transfer(ls_addr, ea_addr, size, tag, MFC_GETB_CMD);
}

/**
 * blocking DMA get operation <b>with barrier</b> 
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @return void
 */
static inline void ctk_dma_getb_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size) __attribute__((always_inline));
static inline void
ctk_dma_getb_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_getb(ls_addr, ea_addr, size, tag);
    ctk_dma_wait(tag);
}

/**
 * blocking DMA get operation <b>with barrier</b> for small size data
 * @param[in] ls_addr - source LS addr
 * @param[in] ea_addr - destination EA addr
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @return void
 */
static inline void ctk_dma_getb_small_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size) __attribute__((always_inline));
static inline void
ctk_dma_getb_small_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_getb_small(ls_addr, ea_addr, size, tag);
    ctk_dma_wait(tag);
}
/*-------------------------------------------------------------------*/
/**
 * DMA put operation (from LS to EA)
 *     only for small size transfer (< #DMA_TRANSFER_SIZE).
 *     The transfer #size should be less than the DMA maximum
 *     transfer size (per one command).
 *     For smaller transfers this should be faster than the generic
 *     put routine #ctk_dma_put.
 * @param[in] ls_addr - source LS addr
 * @param[in] ea_addr - destination EA addr
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void ctk_dma_put_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag) __attribute__((always_inline));
static inline void
ctk_dma_put_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag)
{
    ctk_dma_transfer_small(ls_addr, ea_addr, size, tag, MFC_PUT_CMD);
}

/**
 * DMA put operation (from LS to EA)
 * @param[in] ls_addr - source LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - destination EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void ctk_dma_put(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag) __attribute__((always_inline));
static inline void
ctk_dma_put(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag)
{
    ctk_dma_transfer(ls_addr, ea_addr, size, tag, MFC_PUT_CMD);
}

/**
 * blocking DMA put operation (from EA to LS).
 *     This routine won't return until the DMA transfer finishes.
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @return void
 */
static inline void ctk_dma_put_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size) __attribute__((always_inline));
static inline void
ctk_dma_put_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_put(ls_addr, ea_addr, size, tag);
    ctk_dma_wait(tag);
}

/**
 * blocking DMA put operation (from LS to EA)
 *     only for small size transfer (< #DMA_TRANSFER_SIZE).
 *     This routine won't return until the DMA transfer finishes.
 *     The transfer #size should be less than the DMA maximum
 *     transfer size (per one command).
 * @param[in] ls_addr - source LS addr
 * @param[in] ea_addr - destination EA addr
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @return void
 */
static inline void ctk_dma_put_small_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size) __attribute__((always_inline));
static inline void
ctk_dma_put_small_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_put_small(ls_addr, ea_addr, size, tag);
    ctk_dma_wait(tag);
}

/**
 * DMA put operation <b>with fence</b> for small size data
 * @param[in] ls_addr - destination LS addr
 * @param[in] ea_addr - source EA addr
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void ctk_dma_putf_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag) __attribute__((always_inline));
static inline void
ctk_dma_putf_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag)
{
    ctk_dma_transfer_small(ls_addr, ea_addr, size, tag, MFC_PUTF_CMD);
}

/**
 * DMA put operation <b>with fence</b> 
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void ctk_dma_putf(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag) __attribute__((always_inline));
static inline void
ctk_dma_putf(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag)
{
    ctk_dma_transfer(ls_addr, ea_addr, size, tag, MFC_PUTF_CMD);
}

/**
 * blocking DMA put operation <b>with fence</b> 
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @return void
 */
static inline void ctk_dma_putf_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size) __attribute__((always_inline));
static inline void
ctk_dma_putf_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_putf(ls_addr, ea_addr, size, tag);
    ctk_dma_wait(tag);
}

/**
 * blocking DMA put operation <b>with fence</b> for small size data
 * @param[in] ls_addr - source LS addr
 * @param[in] ea_addr - destination EA addr
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @return void
 */
static inline void ctk_dma_putf_small_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size) __attribute__((always_inline));
static inline void
ctk_dma_putf_small_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_putf_small(ls_addr, ea_addr, size, tag);
    ctk_dma_wait(tag);
}

/**
 * DMA put operation <b>with barrier</b> for small size data
 * @param[in] ls_addr - destination LS addr
 * @param[in] ea_addr - source EA addr
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void ctk_dma_putb_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag) __attribute__((always_inline));
static inline void
ctk_dma_putb_small(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag)
{
    ctk_dma_transfer_small(ls_addr, ea_addr, size, tag, MFC_PUTB_CMD);
}

/**
 * DMA put operation <b>with barrier</b> 
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void ctk_dma_putb(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag) __attribute__((always_inline));
static inline void
ctk_dma_putb(void *ls_addr, ea_addr_t ea_addr, dma_size_t size, int tag)
{
    ctk_dma_transfer(ls_addr, ea_addr, size, tag, MFC_PUTB_CMD);
}

/**
 * blocking DMA put operation <b>with barrier</b> 
 * @param[in] ls_addr - destination LS addr (must be 16-byte aligned)
 * @param[in] ea_addr - source EA addr (must be 16-byte aligned)
 * @param[in] size - transfer size (must be 16-byte aligned)
 * @return void
 */
static inline void ctk_dma_putb_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size) __attribute__((always_inline));
static inline void
ctk_dma_putb_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_putb(ls_addr, ea_addr, size, tag);
    ctk_dma_wait(tag);
}

/**
 * blocking DMA put operation <b>with barrier</b> for small size data
 * @param[in] ls_addr - source LS addr
 * @param[in] ea_addr - destination EA addr
 * @param[in] size - transfer size (must be < #DMA_TRANSFER_SIZE)
 * @return void
 */
static inline void ctk_dma_putb_small_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size) __attribute__((always_inline));
static inline void
ctk_dma_putb_small_block(void *ls_addr, ea_addr_t ea_addr, dma_size_t size)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_putb_small(ls_addr, ea_addr, size, tag);
    ctk_dma_wait(tag);
}
#ifdef __cplusplus
}
#endif

#endif /* __SPU__ */

#endif /* _CTK_SPU_DMA_H */
