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

#ifndef _CTK_SPU_HPP
#define _CTK_SPU_HPP

/**
 * @file Spu.hpp
 * @brief Definitions for CTK++ SPU interface
 */
#ifdef __SPU__

#include "ctk_spu_common.h"

namespace ctk {

/**
 * SPU class
 */
class Spu {
    public:
        /**
         * Initializes the CTK SPU environment.
         * <b>SPE programs should call this earlier than any 
         * other environment-dependent operations.</b>
         * @param[in] argp - the 2nd parameter of the main function
         * @param[in] envp - the 3rd parameter of the main function
         */
        static inline void
        init(unsigned long long &argv, unsigned long long &envp)
        {
            ::ctk_env_init(&envp);
        }

        /**
         * Returns the group ID.
         * @note #Spu::init must be called before calling this method.
         * @return - valid group ID if the SPE program is instantiated
         *  as a member of a SPE thread group, -1 otherwise.
         */
        static inline ctk_uint64_t getGroupId() {
            return ::ctk_env_get_gid();
        }

        /**
         * Returns the rank ID of the SPE program.
         * @note #Spu::init must be called before calling this method.
         * @return - rank ID of the SPE program if it is instantiated
         *  as a member of a SPE thread group, -1 otherwise.
         */
        static inline ctk_uint32_t getRank() {
            return ::ctk_env_get_rank();
        }

        /**
         * Sleeps (spin-lock-waits) for the given seconds.
         * @param[in] sec - sleep period
         * @note This reads/writes decrementer.
         */
        static inline void sleep(unsigned long sec) {
            ::ctk_sleep(sec);
        }

        /**
         * Sleeps (spin-lock-waits) for the given microseconds.
         * @param[in] usec - sleep period (in usec)
         * @note This reads/writes decrementer.
         */
        static inline void usleep(unsigned long sec) {
            ::ctk_usleep(sec);
        }
};

/** This SPU - the single SPU instance */
//extern Spu spu;
static Spu spu __attribute__((unused));

};

#endif /* __SPU__ */

#endif /* _CTK_SPU_HPP */
