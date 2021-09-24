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

#ifndef _CTK_MBOX_H
#define _CTK_MBOX_H

/**
 * @file ctk_mbox.h
 * @brief Definitions for PPU-side Mailbox related functions and macros.
 *
 * @note Most of the Mailbox functions initiated by PPU are
 * <b>non-blocking</b>.
 */

#include "ctk_common.h"
#include "ctk_spe.h"
#include "ctk_spe_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CTK_MBOX_OUT_MAX_ENTRIES      1 /**< Max entries of outbound mbox */
#define CTK_MBOX_IN_MAX_ENTRIES       4 /**< Max entries of nbound mbox */
#define CTK_MBOX_OUT_INTR_MAX_ENTRIES 1 /**< Max entries of outbound interrupt
                                             mbox */

#define CTK_MBOX_OUT_STAT_SHIFT       0  /**< shift in the MboxStat */
#define CTK_MBOX_IN_STAT_SHIFT        8  /**< shift in the MboxStat */
#define CTK_MBOX_OUT_INTR_STAT_SHIFT  16 /**< shift in the MboxStat */

/**
 * Reads the contents of the SPU outbound mailbox for the given SPE.
 * [Non-blocking]
 * This read is non-blocking and returns CTK_ERROR_NO_DATA
 * if no mailbox data is available.
 *
 * @see #ctk_mbox_read_block for blocking call
 * @param[in] spe - the SPE whose outbound mailbox is to be read.
 * @param[out] data - the value of the Mailbox.
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_mbox_read(ctk_spe_t spe, ctk_uint32_t *data);

/**
 * Reads the contents of the SPU outbound mailbox for the given SPE.
 * [Non-blocking]
 * This read is non-blocking and returns CTK_ERROR_NO_DATA
 * if no mailbox data is available.
 *
 * @see #ctk_intr_mbox_read_block for blocking call
 * @param[in] spe - the SPE whose outbound intr mailbox is to be read.
 * @param[in] data - the value of the Mailbox.
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_intr_mbox_read(ctk_spe_t spe, ctk_uint32_t *data);

/**
 * Writes the 32-bit data specified by #data into the SPU inbound
 * mailbox for the given SPE. [Non-blocking]
 * If the mailbox is full, then ctk_mbox_write may overwrite the last
 * entry in the mailbox. The ctk_mbox_write_count function can be called to
 * ensure the space is available prior to writing to the inbound mailbox.
 *
 * @see #ctk_mbox_write_block for blocking call
 * @param[in] spe - the SPE whose inbound mailbox is to be written.
 * @param[in] data - the value to be written.
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_mbox_write(ctk_spe_t spe, ctk_uint32_t data);

/**
 * Gets the count of empty data slots of the SPU inbound
 * mailbox for the given SPE.
 * @param[in] spe - the SPE whose inbound mailbox is to be examined.
 * @return the number of (32-bit) empty (available) mailbox slots (>= 0).
 *  A 0 value indicates the mailbox is <b>full</b>.
 *  On failure, -errno is returned (< 0).
 */
int ctk_mbox_write_count(ctk_spe_t spe);

/**
 * Gets the count of unread entries of the SPU outbound mailbox for the
 * given SPE.
 * @param[in] spe - the SPE whose outbound mailbox is to be examined.
 * @return the number of unread (32-bit) mailbox entries (>= 0).
 *  A 0 value indicates the mailbox is <b>empty</b>.
 *  On failure, -errno is returned (< 0).
 */
int ctk_mbox_read_count(ctk_spe_t spe);

/**
 * Gets the count of the unread entries of the SPU outbound interrupt mailbox
 * for the given SPE.
 * @param[in] spe - the SPE whose outbound interrupt mailbox is to
 *  be examined.
 * @return the number of (32-bit) unread mailbox entries (>= 0).
 *  A 0 value indicates the mailbox is <b>empty</b>.
 *  On failure, -errno is returned (< 0).
 */
int ctk_intr_mbox_read_count(ctk_spe_t spe);

/* alias for backward compatibility */
#define ctk_mbox_interrupt_count(spe)   ctk_intr_mbox_read_count(spe)

/**
 * Reads the contents of the SPU outbound mailbox for the given SPE.
 * This blocks if the mailbox is empty. [Blocking]
 * @param[in] spe - the SPE whose outbound mailbox is to be read.
 * @param[out] data - the value of the Mailbox.
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_mbox_read_block(ctk_spe_t spe, ctk_uint32_t *data);

/**
 * Reads the contents of the SPU outbound interrupt mailbox for the given SPE.
 * This blocks if the mailbox is empty. [Blocking]
 * @param[in] spe - the SPE whose outbound interrupt mailbox is to be read.
 * @param[out] data - the value of the Mailbox.
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_intr_mbox_read_block(ctk_spe_t spe, ctk_uint32_t *data);

/**
 * Writes the 32-bit data specified by #data into the SPU inbound
 * mailbox for the given SPE. [Blocking]
 * The call blocks if the mailbox is full.
 * @param[in] spe - the SPE whose inbound mailbox is to be written.
 * @param[in] data - the value to be written.
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_mbox_write_block(ctk_spe_t spe, ctk_uint32_t data);

#ifdef __cplusplus
}
#endif

#endif /* _CTK_MBOX_H */
