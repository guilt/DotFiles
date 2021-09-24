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

#ifndef _CTK_ERROR_H
#define _CTK_ERROR_H

/**
 * @file ctk_error.h
 * @brief Definitions for CTK error numbers and error routines.
 */

#define CTK_ERROR_BASE  1000

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <errno.h>

#ifndef _CTK_ERROR_GEN_H
# include "ctk_error_code.h"

/**
 * Returns true or false to indicate whether the given errno is in the
 * range of ctk_error or not.
 */
static inline int is_ctk_error(int errnum) {
    if (errnum < 0)
        errnum = -errnum;
    return (errnum >= CTK_ERROR_BASE && errnum <= CTK_ERROR_MAX);
}

#ifdef CELL_ENV_TSDK
/**
 * Translate the tSDK internal error code to CTK error code
 */
int ctk_error_translate_tsdk(unsigned int tsdk_error);
#endif

/**
 * Returns string describing the given error code
 * @param[in] errnum - the error code
 * @return the error message corresponding to the given error code.
 *   If the given error code does not fall in the range CTK_ERROR_BASE <
 *   errnum < CTK_ERROR_BASE + ctk_nerr, ctk_strerror returns "Unknown error".
 */
const char *ctk_strerror(int errnum);

/**
 * Returns string describing the given error code
 * @param[in] errnum - the error code
 * @param[out] buf - pointer to store the error message, which must be
 *   allocated by the caller.
 * @param[in] buflen - the length of the buffer
 * @return 0 (CTK_SUCCESS) on success, -1 on failure.
 */
int ctk_strerror_r(int errnum, char *buf, int buflen);

#ifndef __SPU__
/**
 * Prints out an error message corresponding to the given error code
 * to stderr.
 * @param[in] errnum - the error code
 * @param[in] msg - the error message which will be printed as the first
 *   part of the error message.
 */
void ctk_perror(int errnum, const char *msg);
#endif /* __SPU__ */

/**
 * Translate the tSDK internal error code to CTK error code
 */
int ctk_error_translate_unix(int unix_error);

#endif /* _CTK_ERROR_GEN_H */

#ifdef __cplusplus
}
#endif

#endif /* _CTK_ERROR_H */
