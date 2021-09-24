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

#ifndef _CTK_SIGNAL_H
#define _CTK_SIGNAL_H

/**
 * @file ctk_signal.h
 * @brief Definitions for signal helpers.
 */
#include "ctk_common.h"
#include "ctk_spe.h"
#include "ctk_spe_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CTK_SIGNAL_REG1     0x01
#define CTK_SIGNAL_REG2     0x02

/**
 * Writes the #data to the signal notification register specified by
 * #reg for the given SPE. [This will block until the SPE starts
 * if it has not yet started]
 * @param[in] spe - the SPE whose outbound mailbox is to be read.
 * @param[in] reg - the signal register. This can be one of either
 *   #CTK_SIGNAL_REG1 or #CTK_SIGNAL_REG2.
 * @param[in] data - the 32-bit data to be written
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
int ctk_signal_write(ctk_spe_t spe, unsigned int reg, unsigned int data);

/**
 * Writes the #data to the signal notification register 1
 * for the given SPE.
 * @param[in] spe - the SPE whose outbound mailbox is to be read.
 * @param[in] data - the 32-bit data to be written
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
static inline int ctk_signal_write_reg1(ctk_spe_t spe, unsigned int data) __attribute__((always_inline)); 
static inline int
ctk_signal_write_reg1(ctk_spe_t spe, unsigned int data)
{
    return ctk_signal_write(spe, CTK_SIGNAL_REG1, data);
}

/**
 * Writes the #data to the signal notification register 2
 * for the given SPE.
 * @param[in] spe - the SPE whose outbound mailbox is to be read.
 * @param[in] data - the 32-bit data to be written
 * @return CTK_SUCCESS (0) on success, errno on failure.
 */
static inline int ctk_signal_write_reg2(ctk_spe_t spe, unsigned int data) __attribute__((always_inline)); 
static inline int
ctk_signal_write_reg2(ctk_spe_t spe, unsigned int data)
{
    return ctk_signal_write(spe, CTK_SIGNAL_REG2, data);
}

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SIGNAL_H */
