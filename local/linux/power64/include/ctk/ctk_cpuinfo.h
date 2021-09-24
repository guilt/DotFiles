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

#ifndef _CTK_CPUINFO_H
#define _CTK_CPUINFO_H

/**
 * @file ctk_cpuinfo.h
 * @brief Utilities for read cpuinfo
 */

#ifndef CPUINFO_PATH
# define CPUINFO_PATH "/proc/cpuinfo"
#endif

/* CPU info keys */
#define CTK_CPUINFO_TIMEBASE    "timebase" /**< CPU info key for timebase */
#define CTK_CPUINFO_CLOCK       "clock"    /**< CPU info key for CPU clock */
#define CTK_CPUINFO_MACHINE     "machine"  /**< CPU info key for machine */
#define CTK_CPUINFO_REVISION    "revision" /**< CPU info key for revision */

/**
 * Initializes the CPU info internal hash
 * @return 0 (CTK_SUCCESS) on success, non-zero otherwise
 */
int ctk_cpuinfo_init();

/**
 * Destroy the CPU info internal hash
 */
void ctk_cpuinfo_destroy();

/**
 * Returns the CPU info for the given key
 * @param[in] key - key string (e.g. "timebase")
 * @param[in] default_value - a default value for the key
 * @return the value (of string) or default_value if not found.
 */
const char *ctk_cpuinfo_get_value(char *key, const char *default_value);

/**
 * Returns the CPU info for the given key
 * @param[in] key - key string (e.g. "timebase")
 * @param[in] default_value - a default value for the key
 * @return the value (of integer) or #default_value if not found.
 */
int ctk_cpuinfo_get_int_value(char *key, int default_value);

#endif /* _CTK_CPUINFO_H */
