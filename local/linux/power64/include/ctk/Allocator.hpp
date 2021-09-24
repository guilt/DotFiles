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

#ifndef _CTK_ALLOCATOR_HPP
#define _CTK_ALLOCATOR_HPP

/**
 * @file Allocator.hpp
 * @see #ctk_memalign, #ctk_malloc_align
 * @brief Definitions for CTK++ memory allocation interface
 */
#include "ctk_types.h"
#include "ctk_malloc.h"
#ifndef __SPU__
# include "Exception.hpp"
#endif

#include <limits>

namespace ctk {

/**
 * Aligned memory allocator (an alternative to std::allocator).
 * Usage:
 * <pre>
 *    std::vector<int, AlignedAllocator<int> > v(30);
 *    std::vector<Data, AlignedAllocator<Data, 16> > data(5);
 * </pre>
 */
template <typename T, int _ALIGN = 128, 
          int __aligned_T_size = ALIGN_UP(sizeof(T), _ALIGN)>
class AlignedAllocator {
    public:
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T value_type;

        template <class U> struct rebind {
            typedef AlignedAllocator<U> other;
        };

        // constructors
        AlignedAllocator() throw() { }
        AlignedAllocator(const AlignedAllocator&) throw() {}
        template <class U> AlignedAllocator(const AlignedAllocator<U>&)
            throw() {}

        // destructor
        ~AlignedAllocator() throw() { }

        // assignment
        template <class U>
	AlignedAllocator& operator=(AlignedAllocator<U> const& other){
		return *this;
	}

        /** Allocates aligned memory region
          * @note 3rd param expects a boolean value which indicates
          * if the caller want each allocated element to be aligned
          * or not (default = false).
          */
        pointer allocate(size_type num, const void *hint = 0, bool align_each = false) {
            if (__builtin_expect(align_each, 0))
                return (pointer)::ctk_malloc_align((num)*__aligned_T_size, _ALIGN);
            else
                return (pointer)::ctk_malloc_align(
                        ALIGN_UP((num)*sizeof(T), _ALIGN), _ALIGN);
        }

        /** Constructs the given pointer */
        void construct(pointer p, const T& value)
        {
            new((void*)p) T(value);
        }

        /** Deallotes the given region */
        void deallocate(pointer p, size_t num = 1)
        {
            if (p != NULL)
                ::ctk_free_align((void*)p);
        }

        /** Destroys the given pointer */
        void destroy(pointer p)
        {
            p->~T();
        }

        // Returns the address
        pointer address(reference value) const { return &value; }
        const_pointer address(const_reference value) const { return &value; }

        // Returns the maximum number of sizes it can allocate
        size_type max_size() const throw()
        {
            return std::numeric_limits<size_t>::max() / sizeof(T);
        }
};

};

#endif /* _CTK_ALLOCATOR_HPP */
