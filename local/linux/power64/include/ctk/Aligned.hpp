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

#ifndef _CTK_ALIGNED_HPP
#define _CTK_ALIGNED_HPP

/**
 * @file Aligned.hpp
 * @brief Definitions for CTK++ interface for aligned memory region
 */
#include "ctk_types.h"
#include "Allocator.hpp"
#include <vector>

#include <new>

namespace ctk {

/* forward declaration */
template <typename T, int _ALIGN, typename _ALLOC>
class AlignedArray;

/**
 * Aligned pointer template -<br />
 * <p>
 * The constructor allocates an aligned buffer for the given
 * type. By default the memory is allocated on the stack or 
 * at the static data region, but the caller can change
 * the allocator by giving other allocators as the ALLOC
 * template parameter.</p>
 *
 * <p>
 * For primitive types, an instance of the template can be
 * treated as a normal instance of the given type.<br />
 * For array types (like structures or classes), each field
 * can be accessed via '[]' operator (but probably you should
 * consider using Aligned<T>::vector instead).<br />
 * For complex types (like structures or classes), their members
 * can be accessed via '->' operator.</p>
 *
 * Sample Usages:
 * <pre>
 *   Aligned<int, 16> flag;          // 'flag' is 16-byte aligned
 *   Aligned<arg_type_t> arg;        // 'arg' is 128-byte aligned
 *   Aligned<char[100]> global;      // 'global' is 128-byte aligned
 *
 *   Aligned<char>::vector data(10); // 'data' vector is 128-byte aligned
 *   Aligned<char>::array array(10);  // Each 'data' element is 128-byte aligned
 *                                    // (see #AlignedArray also)
 *
 *   flag = 0;
 *
 *   arg->flagAddr = &flag;
 *   arg->dataAddr = global.addr();
 *   arg->dataSize = global.size();
 *
 *   for (int i = 0; i < 100; i++)
 *       global[i] = i * 2;
 *    
 *   SpeContext spe;
 *   spe.load(program);
 *   spe.run(&arg);
 * </pre>
 */
template <typename T, int _ALIGN = 128, 
        typename _ALLOC = AlignedAllocator<T, _ALIGN>, 
        int _SIZE = ALIGN_UP(sizeof(T), _ALIGN),      // hidden parameter
        bool _inlineVar = (sizeof(T) < _ALIGN) >      // hidden parameter
class Aligned {
    public:
        typedef _ALLOC allocator_type;
        typedef std::vector<T, _ALLOC> vector;
        typedef AlignedArray<T, _ALIGN, _ALLOC> array;
        enum { size = _SIZE };

    private:
        T *m_data;
        allocator_type m_allocator;

        inline void setStorage(T *storage) {
            m_data = new ((void*)storage) T;
        }
        inline void copyData(T* const& data) {
            *m_data = *data;
        }

    public:
        // default constructor
        Aligned()
        {
            setStorage(m_allocator.allocate(1));
        }
        // copy constructor
        Aligned(const Aligned<T, _ALIGN, _ALLOC> &a)
        {
            setStorage(m_allocator.allocate(1));
            copyData(a.m_data);
        }
        virtual ~Aligned() {
            m_data->~T();
            m_allocator.deallocate(m_data, 1);
        }

        // Assignment operator
        Aligned& operator=(const Aligned<T, _ALIGN, _ALLOC> &a) {
            if (m_data == a.m_data)
                return *this;
            setStorage(m_allocator.allocate(1));
            copyData(a.m_data);
        }
        Aligned& operator=(const T &t) { *m_data = t; return *this; }

        void *ptr() const { return (void*)m_data; }
#ifdef __SPU__
        ls_addr_t addr() const { return (ls_addr_t)(unsigned long)ptr(); }
#else
        ea_addr_t addr() const { return (ea_addr_t)(unsigned long)ptr(); }
#endif
        const void *operator &() const { return ptr(); }
        T* operator->() const { return m_data; }
        operator T& () { return *m_data; }
        operator void *() { return ptr(); }
};

/* specialized one */
template <typename T, int _ALIGN, typename _ALLOC, int _SIZE>
class Aligned <T, _ALIGN, _ALLOC, _SIZE, true> {
    public:
        typedef _ALLOC allocator_type;
        typedef std::vector<T, _ALLOC> vector;
        typedef AlignedArray<T, _ALIGN, _ALLOC> array;
        enum { size = _SIZE };

    private:
        char m_buf[_ALIGN * 2];
        T *m_data;
        allocator_type m_allocator;

        inline void setStorage() {
            T *storage = reinterpret_cast<T*>(ALIGN_UP_PTR(m_buf, _ALIGN));
            m_data = new ((void*)storage) T;
        }
        inline void copyData(T* const& data) {
            *m_data = *data;
        }

    public:
        // default constructor
        Aligned() 
        {
            setStorage();
        }
        // copy constructor
        Aligned(const Aligned<T, _ALIGN, _ALLOC> &a)
        {
            setStorage();
            copyData(a.m_data);
        }
        virtual ~Aligned()
        {
            m_data->~T();
        }

