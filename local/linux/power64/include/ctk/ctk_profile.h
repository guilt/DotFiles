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

#ifndef _CTK_PROFILE_H
#define _CTK_PROFILE_H

/**
 * @file ctk_profile.h
 * @brief Definitions for profile routines (using timebase register)
 */

#include "ctk_common.h"
#include "ctk_init.h"
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/** profiling data structure */
typedef struct {
    unsigned long long start;
    unsigned long long end;
} ctk_profile_t;

/**
 * Reads the current time base register value
 * @return 64-bit timebase register value
 */
static inline unsigned long long
ctk_profile_gettimeval() {
    register unsigned long tbu, tbl, tbu_check;
    do {
        __asm__ __volatile__ ("mftbu %0" : "=r" (tbu));
        __asm__ __volatile__ ("mftbl %0" : "=r" (tbl));
        /* to make sure there hasn't been a carry over between 
           checking tbu and tbl */
        __asm__ __volatile__ ("mftbu %0" : "=r" (tbu_check));
    } while (tbu != tbu_check);
    return ((unsigned long long)tbu << 32 | (unsigned long long)tbl);
}

/**
 * Starts the profile counter.
 * @param[in] prof - the current profiling context
 */
static inline void ctk_profile_start(ctk_profile_t *prof) {
    CTK_DEBUG_ASSERT(prof != NULL);
    prof->start = ctk_profile_gettimeval();
}

/**
 * Reads the current profile counter in msec.
 * @param[in] prof - the current profiling context
 * @return profiled time in msec
 */
static inline int ctk_profile_read(ctk_profile_t *prof) {
    prof->end = ctk_profile_gettimeval();
    return (prof->end - prof->start) / (int)(ctk_timebase_clock * 1.0e-3);
}

/**
 * Reads the current profile counter in msec.
 * @param[in] prof - the current profiling context
 * @return profiled time in msec
 */
static inline int ctk_profile_read_in_msec(ctk_profile_t *prof) {
    prof->end = ctk_profile_gettimeval();
    return (prof->end - prof->start) / (int)(ctk_timebase_clock * 1.0e-3);
}

/**
 * Reads and stops the profile counter.
 * @param[in] prof - the current profiling context
 * @return profiled time in msec
 * @note this is just an alias for ctk_profile_read()
 */
static inline int ctk_profile_end(ctk_profile_t *prof) {
    return ctk_profile_read(prof);
}

/**
 * Reads the current profile counter in usec.
 * @param[in] prof - the current profiling context
 * @return profiled time in usec
 */
static inline int ctk_profile_read_in_usec(ctk_profile_t *prof) {
    prof->end = ctk_profile_gettimeval();
    return (prof->end - prof->start) / (int)(ctk_timebase_clock * 1.0e-6);
}

/**
 * Reads the current profile counter in raw timebase clock.
 * @param[in] prof - the current profiling context
 * @return profiled time in timebase clock
 */
static inline int ctk_profile_read_in_clock(ctk_profile_t *prof) {
    prof->end = ctk_profile_gettimeval();
    return (prof->end - prof->start);
}

#ifdef __cplusplus
}
#endif

#endif /* _CTK_PROFILE_H */

