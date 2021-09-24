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

#ifndef _CTK_PROFILE_HPP
#define _CTK_PROFILE_HPP

#ifndef __SPU__
# include "ctk_common.h"
# include "ctk_profile.h"
#else /* __SPU__ */
# if defined(UNIT_TEST) || defined(_CTKLIB)
#  include "ctk_spu_common.h"
#  include "ctk_spu_profile.h"
# else
#  include "spu/ctk_spu_common.h"
#  include "spu/ctk_spu_profile.h"
# endif
#endif /* __SPU__ */

/**
 * @file Profile.hpp
 * @brief Definitions of SPE CTK++ profile class
 */

namespace ctk {
/**
 * Profile class (using timebase / decrementer)
 * @see #ctk_profile_t
 */
class Profile {
        ctk_profile_t m_prof;
    public:
        /** starts the counter */
        void start() { ::ctk_profile_start(&m_prof); }
        /** reads the current counter value */
        unsigned int read() {
            return ::ctk_profile_read_in_clock(&m_prof);
        }
        /** reads the current counter value in msec */
        unsigned int readInMsec() {
            return ::ctk_profile_read_in_msec(&m_prof);
        }
        /** reads the current counter value in usec */
        unsigned int readInUsec() {
            return ::ctk_profile_read_in_usec(&m_prof);
        }
};

}; // namespace ctk

#endif /* _CTK_PROFILE_HPP */
