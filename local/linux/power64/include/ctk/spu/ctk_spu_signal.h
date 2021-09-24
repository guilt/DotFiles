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

#ifndef _CTK_SPU_SIGNAL_H
#define _CTK_SPU_SIGNAL_H

/**
 * @file ctk_spu_signal.h
 * @brief Definitions for SPU-side signal related functions and macros
 *
 *  Note that most of the signal functions initiated by SPU
 * <b>blocking</b>.
 */
#include "ctk_spu_common.h"

#ifdef __SPU__

#ifdef __cplusplus
extern "C" {
#endif

#define CTK_SIGNAL_REG1     SPU_RdSigNotify1
#define CTK_SIGNAL_REG2     SPU_RdSigNotify2

/**
 * Reads the value of the signal notification register specified by #reg.
 * [Blocking]
 * This blocks until it actually gets a signal notification.
 * @param[in] reg - specifies the signal notification register to be read.
 *    The value should be one of either #CTK_SIGNAL_REG1 or #CTK_SIGNAL_REG2.
 * @param[out] data - the 32-bit value
 * @return void
 */
static inline void ctk_signal_read(unsigned int reg, ctk_uint32_t *data) __attribute__((always_inline)); 
static inline void
ctk_signal_read(unsigned int reg, ctk_uint32_t *data)
{
    *data = spu_readch(reg);
}

/**
 * Sends a value to the signal notification register specified by the
 * given EA address.
 * [Blocking]
 * This blocks until it actually sends the signal notification.
 * @see BE Handbook pp.574 for more details.
 * @param[in] ea_addr - specifies EA addr of the target signal notification
 *  register.
 * @param[in] data - the 32-bit value
 * @return void
 */
static inline void
ctk_signal_send(ea_addr_t ea_addr, ctk_uint32_t data)
{
    volatile vector unsigned int signal_qword;
    int tag = ctk_dma_new_tag();
    spu_writech(MFC_WrTagMask, 1 << tag);

    /* The signall address must be at the address such like mod16 == 12 */
    CTK_DEBUG_ASSERT(ea_addr%16 == 12);

#if 0
    /* first empty the register */
    do {
        ctk_dma_get_block((void*)(&signal_qword) + 12, ea_addr, 4);
    } while (spu_extract(signal_qword, 3));
#endif

    /* place the requested data into the LS to be DMA'ed */
    signal_qword = spu_promote(data, 3);

    spu_dsync();

#if 1
    spu_mfcdma64((volatile void*)(((int)(&signal_qword)) + 12),
        ea_hi32(ea_addr), ea_lo32(ea_addr), 4, tag, MFC_SNDSIGB_CMD);
#else
    mfc_sndsig((volatile void*)(((int)(&signal_qword)) + 12),
        ea_addr, tag, 0, 0);
#endif

    ctk_eieio(tag);

    spu_writech(MFC_WrTagUpdate, MFC_TAG_UPDATE_ALL);
    spu_readch(MFC_RdTagStat);
}

/**
 * Reads the value of the signal notification register 1. [Blocking]
 * This blocks until it actually gets a signal notification.
 * @param[out] data - the 32-bit value
 * @return void
 */
static inline void ctk_signal_read_reg1(ctk_uint32_t *data) __attribute__((always_inline)); 
static inline void
ctk_signal_read_reg1(ctk_uint32_t *data)
{
    *data = spu_readch(SPU_RdSigNotify1);
}

/**
 * Reads the value of the signal notification register 2. [Blocking]
 * This blocks until it actually gets a signal notification.
 * @param[out] data - the 32-bit value
 * @return void
 */
static inline void ctk_signal_read_reg2(ctk_uint32_t *data) __attribute__((always_inline)); 
static inline void
ctk_signal_read_reg2(ctk_uint32_t *data)
{
    *data = spu_readch(SPU_RdSigNotify2);
}

/**
 * Gets the count of unread signal notifications for the signal notification
 * register specified by #reg.
 * @param[in] reg - specifies the signal notification register to be read.
 *    The value should be one of either CTK_SIGNAL_REG1 or CTK_SIGNAL_REG2.
 * @return the number of unread signal notifications.
 *  A 0 value indicates no signal notification has been written.
 */
static inline int ctk_signal_read_count(unsigned int reg) __attribute__((always_inline)); 
static inline int
ctk_signal_read_count(unsigned int reg)
{
    return spu_readchcnt(reg);
}

/**
 * Gets the count of unread signal notifications for the signal notification
 * register 1.
 * @return the number of unread signal notifications.
 *  A 0 value indicates the signal notification register is <b>empty</b>.
 */
static inline int ctk_signal_read_reg1_count() __attribute__((always_inline)); 
static inline int
ctk_signal_read_reg1_count()
{
    return spu_readchcnt(SPU_RdSigNotify1);
}

/**
 * Gets the count of unread signal notifications for the signal notification
 * register 2.
 * @return the number of unread signal notifications.
 *  A 0 value indicates no signal notification has been written.
 */
static inline int ctk_signal_read_reg2_count() __attribute__((always_inline)); 
static inline int
ctk_signal_read_reg2_count()
{
    return spu_readchcnt(SPU_RdSigNotify2);
}

#ifdef __cplusplus
}
#endif

#endif /* __SPU__ */

#endif /* _CTK_SPU_SIGNAL_H */
