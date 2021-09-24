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

#ifndef _CTK_SPU_MISC_H
#define _CTK_SPU_MISC_H

#include "ctk_spu_common.h"
#include "ctk_spu_event.h"
#include <limits.h>

/**
 * @file ctk_spu_misc.h
 * @brief Definitions for SPU misc routines
 */

#ifdef __SPU__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sleeps (spin-lock-waits) for the given microseconds.
 * @param[in] usec - sleep period
 * @return 0 on success, -1 on error
 */
static inline int ctk_usleep(unsigned long usec) {
    ctk_uint32_t init, current;
    ctk_uint32_t oldmask;
    ctk_uint32_t tb_sleep_count = (ctk_uint32_t)
           (usec * ((float)ctk_timebase_clock * 1.0e-6));

    if (!ctk_do_profiling)
        spu_writech(SPU_WrDec, INT_MAX);

    init = spu_readch(SPU_RdDec);
    oldmask = ctk_event_set_mask(MFC_DECREMENTER_EVENT);
    while (1) {
        if (spu_readchcnt(SPU_RdEventStat)) {
            unsigned int tmp = spu_readch(SPU_RdEventStat);
            spu_writech(SPU_WrEventAck, tmp);
            spu_writech(SPU_WrDec, UINT_MAX);
        }
        current = spu_readch(SPU_RdDec);
        if (init - current > tb_sleep_count)
            break;
        if (current <= 0) {
            tb_sleep_count -= (init - current);
            spu_writech(SPU_WrDec, UINT_MAX);
        }
    }
    ctk_event_set_mask(oldmask);
    return 0;
}

/**
 * Sleeps (spin-lock-waits) for the given seconds.
 * @param[in] sec - sleep period
 * @return 0 on success, -1 on error
 * @note This reads/writes decrementer.
 */
static inline int ctk_sleep(unsigned long sec) {
    unsigned long i;
    for (i = 0; i < sec; i++)
        ctk_usleep(1000 * 1000);
    return 0;
}

static signed __ctk_rand_seed = 1;

/**
 * Initializes an initial random number. (simple LCG)
 */
static inline void ctk_srand(signed int s)
{
    __ctk_rand_seed = s;
}

/**
 * Generates a random 31-bit integer. (simple LCG & not efficient)
 */
static inline signed int ctk_rand()
{
    __ctk_rand_seed = __ctk_rand_seed * 69621;
    return (__ctk_rand_seed & 2147483647);
}

/**
 * Initializes the random seed with Decrementer counter
 */
#define ctk_init_rand()     ctk_srand(spu_readch(SPU_RdDec))

/**
 * Issues an MFC eieio (enforce in-order execution of I/O) command.
 * EIEIO creates a store-store memory barrier for cacheable memory
 * and creates a full barrier for noncacheable memory
 * (i.e. MMIO or PIO registers).
 * (alias to ctk_eieio())
 */
static inline void
ctk_write_barrier(int tag) {
    spu_mfcdma32(0, 0, 0, tag, MFC_EIEIO_CMD);
}

/**
 * Issue an MFC sync command to create a memory barrier.
 * (alias to ctk_sync())
 */
static inline void
ctk_full_barrier(int tag) {
    spu_mfcdma32(0, 0, 0, tag, MFC_SYNC_CMD);
}

/**
 * Issue an MFC eieio (enforce in-order execution of I/O) command.
 * EIEIO creates a store-store memory barrier for cacheable memory
 * and creates a full barrier for noncacheable memory
 * (i.e. MMIO or PIO registers).
 */
static inline void
ctk_eieio(int tag) {
    spu_mfcdma32(0, 0, 0, tag, MFC_EIEIO_CMD);
}

/**
 * Issue an MFC sync command 
 */
static inline void
ctk_sync(int tag) {
    spu_mfcdma32(0, 0, 0, tag, MFC_SYNC_CMD);
}

/**
 * Issue an MFC barrier
 */
static inline void
ctk_barrier(int tag) {
    spu_mfcdma32(0, 0, 0, tag, MFC_BARRIER_CMD);
}

#ifdef __cplusplus
}
#endif

/**
 * Returns the number of leading zeros
 */
static inline int ctk_nlz(unsigned int x) __attribute__((always_inline));
static inline int ctk_nlz(unsigned int x)
{
    return spu_extract(spu_cntlz(spu_splats(x)), 0);
}

/**
 * Returns the number of leading zeros in the given double word
 */
static inline int ctk_nlz64(unsigned long long x) __attribute__((always_inline));
static inline int ctk_nlz64(unsigned long long x)
{
    vec_uint4 clz = spu_cntlz((vec_uint4)spu_splats(x));
    vec_uint4 mask = spu_cmpeq(clz, 32U);
    return spu_extract(clz, 0) + 
           spu_extract(spu_sel(clz, spu_splats(0U), mask), 1);
}

/**
 * Returns the number of leading ones
 */
static inline int ctk_nlo(unsigned int x)
{
    return ctk_nlz(~x);
}

/**
 * Returns the number of leading ones in the given double word
 */
static inline int ctk_nlo64(unsigned long long x)
{
    return ctk_nlz64(~x);
}


#endif /* __SPU__ */

#endif /* _CTK_SPU_MISC_H */
