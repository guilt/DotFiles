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

#ifndef _CTK_DMA_COMMON_H
#define _CTK_DMA_COMMON_H

/**
 * @file ctk_dma_common.h
 * @brief Definitions for PPU and SPU common DMA routines / constants
 */

#if defined(__SPU__) && defined(HAVE_SPU_MFCIO_H)
# include <spu_mfcio.h>
#endif

/* DMA transfer alignment & size constants -------------------------------- */
#ifndef DMA_ALIGN_SIZE
#define DMA_MIN_ALIGN_SIZE     (16)   /**< DMA minimum Alignment size */
#define DMA_ALIGN_SIZE         (128)  /**< DMA optimal Alignment size */
#endif /* DMA_ALIGN_SIZE */

#ifndef MFC_MIN_DMA_SIZE
#define MFC_MIN_DMA_SIZE_SHIFT  4       /* 16 bytes */
#define MFC_MAX_DMA_SIZE_SHIFT  14      /* 16384 bytes */
#define MFC_MIN_DMA_SIZE        (1 << MFC_MIN_DMA_SIZE_SHIFT)
#define MFC_MAX_DMA_SIZE        (1 << MFC_MAX_DMA_SIZE_SHIFT)
#define MFC_MIN_DMA_SIZE_MASK   (MFC_MIN_DMA_SIZE - 1)
#define MFC_MAX_DMA_SIZE_MASK   (MFC_MAX_DMA_SIZE - 1)
#define MFC_MIN_DMA_LIST_SIZE   0x0008  /*   8 bytes */
#define MFC_MAX_DMA_LIST_SIZE   0x4000  /* 16K bytes */
#endif

#ifndef DMA_TRANSFER_SIZE
#define DMA_TRANSFER_SIZE   MFC_MAX_DMA_SIZE /**< DMA transfer size unit */
#endif /* DMA_TRANSFER_SIZE */

#define DMA_ALIGN_DOWN(v)   ALIGN_DOWN(v, DMA_ALIGN_SIZE)
#define DMA_ALIGN_UP(v)     ALIGN_UP(v, DMA_ALIGN_SIZE)

#ifndef MFC_PUT_CMD
/* MFC DMA Commands -------------------------------------------------------- */
#define MFC_PUT_CMD             0x20
#define MFC_PUTS_CMD            0x28
#define MFC_PUTR_CMD            0x30
#define MFC_PUTF_CMD            0x22
#define MFC_PUTB_CMD            0x21
#define MFC_PUTFS_CMD           0x2A
#define MFC_PUTBS_CMD           0x29
#define MFC_PUTRF_CMD           0x32
#define MFC_PUTRB_CMD           0x31
#define MFC_PUTL_CMD            0x24
#define MFC_PUTRL_CMD           0x34
#define MFC_PUTLF_CMD           0x26
#define MFC_PUTLB_CMD           0x25
#define MFC_PUTRLF_CMD          0x36
#define MFC_PUTRLB_CMD          0x35

#define MFC_GET_CMD             0x40
#define MFC_GETS_CMD            0x48
#define MFC_GETF_CMD            0x42
#define MFC_GETB_CMD            0x41
#define MFC_GETFS_CMD           0x4A
#define MFC_GETBS_CMD           0x49
#define MFC_GETL_CMD            0x44
#define MFC_GETLF_CMD           0x46
#define MFC_GETLB_CMD           0x45

#define MFC_SDCRT_CMD           0x80
#define MFC_SDCRTST_CMD         0x81
#define MFC_SDCRZ_CMD           0x89
#define MFC_SDCRS_CMD           0x8D
#define MFC_SDCRF_CMD           0x8F

#define MFC_GETLLAR_CMD         0xD0
#define MFC_PUTLLC_CMD          0xB4
#define MFC_PUTLLUC_CMD         0xB0
#define MFC_PUTQLLUC_CMD        0xB8
#define MFC_SNDSIG_CMD          0xA0
#define MFC_SNDSIGB_CMD         0xA1
#define MFC_SNDSIGF_CMD         0xA2
#define MFC_BARRIER_CMD         0xC0
#define MFC_EIEIO_CMD           0xC8
#define MFC_SYNC_CMD            0xCC