        // Assignment operator
        Aligned& operator=(const Aligned<T, _ALIGN, _ALLOC> &a) {
            if (m_data == a.m_data)
                return *this;
            setStorage();
            copyData(a.m_data);
        }
        Aligned& operator=(const T &t) { *m_data = t; return *this; }

        void *ptr() const { return (void*)m_data; }
#ifdef __SPU__
        ls_addr_t addr() const { return (ls_addr_t)(unsigned long)ptr(); }
#else
        ea_addr_t addr() const { return (ea_addr_t)(unsigned long)ptr(); }
#endif
        const void *operator &() const { return ptr(); }
        T* operator->() const { return m_data; }
        operator T& () { return *m_data; }
        operator void *() { return ptr(); }
};

/* specialized one (array) */
template <typename T, int _NUNITS, int _ALIGN, typename _ALLOC, int _SIZE>
class Aligned <T[_NUNITS], _ALIGN, _ALLOC, _SIZE, false> {
    public:
        typedef _ALLOC allocator_type;
        typedef typename _ALLOC::pointer pointer;
        typedef AlignedArray<T[_NUNITS], _ALIGN, _ALLOC> array;
        enum { size = _SIZE };

    private:
        T *m_data;
        allocator_type m_allocator;

        inline void setStorage(pointer storage) {
            m_data = new ((void*)storage) T[_NUNITS];
        }
        inline void copyData(T* const& data) {
            for (int i = 0; i < _NUNITS; i++) 
                m_data[i] = data[i];
        }

    public:
        // default constructor
        Aligned()
        {
            setStorage(m_allocator.allocate(1));
        }
        // copy constructor
        Aligned(const Aligned<T[_NUNITS], _ALIGN, _ALLOC> &a)
        {
            setStorage(m_allocator.allocate(1));
            copyData(a.m_data);
        }
        virtual ~Aligned() {
            //DEBUG_PRINTF("Aligned: free %p\n", (void*)m_data);
            m_data->~T();
            m_allocator.deallocate((pointer)m_data, 1);
        }

        // Assignment operator
        Aligned& operator=(const Aligned<T, _ALIGN, _ALLOC> &a) {
            if (m_data == a.m_data)
                return *this;
            setStorage(_ALLOC::allocate(1));
            copyData(a.m_data);
        }

        void *ptr() const { return (void*)m_data; }
#ifdef __SPU__
        ls_addr_t addr() const { return (ls_addr_t)(unsigned long)ptr(); }
        operator ls_addr_t() const { return addr(); }
#else
        ea_addr_t addr() const { return (ea_addr_t)(unsigned long)ptr(); }
        operator ea_addr_t() const { return addr(); }
#endif
        operator T* () const { return m_data; }
        operator void* const() const { return ptr(); }
        T& operator [](int index) { return m_data[index]; }
        const T& operator [](int index) const { return m_data[index]; }
};

/* specialized one (inline array) */
template <typename T, int _NUNITS, int _ALIGN, typename _ALLOC, int _SIZE>
class Aligned <T[_NUNITS], _ALIGN, _ALLOC, _SIZE, true> {
    public:
        typedef AlignedArray<T[_NUNITS], _ALIGN, _ALLOC> array;
        enum { size = _SIZE };

    private:
        char m_buf[_ALIGN * 2];
        T *m_data;

        inline void setStorage() {
            void *storage = ALIGN_UP_PTR(m_buf, _ALIGN);
            m_data = new ((void*)storage) T[_NUNITS];
        }
        inline void copyData(T* const& data) {
            for (int i = 0; i < _NUNITS; i++) 
                m_data[i] = data[i];
        }

    public:
        // default constructor
        Aligned()
        {
            setStorage();
        }
        // copy constructor
        Aligned(const Aligned<T[_NUNITS], _ALIGN, _ALLOC> &a)
        {
            setStorage();
            copyData(a.m_data);
        }
        virtual ~Aligned() {

            m_data->~T();
        }

        // Assignment operator
        Aligned& operator=(const Aligned<T, _ALIGN, _ALLOC> &a) {
            if (m_data == a.m_data)
                return *this;
            setStorage();
            copyData(a.m_data);
        }

        void *ptr() const { return (void*)m_data; }
#ifdef __SPU__
        ls_addr_t addr() const { return (ls_addr_t)(unsigned long)ptr(); }
        operator ls_addr_t() const { return addr(); }
#else
        ea_addr_t addr() const { return (ea_addr_t)(unsigned long)ptr(); }
        operator ea_addr_t() const { return addr(); }
#endif
        operator T* () const { return m_data; }
        operator void* const() const { return ptr(); }
        T& operator [](int index) { return m_data[index]; }
        const T& operator [](int index) const { return m_data[index]; }
};

}; // namespace ctk

#endif /* _CTK_ALIGNED_HPP */
