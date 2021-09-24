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

#ifndef _CTK_SPE_PATH_H
#define _CTK_SPE_PATH_H

/**
 * @file ctk_spe_path.h
 * @brief Definitions for SPE module path resolve routines
 */

#define SPE_MODULE_PATH     "SPE_MODULE_PATH"
#define SPE_PATH_DEFAULT    "."

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ctk_spe_path_resolve: Searches spe module in the module paths defined
 * as environmental variable SPE_MODULE_PATH and returns the full path
 * if the module file is found.  If SPE_MODULE_PATH is not set, by default
 * it searches the current directory.
 * SPE_MODULE_PATH can have multiple paths delimited by ":".
 *
 * @param[in] module_name - the module file name.
 * @param[out] buf - pointer to a buffer to store the resolved module path.
 *                   The buffer should be allocated by the caller.
 * @param[in] buflen - the buffer length
 * @return - CTK_SUCCESS (0) on success, errno (non-zero) on failure.
 */
int ctk_spe_path_resolve(const char *module_name, char *buf, int buflen);

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SPE_PATH_H */
