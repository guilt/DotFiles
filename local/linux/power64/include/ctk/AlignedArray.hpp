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

#ifndef _CTK_ALIGNED_ARRAY_HPP
#define _CTK_ALIGNED_ARRAY_HPP

/**
 * @file AlignedArray.hpp
 * @brief Definitions for CTK++ interface for aligned memory region
 */
#include "ctk_types.h"
#include <vector>
#include "Allocator.hpp"

#include <new>

namespace ctk {

/**
 * Aligned array template -<br />
 * <p>
 * The constructor allocates an aligned buffer for the given
 * type. By default the memory is allocated in the heap.
 * The caller can change the allocator by giving other
 * allocators as the _ALLOC template parameter.
 * </p>
 *
 * <p>
 * The array can contain arbitrary number of elements of the given type.
 * Each element is aligned and is placed in a contiguous memory region
 * so that each element can easily be accessed by addrress.
 * Each element can be accessed via '[]' operator.
 * Also you can access each by using its iterator classs.
 * The number of elements can be obtained by #size() method.
 * </p>
 *
 * @note The number of elements CANNOT be changed and adding or removing 
 * new/old elemnts is currently not supported.
 *
 * @note AlignedArray<T, ALIGN> is equivalent to Aligned<T, ALIGN>::array.
 *
 * Sample Usages:
 * <pre>
 *   AlignedArray<int, 16> flag(5);    // allocates 5 aligned 'flag' region.
 *                                     // Each one is 16-byte aligned.
 *   AlignedArray<arg_data_t> arg(5);  // allocates 5 aligned 'arg' region.
 *   AlignedArray<char[80]> data(5);   // 'data[0 to 5]' is aligned
 *                                     // (and each consists of 80 chars)
 *   // loops 5 times
 *   for (size_t i = 0; i < flag.size(); i++) {
 *       flag[i] = 0;
 *       for (int j = 0; j < 80; j++) 
 *           data[i][j] = j;
 *       arg[i].id = i;
 *   }
 *
 *   // loops 5 times using iterator
 *   AlignedArray<arg_data_t>::iterator i;
 *   AlignedArray<char[80]>::iterator j = data.begin();
 *   for (i = arg.begin(); i != arg.end(); ++i) {
 *       i->flagsAddr = flag;
 *       i->dataAddr = data[j++];
 *       i->dataSize = Aligned<char[80]>::size;
 *       i->numberOfTasks = arg.size();
 *   }
 *    
 *   SpeContext spe[5];
 *   for (int i = 0; i < 5; i++) {
 *       spe[i].load(program);
 *       spe[i].run(&arg[i]);
 *   }
 * </pre>
 */
template <typename T, int _ALIGN = 128, typename _ALLOC = AlignedAllocator<T, _ALIGN> >
class AlignedArray {
    public:
        typedef _ALLOC allocator_type;
        enum { element_size = Aligned<T, _ALIGN>::size };

    private:
        T *m_storage;
        std::vector<T*> m_data;
        allocator_type m_allocator;

        inline void setStorage(T *storage) {
            m_storage = storage;
            char *tmp = (char*)m_storage;
            for (size_t i = 0; i < m_data.size(); i++) {
                m_data[i] = new ((void*)tmp) T;
                tmp += element_size;
            }
        }
        inline void copyData(std::vector<T*> data) {
            for (size_t i = 0; i < m_data.size(); i++)
                *m_data[i] = *data[i];
        }

    public:
        AlignedArray(int nelements = 1)
            : m_data(nelements)
        {
            setStorage(m_allocator.allocate(nelements, 0, true));
        }
        // copy constructor
        AlignedArray(const AlignedArray<T, _ALIGN, _ALLOC> &a)
            : m_data(a.m_data.size())
        {
            setStorage(m_allocator.allocate(m_data.size(), 0, true));
            copyData(a.m_data);
        }
        virtual ~AlignedArray() {
            if (m_storage != NULL) {
                for (size_t i = 0; i < m_data.size(); i++)
                    m_data[i].~T();
                m_allocator.deallocate(m_storage, m_data.size());
                m_storage = NULL;
            }
        }

        // assignment
        AlignedArray& operator=(const AlignedArray<T, _ALIGN, _ALLOC> &a)
        {
            if (m_storage == a.m_storage)
                return *this;
            for (size_t i = 0; i < m_data.size(); i++)
                m_data[i].~T();
            if (m_data.size() < a.m_data.size()) {
                m_allocator.deallocate(m_storage, m_data.size());
                setStorage(m_allocator.allocate(a.m_data.size(), 0, true));
            }
            m_data.resize(a.m_data.size());
            copyData(a.m_data);
        }

        void resize(size_t size) {
            T *storage = m_allocator.allocate(size, 0, true);
            char *tmp = (char*)storage;
            size_t old_size = m_data.size();
            m_data.resize(size);
            for (size_t i = 0; i < size; i++) {
                if (i < old_size) {
                    *(T*)tmp = *m_data[i];
                    m_data[i] = (T*)tmp;
                } else {
                    m_data[i] = new ((void*)tmp) T;
                }
                tmp += element_size;
            }
            m_allocator.deallocate(m_storage, m_data.size());
            m_storage = storage;
        }

