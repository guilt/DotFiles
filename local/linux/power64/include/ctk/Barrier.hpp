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

#ifndef _CTK_BARRIER_HPP
#define _CTK_BARRIER_HPP

#ifndef __SPU__
# include "ctk_barrier.h"
# include "Exception.hpp"
#else /* __SPU__ */
# if defined(UNIT_TEST) || defined(_CTKLIB)
#  include "ctk_spu_barrier.h"
# else
#  include "spu/ctk_spu_barrier.h"
# endif
#endif /* __SPU__ */

/**
 * @file Barrier.hpp
 * @brief Definitions for CTK++ Barrier interface
 */
namespace ctk {

/**
 * Barrier class
 */
class Barrier : public EAObject {
        ctk_barrier_ea_t m_barrier;

    public:
#ifndef __SPU__
        Barrier(unsigned int count) : m_barrier(0) {
            int ret = _ctk_barrier_create(&m_barrier, count);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            alloc();
        }

        /** Copy constructor */
        Barrier(const Barrier &barrier) : m_barrier(barrier.m_barrier) {
            copy(&barrier);
        }

        /** Assignment operator */
        Barrier &operator=(const Barrier &barrier) {
            assign(&barrier);
            m_barrier = barrier.m_barrier;
            return *this;
        }

        virtual ~Barrier() { release(); }

        /* implement */
        virtual void destroy() {
            if (m_barrier != 0)
                _ctk_barrier_destroy(m_barrier);
        }
#else /* __SPU__ */
        Barrier(ea_addr_t ea) : m_barrier(ea) { }
#endif /* __SPU__ */

        void init(unsigned int count) {
            _ctk_barrier_init(m_barrier, count);
        }

        void wait() {
            _ctk_barrier_wait(m_barrier);
        }

        virtual operator void*() {
            return (void*)(unsigned long)m_barrier;
        }

        virtual operator ea_addr_t () {
            return m_barrier;
        }
};

}; // namespace ctk

#endif /* _CTK_BARRIER_HPP */
