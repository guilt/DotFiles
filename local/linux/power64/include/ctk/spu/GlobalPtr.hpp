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

#ifndef _CTK_SPU_GLOBAL_PTR_HPP
#define _CTK_SPU_GLOBAL_PTR_HPP

#include "ctk_spu_common.h"
#include "../Allocator.hpp"
#include "../StaticAssert.hpp"

#include <new>
#include <stdexcept>

#include "GlobalPtr.tpp" 

namespace ctk {

/**
 * Global Pointer template 
 * (The caller can set appropriate SYNC policy template to customize
 *  the behavior; the available templates are: RWSync, RSync, WSync, NoSync)
 */
template <typename T, int SIZE = sizeof(T), typename SYNC = RWSync<T, SIZE> >
class GlobalPtr {
    private:
        _GlobalPtrValue<T, SIZE, SYNC> m_value;
        _GlobalPtrValue<T, SIZE, SYNC> m_cache; /* xxx */
        int m_index;

    public:
        GlobalPtr(ea_addr_t addr) : m_value(addr), m_cache(addr), m_index(0) { }
        GlobalPtr(GlobalPtr<T, SIZE> &t)
            : m_value(t.m_value.m_ea), m_cache(t.m_value.m_ea), m_index(0) { }
        GlobalPtr(const GlobalPtr<T, SIZE> &t)
            : m_value(t.m_value.m_ea), m_cache(t.m_value.m_ea), m_index(0) { }
        /* assignment overload */
        GlobalPtr& operator=(GlobalPtr<T, SIZE> &t) {
            m_value.m_ea = t.m_value.m_ea;
            return *this;
        }
        /* assignment overload */
        GlobalPtr& operator=(const GlobalPtr<T, SIZE> &t) {
            m_value.reset(t.m_value.m_ea);
            return *this;
        }
        /* assignment overload */
        GlobalPtr& operator=(_GlobalPtrWrapper<T, SIZE, SYNC> wrapper) {
            m_value.reset(wrapper.m_body.m_ea);
            return *this;
        }
        /* pointer access */
        _GlobalPtrValue<T, SIZE, SYNC>& operator *() { return m_value; }
        T* operator->() { return m_value; }
        operator T*() { return m_value; }
        /* index access */
        _GlobalPtrValue<T, SIZE, SYNC>& operator [](int index) {
            if (m_index == index) {
                return m_cache;
            }
            m_index = index;
            m_cache.reset(m_value.m_ea + index * SIZE);
            return m_cache;
        }
        GlobalPtr<T, SIZE, SYNC> &operator++() {
            m_value.reset(m_value.m_ea + SIZE);
            return *this;
        }
        GlobalPtr<T, SIZE, SYNC> operator++(int) {
            GlobalPtr<T, SIZE, SYNC> p(*this);
            m_value.reset(m_value.m_ea + SIZE);
            return p;
        }
        GlobalPtr<T, SIZE, SYNC> &operator--() {
            m_value.reset(m_value.m_ea - SIZE);
            return *this;
        }
        GlobalPtr<T, SIZE, SYNC> operator--(int) {
            GlobalPtr<T, SIZE, SYNC> p(*this);
            m_value.reset(m_value.m_ea - SIZE);
            return p;
        }
        GlobalPtr<T, SIZE, SYNC> operator+=(int i) {
            m_value.reset(m_value.m_ea + i * SIZE);
            return *this;
        }
        GlobalPtr<T, SIZE, SYNC> operator-=(int i) {
            m_value.reset(m_value.m_ea - i * SIZE);
            return *this;
        }
        GlobalPtr<T, SIZE, SYNC> operator+(int i) {
            GlobalPtr<T, SIZE> p(*this);
            p.m_value.m_ea += i * SIZE;
            return p;
        }
        GlobalPtr<T, SIZE, SYNC> operator-(int i) {
            GlobalPtr<T, SIZE, SYNC> p(*this);
            p.m_value.m_ea -= i * SIZE;
            return p;
        }
        /* public methods for explicit DMA put/get */
        void load() { m_value.load(); }
        void flush() { m_value.flush(); }
};

template <typename T>
inline std::ostream& operator<<(std::ostream& lhs, GlobalPtr<T>* rhs)
{
    lhs << (T)*rhs;
    return lhs;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& lhs, const GlobalPtr<T>* rhs)
{
    lhs << (T)*rhs;
    return lhs;
}

}; // namespace ctk

#endif /* _CTK_SPU_GLOBAL_PTR_HPP */
