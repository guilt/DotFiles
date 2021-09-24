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

#ifndef _CTK_ALIGNED_BASE_HPP
#define _CTK_ALIGNED_BASE_HPP

#include "ctk_types.h"
#include "Allocator.hpp"

#include <new>

namespace ctk {

/**
 * AlignedBase: an abstract class which forces 'new' to allocate an aligned
 * buffer and is supposed to be a super class of user data types. 
 */
template <int _ALIGN = 128, typename _ALLOC = AlignedAllocator<char, _ALIGN> >
class AlignedBase {
    public:
        static void* operator new(size_t size) {
            _ALLOC allocator;
            return (void*)allocator.allocate(size);
        }
        static void* operator new(size_t size, void* p) {
            return p;
        }
        static void operator delete(void *p) {
            _ALLOC allocator;
            if (p != NULL)
                allocator.deallocate((char*)p);
        }
};

}; // namespace ctk

#endif /* _CTK_ALIGNED_BASE_HPP */
