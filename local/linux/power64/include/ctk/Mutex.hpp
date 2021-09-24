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

#ifndef _CTK_MUTEX_HPP
#define _CTK_MUTEX_HPP

#ifndef __SPU__
# include "ctk_mutex.h"
# include "Exception.hpp"
#else /* __SPU__ */
# if defined(UNIT_TEST) || defined(_CTKLIB)
#  include "ctk_spu_mutex.h"
# else
#  include "spu/ctk_spu_mutex.h"
# endif
#endif /* __SPU__ */

/**
 * @file Mutex.hpp
 * @brief Definitions for CTK++ Mutex interface
 */
namespace ctk {

/**
 * Mutex class
 */
class Mutex : public EAObject {
        ctk_mutex_ea_t m_lock;

    public:
#ifndef __SPU__
        /** Constructor */
        Mutex() : m_lock(0) {
            int ret = _ctk_mutex_create(&m_lock);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            alloc();
        }

        /** Copy constructor */
        Mutex(const Mutex &mutex) : m_lock(mutex.m_lock) {
            copy(&mutex);
        }

        /** Assignment operator */
        Mutex &operator=(const Mutex &mutex) {
            assign(&mutex);
            m_lock = mutex.m_lock;
            return *this;
        }

        virtual ~Mutex() {
            release();
        }

        /* implement */
        virtual void destroy() {
            if (m_lock != 0)
                _ctk_mutex_destroy(m_lock);
        }
#else /* __SPU__ */
        Mutex(ea_addr_t lock_ea) : m_lock(lock_ea) { }
#endif /* __SPU__ */

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

        virtual operator void*() {
            return (void*)(unsigned long)m_lock;
        }

        virtual operator ea_addr_t () {
            return m_lock;
        }
};

/**
 * Simple version of scoped lock
 * <pre>
 * Usage:
 *
 * Mutex mutex;
 * {
 *     ScopedMutexLock lock(mutex);
 *     // critical code
 * }
 * </pre>
 */
template <typename L> class ScopedLock {
    private:
        L& m_lock;

    public:
        ScopedLock(L& mutex) : m_lock(mutex) { m_lock.lock(); }
        ~ScopedLock() { m_lock.unlock(); }

    private:
        ScopedLock();   // no impl.
};

typedef ScopedLock<Mutex> ScopedMutexLock;

}; // namespace ctk

#endif /* _CTK_MUTEX_HPP */
