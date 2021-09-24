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

#ifndef _CTK_SEMAPHORE_HPP
#define _CTK_SEMAPHORE_HPP

#ifndef __SPU__
# include "ctk_sem.h"
# include "Exception.hpp"
#else /* __SPU__ */
# if defined(UNIT_TEST) || defined(_CTKLIB)
#  include "ctk_spu_sem.h"
# else
#  include "spu/ctk_spu_sem.h"
# endif
#endif /* __SPU__ */

/**
 * @file Semaphore.hpp
 * @brief Definitions for CTK++ Semaphore interface
 */
namespace ctk {

/**
 * Semaphore class
 */
class Semaphore : public EAObject {
        ctk_sem_ea_t m_sem;

    public:
#ifndef __SPU__
        Semaphore(unsigned int value) : m_sem(0) {
            int ret = _ctk_sem_create(&m_sem, value);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            alloc();
        }

        /** Copy constructor */
        Semaphore(const Semaphore &sem) : m_sem(sem.m_sem) {
            copy(&sem);
        }

        /** Assignment operator */
        Semaphore &operator=(const Semaphore &sem) {
            assign(&sem);
            m_sem = sem.m_sem;
            return *this;
        }

        virtual ~Semaphore() { release(); }

        /* implement */
        virtual void destroy() {
            if (m_sem != 0)
                _ctk_sem_destroy(m_sem);
        }
#else /* __SPU__ */
        Semaphore(ea_addr_t sem_ea) : m_sem(sem_ea) { }
#endif /* __SPU__ */

        void init(unsigned int value) {
            _ctk_sem_init(m_sem, value);
        }

        void post() {
            _ctk_sem_post(m_sem);
        }

        void wait() {
            _ctk_sem_wait(m_sem);
        }

        int trywait() {
            return _ctk_sem_trywait(m_sem);
        }

        int value() {
            return _ctk_sem_get_value(m_sem);
        }

        operator int () {
            return _ctk_sem_get_value(m_sem);
        }

        virtual operator void*() {
            return (void*)(unsigned long)m_sem;
        }

        virtual operator ea_addr_t () {
            return m_sem;
        }
};

}; // namespace ctk

#endif /* _CTK_SEMAPHORE_HPP */
