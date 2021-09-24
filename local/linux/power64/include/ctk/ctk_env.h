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

#ifndef _CTK_ENV_H
#define _CTK_ENV_H

/**
 * @file ctk_env.h
 * @brief Definitions for CTK SPE Environment structures / routines
 */
#include "ctk_types.h"

#define CTK_ENV_MAGIC   0xce11cafe

/* system environments */
#define CTK_ENV_ID_MAGIC    0   /**< CTK env magic (uint32) */
#define CTK_ENV_ID_VERSION  1   /**< CTK version (float) */
#define CTK_ENV_ID_TIMEBASE 2   /**< timebase (uint32) */
#define CTK_ENV_ID_GID      3   /**< group ID (uint32) */
#define CTK_ENV_ID_RANK     4   /**< thread rank (uint32) */
#define CTK_ENV_ID_ENVP     5   /**< application envp (uint64) */
#define CTK_ENV_ID_RSV1     6   /**< reserved */
#define CTK_ENV_ID_TASKINFO 6   /**< runtime task info addr (for tasks) */

#define CTK_ENV_ID_BASE     7   /**< ID base for user apps */
#define CTK_ENV_ID_MAX     15  /**< Maximum ID */

#define CTK_ENV_MAX      16  /**< Maximum number of env values */

/**
 * A union to hold an SPE environment value (double word)
 */
typedef ctk_val64_t ctk_env_value_t;

/**
 * SPE Environment structure (128 bytes)
 */
typedef struct {
    ctk_env_value_t env[CTK_ENV_MAX] __attribute__((aligned(128)));
} ctk_env_t __attribute__((aligned(128)));

#endif /* _CTK_ENV_H */
