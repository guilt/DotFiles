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

#ifndef _CTK_SPU_PROFILE_H
#define _CTK_SPU_PROFILE_H

/**
 * @file ctk_spu_profile.h
 * @brief Definitions for SPU profile routines
 */

#include "ctk_spu_common.h"
#include <limits.h>

#ifdef __SPU__

#ifdef __cplusplus
extern "C" {
#endif

/** profiling data structure */
typedef struct {
    signed int start;
    signed int end;
} ctk_profile_t;

/**
 * Starts the profile counter.
 * @param[in] prof - the current profiling context
 * @note this writes the decrementer counter.
 */
static inline void ctk_profile_start(ctk_profile_t *prof) {
    ctk_do_profiling = 1;
    spu_writech(SPU_WrDec, INT_MAX);
    prof->start = spu_readch(SPU_RdDec);
}

/**
 * Reads the current profile counter in msec.
 * @param[in] prof - the current profiling context
 * @return profiled time in msec
 */
static inline unsigned int ctk_profile_read(ctk_profile_t *prof) {
    prof->end = spu_readch(SPU_RdDec);
    return (prof->start - prof->end) / 
                (unsigned int)(ctk_timebase_clock * 1.0e-3);
}

/**
 * Reads the current profile counter in msec.
 * @param[in] prof - the current profiling context
 * @return profiled time in msec
 */
static inline unsigned int ctk_profile_read_in_msec(ctk_profile_t *prof) {
    prof->end = spu_readch(SPU_RdDec);
    return (prof->start - prof->end) / 
                (unsigned int)(ctk_timebase_clock * 1.0e-3);
}

/**
 * Reads and stops the profile counter.
 * @param[in] prof - the current profiling context
 * @return profiled time in msec
 */
static inline unsigned int ctk_profile_end(ctk_profile_t *prof) {
    prof->end = spu_readch(SPU_RdDec);
    ctk_do_profiling = 0;
    return (prof->start - prof->end) / 
                (unsigned int)(ctk_timebase_clock * 1.0e-3);
}

/**
 * Reads the current profile counter in usec.
 * @param[in] prof - the current profiling context
 * @return profiled time in usec
 */
static inline int ctk_profile_read_in_usec(ctk_profile_t *prof) {
    prof->end = spu_readch(SPU_RdDec);
    return (prof->start - prof->end) / 
                (unsigned int)(ctk_timebase_clock * 1.0e-6);
}

/**
 * Reads the current profile counter in raw timebase clock.
 * @param[in] prof - the current profiling context
 * @return profiled time in timebase clock
 */
static inline int ctk_profile_read_in_clock(ctk_profile_t *prof) {
    prof->end = spu_readch(SPU_RdDec);
    return (prof->start - prof->end);
}

/** Initializes the profile counter */
static inline void ctk_profile_init() {
    ctk_do_profiling = 1;
    spu_writech(SPU_WrDec, INT_MAX);
}

/** Resets the profile counter */
static inline void ctk_profile_reset(ctk_profile_t *prof) {
    prof->start = spu_readch(SPU_RdDec);
}


#ifdef PROFILE
# include <stdio.h>
# define PROFILE_START() { \
    ctk_profile_t prof; \
    ctk_profile_start(&prof);
# define PROFILE_END(...) \
    int __clocks = ctk_profile_read_in_clock(&prof); \
    char __buf[256]; \
    sprintf(__buf, __VA_ARGS__); \
    printf("[profile %s:%d] %s: %d clocks.\n", \
        __FILE__, __LINE__, __buf, __clocks); \
    }
#else /* !PROFILE */
# define PROFILE_START()  /**< Starts profile only if PROFILE is defined */
# define PROFILE_END(fmt,...) /**< Ends profile and displays the result only if
                               PROFILE is defined */
#endif /* !PROFILE */

#ifdef __cplusplus
}
#endif

#endif /* __SPU__ */

#endif /* _CTK_SPU_PROFILE_H */