/* MFC Tag Status Update definitions for channel 23 (MFC_WrTagUpdate) ------ */
#define MFC_TAG_UPDATE_IMMEDIATE   0x0
#define MFC_TAG_UPDATE_ANY         0x1
#define MFC_TAG_UPDATE_ALL         0x2
#endif /* !MFC_PUT_CMD */

/* MFC_CMDStatus register constants & macros (see BEA 8.3.1) --------------- */
#define MFC_CMD_STATUS_RC_MASK          0x03
#define MFC_CMD_STATUS_RC(reg)          (MFC_CMD_STATUS_RC_MASK&(reg))
#define MFC_CMD_STATUS_RC_SUCCESS_MASK   0x00  /**< command success */
#define MFC_CMD_STATUS_RC_ERROR_SEQ_MASK 0x01 /**< sequence error */
#define MFC_CMD_STATUS_RC_ERROR_ENQ_MASK 0x10 /**< command enqueue failure
                                                because of out of space */
#define MFC_CMD_STATUS_CHECK_RC(reg,mask)   \
        ((MFC_CMD_STATUS_RC(reg) & (mask)) == (mask))

/**
 * Checks if the MFC command status indicates success
 * @param[in] reg - value of the MFC command status register
 */
#define MFC_CMD_STATUS_IS_SUCCESS(reg) \
        MFC_CMD_STATUS_CHECK_RC(reg, MFC_CMD_STATUS_RC_SUCCESS_MASK)

/**
 * Checks if the MFC command status indicates error
 * @param[in] reg - value of the MFC command status register
 */
#define MFC_CMD_STATUS_IS_ERROR(reg) \
        (!MFC_CMD_STATUS_CHECK_RC(reg, MFC_CMD_STATUS_RC_SUCCESS_MASK))

/**
 * Checks if the MFC command status indicates command sequence error
 * @param[in] reg - value of the MFC command status register
 */
#define MFC_CMD_STATUS_IS_ERROR_SEQ(reg) \
        MFC_CMD_STATUS_CHECK_RC(reg, MFC_CMD_STATUS_RC_ERROR_SEQ_MASK)

/**
 * Checks if the MFC command status indicates enqueue error
 * @param[in] reg - value of the MFC command status register
 */
#define MFC_CMD_STATUS_IS_ERROR_ENQUEUE(reg) \
        MFC_CMD_STATUS_CHECK_RC(reg, MFC_CMD_STATUS_RC_ERROR_ENQ_MASK)

/* MFC_QStatus register constants & macros (see BEA 8.3.2) ---------------- */
#define MFC_PRXY_STATUS_QUEUE_EMPTY        (1<<31)
#define MFC_PRXY_STATUS_QUEUE_NOT_EMPTY    0x0

/**
 * Checks if the MFC proxy queue status is empty
 * @param[in] reg - value of the MFC proxy queue status register
 */
#define MFC_PRXY_STATUS_IS_QUEUE_EMPTY(reg)    (reg != 0x0)

/** Proxy tag-group query type to query ANY of the tag completion */
#define CTK_MFC_PRXY_QUERYTYPE_ANY     1
/** Proxy tag-group query type to query ALL of the tag completion */
#define CTK_MFC_PRXY_QUERYTYPE_ALL     2

/* ------------------------------------------------------------------------- */
/**
 * Adds (sets) the given DMA tag to the given tag_mask
 */
static inline void CTK_DMA_TAG_SET(int tag, unsigned int *tag_mask) {
    (*tag_mask) |= (1 << tag);
}

/**
 * Checks if the given DMA tag is set in the given tag_mask
 */
static inline int CTK_DMA_TAG_ISSET(int tag, unsigned int *tag_mask) {
    return ((*tag_mask) | (1 << tag));
}

/**
 * Clears the given tag from the given tag_mask
 */
static inline void CTK_DMA_TAG_CLR(int tag, unsigned int *tag_mask) {
    (*tag_mask) &= ~(1 << tag);
}

/**
 * Clears (zeros) all the given tag_mask
 */
static inline void CTK_DMA_TAG_ZERO(unsigned int *tag_mask) {
    (*tag_mask) = 0x0;
}

#endif /* _CTK_DMA_COMMON_H */
