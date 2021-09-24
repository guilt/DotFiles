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

#ifndef _CTK_SPU_DMA_2D_H
#define _CTK_SPU_DMA_2D_H

/**
 * @file ctk_spu_dma_2d.h
 * @brief Definitions for SPU-initiated DMA transfer for 2D region
 */
#ifdef __SPU__

#include "ctk_spu_common.h"
#include "ctk_spu_dma_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * DMA transfer for 2D region
 * @param[in] ls_addr - LS addr
 * @param[in] ea_base - EA base addr
 * @param[in] region_size_x - the width (X size) of the region 
 * @param[in] transfer_start_x - X left-top corner point of the region to be transfered
 * @param[in] transfer_start_y - Y left-top corner point of the region to be transfered
 * @param[in] tansfer_size_x - size in X to be transfered
 * @param[in] tansfer_size_y - size in Y to be transfered
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @param[in] cmd - DMA command to issue
 * @return void
 */
static inline void
ctk_dma_2d_transfer(void *ls_addr, ea_addr_t ea_base, int region_size_x, int transfer_start_x, int transfer_start_y, int transfer_size_x, int transfer_size_y, int tag, int cmd)
{
    int i;
#if 0
    ctk_dma_list_va_t list;
    ctk_dma_list_va_init(&list, ls_addr, ea_base);
    ctk_dma_list_va_alloc_elems(&list, transfer_size_y);
    for (i = 0; i < transfer_size_y; i++) {
        ea_addr_t addr = ea_base + (transfer_start_y + i) * region_size_x 
                                      + transfer_start_x;
        ctk_dma_list_va_add_elem(&list, addr, transfer_size_x);
    }
    ctk_dma_list_va_transfer(&list, tag, cmd);
    ctk_dma_list_va_free_elems(&list)
#else
    for (i = 0; i < transfer_size_y; i++) {
        ea_addr_t addr = ea_base + (transfer_start_y + i) * region_size_x 
                                      + transfer_start_x;
        spu_mfcdma64((volatile void*)((ls_addr_t)ls_addr + transfer_size_x * i),
            ea_hi32(addr), ea_lo32(addr), transfer_size_x, tag, cmd);
    }
#endif
}

/**
 * DMA get operation for 2D region
 * @param[in] ls_addr - destination LS addr
 * @param[in] ea_base - EA base addr
 * @param[in] region_size_x - the width (X size) of the region 
 * @param[in] transfer_start_x - X left-top corner point of the region to be transfered
 * @param[in] transfer_start_y - Y left-top corner point of the region to be transfered
 * @param[in] tansfer_size_x - size in X to be transfered
 * @param[in] tansfer_size_y - size in Y to be transfered
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void
ctk_dma_2d_get(void *ls_addr, ea_addr_t ea_base, int region_size_x, int transfer_start_x, int transfer_start_y, int transfer_size_x, int transfer_size_y, int tag)
{
    ctk_dma_2d_transfer(ls_addr, ea_base, region_size_x, 
        transfer_start_x, transfer_start_y, 
        transfer_size_x, transfer_size_y,
        tag, MFC_GET_CMD);
}

/**
 * blocking DMA get operation for 2D region
 * @param[in] ls_addr - destination LS addr
 * @param[in] ea_base - EA base addr
 * @param[in] region_size_x - the width (X size) of the region 
 * @param[in] transfer_start_x - X left-top corner point of the region to be transfered
 * @param[in] transfer_start_y - Y left-top corner point of the region to be transfered
 * @param[in] tansfer_size_x - size in X to be transfered
 * @param[in] tansfer_size_y - size in Y to be transfered
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void
ctk_dma_2d_get_block(void *ls_addr, ea_addr_t ea_base, int region_size_x, int transfer_start_x, int transfer_start_y, int transfer_size_x, int transfer_size_y, int cmd)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_2d_get(ls_addr, ea_base, region_size_x, 
        transfer_start_x, transfer_start_y, 
        transfer_size_x, transfer_size_y,
        tag);
    ctk_dma_wait(tag);
}

/**
 * DMA put operation for 2D region
 * @param[in] ls_addr - source LS addr
 * @param[in] ea_base - EA base addr
 * @param[in] region_size_x - the width (X size) of the region 
 * @param[in] transfer_start_x - X left-top corner point of the region to be transfered
 * @param[in] transfer_start_y - Y left-top corner point of the region to be transfered
 * @param[in] tansfer_size_x - size in X to be transfered
 * @param[in] tansfer_size_y - size in Y to be transfered
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void
ctk_dma_2d_put(void *ls_addr, ea_addr_t ea_base, int region_size_x, int transfer_start_x, int transfer_start_y, int transfer_size_x, int transfer_size_y, int tag)
{
    ctk_dma_2d_transfer(ls_addr, ea_base, region_size_x, 
        transfer_start_x, transfer_start_y, 
        transfer_size_x, transfer_size_y,
        tag, MFC_PUT_CMD);
}

/**
 * blocking DMA put operation for 2D region
 * @param[in] ls_addr - source LS addr
 * @param[in] ea_base - EA base addr
 * @param[in] region_size_x - the width (X size) of the region 
 * @param[in] transfer_start_x - X left-top corner point of the region to be transfered
 * @param[in] transfer_start_y - Y left-top corner point of the region to be transfered
 * @param[in] tansfer_size_x - size in X to be transfered
 * @param[in] tansfer_size_y - size in Y to be transfered
 * @param[in] tag - DMA tag to be associated to the initiating transfer
 * @return void
 */
static inline void
ctk_dma_2d_put_block(void *ls_addr, ea_addr_t ea_base, int region_size_x, int transfer_start_x, int transfer_start_y, int transfer_size_x, int transfer_size_y, int cmd)
{
    int tag = ctk_dma_new_tag();
    ctk_dma_2d_put(ls_addr, ea_base, region_size_x, 
        transfer_start_x, transfer_start_y, 
        transfer_size_x, transfer_size_y,
        tag);
    ctk_dma_wait(tag);
}


#ifdef __cplusplus
}
#endif

#endif /* __SPU__ */

#endif /* _CTK_SPU_DMA_2D_H */