        void *ptr() const { return (void*)m_data[0]; }
        ea_addr_t addr() const { return (ea_addr_t)(unsigned long)ptr(); }
        size_t size() { return m_data.size(); }
        operator T* () { return m_data[0]; }
        operator void* const() const { return ptr(); }
        operator ea_addr_t() const { return addr(); }
        T& operator [](int index) { return *m_data[index]; }
        const T& operator [](int index) const { return *m_data[index]; } 

        // Forward iterator
        class iterator {
            public:
                T *m_cursor;
                explicit iterator(T* p = 0) : m_cursor(p) { }
                iterator& operator++() {
                    m_cursor = (T*)((char*)m_cursor + element_size);
                    return *this;
                }
                iterator operator++(int) {
                    T* old = m_cursor;
                    m_cursor = (T*)((char*)m_cursor + element_size);
                    return iterator(old);
                }
                T& operator*() { return *m_cursor; }
                T* operator->() { return m_cursor; }
                friend bool operator==(const iterator& x, const iterator& y) {
                    return x.m_cursor == y.m_cursor;
                }
                friend bool operator!=(const iterator& x, const iterator& y) {
                    return x.m_cursor != y.m_cursor;
                }
        };

        iterator begin() { return iterator(m_data[0]); }
        iterator end() { return iterator((T*)
            ((char*)m_data[m_data.size() - 1] + element_size)); }
};

/* specialized one */
template <typename T, int _NUNITS, int _ALIGN, typename _ALLOC>
class AlignedArray <T[_NUNITS], _ALIGN, _ALLOC> {
    public:
        typedef _ALLOC allocator_type;
        typedef typename _ALLOC::pointer pointer;
        enum { element_size = Aligned<T[_NUNITS], _ALIGN>::size };

    private:
        pointer m_storage;
        std::vector<T*> m_data;
        allocator_type m_allocator;

        inline void setStorage(pointer storage) {
            m_storage = storage;
            char *tmp = (char*)storage;
            for (size_t i = 0; i < m_data.size(); i++) {
                m_data[i] = new ((void*)tmp) T[_NUNITS];
                tmp += element_size;
            }
        }
        inline void copyData(std::vector<T*> data) {
            for (size_t i = 0; i < m_data.size(); i++) {
                for (int j = 0; j < _NUNITS; j++) 
                    m_data[i][j] = data[i][j];
            }
        }

    public:
        AlignedArray() : m_storage(NULL) { }
        AlignedArray(int nelements) : m_data(nelements)
        {
            setStorage(m_allocator.allocate(m_data.size(), 0, true));
        }
        // copy constructor
        AlignedArray(const AlignedArray<T[_NUNITS], _ALIGN, _ALLOC> &a)
            : m_data(a.m_data.size())
        {
            setStorage(m_allocator.allocate(a.m_data.size(), 0, true));
            copyData(a.m_data);
        }
        virtual ~AlignedArray() {
            //DEBUG_PRINTF("AlignedArray: free %p\n", (void*)m_data);
            for (size_t i = 0; i < m_data.size(); i++)
                m_data[i].~T();
            if (m_storage != NULL) {
                m_allocator.deallocate(m_storage, m_data.size());
                m_storage = NULL;
            }
        }

        // assignment
        AlignedArray& operator=(const AlignedArray<T, _ALIGN, _ALLOC> &a)
        {
            if (m_storage == a.m_storage)
                return *this;
            for (size_t i = 0; i < m_data.size(); i++)
                m_data[i].~T();
            if (m_data.size() < a.m_data.size()) {
                m_allocator.deallocate(m_storage, m_data.size());
                setStorage(m_allocator.allocate(a.m_data.size(), 0, true));
            }
            m_data.resize(a.m_data.size());
            copyData(a.m_data);
        }

        void resize(size_t size) {
            pointer storage = m_allocator.allocate(size, 0, true);
            char *tmp = (char*)storage;
            size_t old_size = m_data.size();
            m_data.resize(size);
            for (size_t i = 0; i < size; i++) {
                if (i < old_size) {
                    T *d = (T*)((char*)m_storage + element_size * i);
                    for (int j = 0; j < _NUNITS; j++) {
                        m_data[i][j] = d[j];
                    }
                } else {
                    m_data[i] = new ((void*)tmp) T[_NUNITS];
                }
                tmp += element_size;
            }
            m_allocator.deallocate(m_storage, m_data.size());
            m_storage = storage;
        }

        void *ptr() const { return (void*)m_data[0]; }
        ea_addr_t addr() const { return (ea_addr_t)(unsigned long)ptr(); }
        size_t size() { return m_data.size(); }
        operator T* () { return m_data[0]; }
        operator void* const() const { return ptr(); }
        operator ea_addr_t() const { return addr(); }
        T* operator [](int index) { return m_data[index]; }
        const T* operator [](int index) const { return m_data[index]; }

        typedef typename std::vector<T*>::iterator iterator;
        iterator begin() { return m_data.begin(); }
        iterator end() { return m_data.end(); }
};

}; // namespace ctk

#endif /* _CTK_ALIGNED_ARRAY_HPP */
