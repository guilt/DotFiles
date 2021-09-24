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

#ifndef _CTK_SPU_MBOX_H
#define _CTK_SPU_MBOX_H

/**
 * @file ctk_spu_mbox.h
 * @brief Definitions for SPU-side Mailbox related functions and macros
 *
 *  Note that most of the Mailbox functions initiated by SPU
 * <b>blocking</b>.
 */

#include "ctk_spu_common.h"

#ifdef __SPU__

#ifdef __cplusplus
extern "C" {
#endif

/* for PPU API compatibility */
#define ctk_mbox_write_block    ctk_mbox_write
#define ctk_mbox_read_block     ctk_mbox_read
#define ctk_intr_mbox_read_block     ctk_intr_mbox_read

/**
 * Reads the contents of the SPU inbound mailbox. [Blocking]
 * SPU stalls if the mbox has no unread entry.
 * @param[out] data - the value of the Mailbox.
 */
static inline void ctk_mbox_read(ctk_uint32_t *data) __attribute__((always_inline));
static inline void
ctk_mbox_read(ctk_uint32_t *data)
{
    *data = spu_readch(SPU_RdInMbox);
}

/**
 * Writes the 32-bit data specified by #data into the SPU outbound
 * mailbox. [Blocking]
 * SPU stalls if the mbox is full and count of the empty slots is 0.
 * @param[in] data - the value to be written.
 */
static inline void ctk_mbox_write(ctk_uint32_t data) __attribute__((always_inline));
static inline void
ctk_mbox_write(ctk_uint32_t data)
{
    spu_writech(SPU_WrOutMbox, data);
}

/**
 * Writes the 32-bit data specified by #data into the SPU outbound
 * interrupt mailbox. [Blocking]
 * SPU stalls if the mbox is full and count of the empty slots is 0.
 * @param[in] data - the value to be written.
 */
static inline void ctk_intr_mbox_write(ctk_uint32_t data) __attribute__((always_inline));
static inline void
ctk_intr_mbox_write(ctk_uint32_t data)
{
    spu_writech(SPU_WrOutIntrMbox, data);
}

/**
 * Gets the unread entries of the SPU inbound mailbox.
 * @return the number of nread (32-bit) mailbox entries.
 *  A value 0 indicates the mbox is <b>empty</b> and succeeding read will block.
 */
static inline int ctk_mbox_read_count() __attribute__((always_inline));
static inline int
ctk_mbox_read_count()
{
    return spu_readchcnt(SPU_RdInMbox);
}

/**
 * Gets the empty (available) slots of the SPU outbound mailbox.
 * @return the number of available (32-bit) mailbox entries.
 *  A value 0 indicates the mbox is <b>full</b> and succeeding write will block.
 */
static inline int ctk_mbox_write_count() __attribute__((always_inline));
static inline int
ctk_mbox_write_count()
{
    return spu_readchcnt(SPU_WrOutMbox);
}

/**
 * Gets the empty (available) slots of the SPU outbound interrupt mailbox.
 * @return the number of available (32-bit) mailbox entries.
 *  A value 0 indicates the mbox is <b>full</b> and succeeding interrupt will block.
 */
static inline int ctk_intr_mbox_write_count() __attribute__((always_inline));
static inline int
ctk_intr_mbox_write_count()
{
    return spu_readchcnt(SPU_WrOutIntrMbox);
}

/* for backward compatibility */
#define ctk_mbox_interrupt_count()      ctk_intr_mbox_write_count()
#define ctk_mbox_interrupt(data)        ctk_intr_mbox_write(data)

#ifdef __cplusplus
}
#endif

#endif /* __SPU__ */

#endif /* _CTK_SPU_MBOX_H */
