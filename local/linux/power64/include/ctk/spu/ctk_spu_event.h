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

#ifndef _CTK_SPU_EVENT_H
#define _CTK_SPU_EVENT_H

/** 
 * @file ctk_spu_event.h
 * @brief Definitions for SPU event handling operations
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MFC_DMA_QUEUE_AVAILABLE_EVENT
/* Events for Channels 0-2 */
# define MFC_DMA_TAG_STATUS_UPDATE_EVENT     0x00000001
# define MFC_DMA_TAG_CMD_STALL_NOTIFY_EVENT  0x00000002
# define MFC_DMA_QUEUE_AVAILABLE_EVENT       0x00000008
# define MFC_SPU_MAILBOX_WRITTEN_EVENT       0x00000010
# define MFC_DECREMENTER_EVENT               0x00000020
# define MFC_PU_INT_MAILBOX_AVAILABLE_EVENT  0x00000040
# define MFC_PU_MAILBOX_AVAILABLE_EVENT      0x00000080
# define MFC_SIGNAL_2_EVENT                  0x00000100
# define MFC_SIGNAL_1_EVENT                  0x00000200
# define MFC_LLR_LOST_EVENT                  0x00000400
# define MFC_PRIV_ATTN_EVENT                 0x00000800
# define MFC_MULTI_SRC_EVENT                 0x00001000
#endif

/* the define has changed from Cell SDK 1.1 -> 2.0 */
#ifdef SPU_RdEventStatMask
# define SPU_RdEventMask     SPU_RdEventStatMask
#endif

/**
 * Gets the count of unhandled SPU events (MAX = 1).
 * @return 1 if there's an event, 0 otherwise.
 */
static inline int ctk_event_count()
{
    return spu_readchcnt(SPU_RdEventStat);
}

/**
 * Reads the SPU event status. [Blocking]
 * SPU stalls if there's no pending event.
 */
static inline int ctk_event_read()
{
    return spu_readch(SPU_RdEventStat);
}

/**
 * Acks an SPU event and resets it. [Non-Blocking]
 * @note
 *  Events that have been reported (i.e., enabled and not masked)
 *  continue to be reported until acknowleged (unless cleared by
 *  the privileged software).
 *  from CBE Architecture's guide: "Software should acknowledge all
 *  events to be processed before processing the vents."
 */
static inline void ctk_event_ack(unsigned int event)
{
    spu_writech(SPU_WrEventAck, event);
}

/**
 * Reads the current interrupt (event) mask.
 * @return the event mask
 */
static inline unsigned int ctk_event_get_mask()
{
    return spu_readch(SPU_RdEventMask);
}

/**
 * Sets the interrupt (event) mask to the new value
 * and returns its previous value.
 * @param[in] new - the new event mask to be set
 * @return the old event mask
 */
static inline unsigned int ctk_event_set_mask(unsigned int new_mask)
{
    unsigned int old, tmp = 0;
    old = spu_readch(SPU_RdEventMask);
    spu_writech(SPU_WrEventMask, tmp);
    if (spu_readchcnt(SPU_RdEventStat)) {
        tmp = spu_readch(SPU_RdEventStat);
        spu_writech(SPU_WrEventAck, tmp);
    }
    spu_writech(SPU_WrEventMask, new_mask);
    return old;
}

/**
 * Checks the current SPU machine status and returns 1 if interrupts
 * are enabled.
 * @return 1 if interrupts (events) are enabled, 0 otherwise.
 */
static inline int ctk_event_check_enabled()
{
    unsigned int status = spu_readch(SPU_RdMachStat);
    return status & 0x1;
}

/**
 * Enables interrupts.
 */
static inline void ctk_event_enable()
{
    spu_ienable();
}

/**
 * Disables interrupts and returns the old interrupts status.
 * @return the old interrupt status
 */
static inline unsigned int ctk_event_disable()
{
    unsigned int status = spu_readch(SPU_RdMachStat);
    spu_idisable();
    return (status & 0x1);
}

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SPU_EVENT_H */
