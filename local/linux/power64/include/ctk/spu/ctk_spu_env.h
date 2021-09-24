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

#ifndef _CTK_SPU_ENV_H
#define _CTK_SPU_ENV_H

/**
 * @file ctk_spu_env.h
 * @brief Definitions for SPU environment values
 */
#include "../ctk_env.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined (CTK_SPU_RUNTIME)
/**
 * SPE environment structure
 */
#ifndef UNIT_TEST
extern ctk_env_t ctk_env;
#else /* UNIT_TEST */
static ctk_env_t ctk_env;
#endif /* UNIT_TEST */

/**
 * Initializes the SPU environment.  
 * <b>SPE programs should call this earlier than any 
 * other environment-dependent operations.</b>
 * @param[in] envp - the 3rd parameter of the main function
 */
static inline void ctk_env_init(unsigned long long *envpp) {
    unsigned long long envp = *envpp;
    if (envp == 0)
        return;
    ctk_dma_get_small_block(&ctk_env, envp, 128);
    if (ctk_env.env[CTK_ENV_ID_MAGIC].val32[0] == CTK_ENV_MAGIC) {
        ctk_timebase_clock = ctk_env.env[CTK_ENV_ID_TIMEBASE].val32[0];
        *envpp = ctk_env.env[CTK_ENV_ID_ENVP].ea_addr;
    }
}

/**
 * Gets the environment variable specified by the given id.
 * @note Should be called after #ctk_env_init().
 * @note The ID should be valid.
 * @param[in] id
 * @return the env value (of uint16_t) specified by the id
 */
static inline ctk_uint16_t ctk_env_get_int16_value(int id) __attribute__((always_inline));
static inline ctk_uint16_t ctk_env_get_int16_value(int id) {
    return ctk_env.env[id].val16[0];
}

/**
 * Gets the environment variable specified by the given id.
 * @note Should be called after #ctk_env_init().
 * @note The ID should be valid.
 * @param[in] id
 * @return the env value (of uint32_t) specified by the id
 */
static inline ctk_uint32_t ctk_env_get_int32_value(int id) __attribute__((always_inline));
static inline ctk_uint32_t ctk_env_get_int32_value(int id) {
    return ctk_env.env[id].val32[0];
}

/**
 * Gets the environment variable specified by the given id.
 * @note Should be called after #ctk_env_init().
 * @note The ID should be valid.
 * @param[in] id
 * @return the env value (of uint64_t) specified by the id
 */
static inline ctk_uint64_t ctk_env_get_int64_value(int id) __attribute__((always_inline));
static inline ctk_uint64_t ctk_env_get_int64_value(int id) {
    return ctk_env.env[id].val64;
}

/**
 * Gets the environment variable specified by the given id.
 * @note The ID should be valid.
 * @param[in] id
 * @return the env value (of ea_addr_t) specified by the id
 */
static inline ea_addr_t ctk_env_get_addr_value(int id) __attribute__((always_inline));
static inline ea_addr_t ctk_env_get_addr_value(int id) {
    return ctk_env.env[id].ea_addr;
}

/**
 * Returns the SPE thread group ID (returns a valid value
 * only if the SPE is created as a member of a SPE thread group)
 */
static inline ctk_uint64_t ctk_env_get_gid() {
    return ctk_env_get_int64_value(CTK_ENV_ID_GID);
}

/**
 * Returns the SPE thread 'rank' in the thread group (returns a valid value
 * only if the SPE is created as a member of a SPE thread group)
 */
static inline ctk_uint32_t ctk_env_get_rank() __attribute__((always_inline));
static inline ctk_uint32_t ctk_env_get_rank() {
    return ctk_env_get_int32_value(CTK_ENV_ID_RANK);
}
#endif /* !CTK_SPU_RUNTIME */

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SPU_ENV_H */
