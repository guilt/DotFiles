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

#ifndef _CTK_SPINLOCK_HPP
#define _CTK_SPINLOCK_HPP

#ifndef __SPU__
# include "ctk_spinlock.h"
# include "ctk_mutex.h"
# include "Exception.hpp"
#else
# if defined(UNIT_TEST) || defined(_CTKLIB)
#  include "ctk_spu_mutex.h"
# else
#  include "spu/ctk_spu_mutex.h"
# endif
#endif

/**
 * @file Spinlock.hpp
 * @brief Definitions for CTK++ Spinlock interface
 */
namespace ctk {

/**
 * Spinlock class
 */
class Spinlock : public EAObject {
#ifndef __SPU__
        ctk_spinlock_ea_t m_lock;

    public:
        Spinlock() : m_lock(0) {
            int ret = _ctk_mutex_create(&m_lock);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            alloc();
        }

        /** Copy constructor */
        Spinlock(const Spinlock &lock) : m_lock(lock.m_lock) {
            copy(&lock);
        }

        /** Assignment operator */
        Spinlock &operator=(const Spinlock &lock) {
            assign(&lock);
            m_lock = lock.m_lock;
            return *this;
        }

        virtual ~Spinlock() { release(); }

        /* implement */
        virtual void destroy() {
            if (m_lock != 0)
                _ctk_mutex_destroy(m_lock);
        }

        void init() {
            _ctk_spinlock_init(m_lock);
        }

        void lock() {
            _ctk_spin_lock(m_lock);
        }

        void unlock() {
            _ctk_spin_unlock(m_lock);
        }

        int trylock() {
            return _ctk_spin_trylock(m_lock);
        }
#else /* __SPU__ */
        ea_addr_t m_lock;

    public:
        Spinlock(ea_addr_t ea) : m_lock(ea) { }

        void init() {
            _ctk_mutex_init(m_lock);
        }

        void lock() {
            _ctk_mutex_lock(m_lock);
        }

        void unlock() {
            _ctk_mutex_unlock(m_lock);
        }

        int trylock() {
            return _ctk_mutex_trylock(m_lock);
        }
#endif /* __SPU__ */

        virtual operator void*() {
            return (void*)(unsigned long)m_lock;
        }

        virtual operator ea_addr_t () {
            return m_lock;
        }
};

}; // namespace ctk

#endif /* _CTK_SPINLOCK_HPP */
