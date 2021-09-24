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

#ifndef CTK_SPE_FUNCTION_ARG_TPP
#define CTK_SPE_FUNCTION_ARG_TPP

#include "ctk_types.h"
#include "Aligned.hpp"
#include "AlignedArray.hpp"
#include <vector>
#include <string>

namespace ctk {

    //typedef ea_addr_t arg_size_t;
    typedef unsigned int arg_size_t;

// Value: [value]
#define CTK_MARSHAL_VALUE(_p, _v, _type, _sz)  do {  \
        *(_type*)_p = _v; _p += _sz; \
    } while(0)

#if 0
// Value: [sizeof(value), value]
#define CTK_MARSHAL_VALUE(_p, _v, _type, _sz)  do {  \
        *(arg_size_t*)_p = _sz; _p += sizeof(arg_size_t); \
        *(_type*)_p = _v; _p += _sz; \
    } while(0)
#endif

// Altivec Vector: [(align addr) 16 bytes value]
#define CTK_MARSHAL_VECTOR(_p, _v, _type)  do {  \
        _p = (char*)ALIGN_UP_PTR(_p, 16); \
        *(_type*)_p = _v; _p += 16; \
    } while(0)

// Pointer: [address, sizeof(value)]
#define CTK_MARSHAL_PTR(_p, _v, _value_sz)  do {  \
        *(ea_addr_t *)_p = (ea_addr_t)(unsigned long)(void*)_v; \
        _p += sizeof(ea_addr_t); \
        *(arg_size_t *)_p = _value_sz; \
        _p += sizeof(arg_size_t); \
    } while(0)

// Array: [address, sizeof(unit), num_elements]
#define CTK_MARSHAL_ARRAY(_p, _v, _unit_sz, _nelems)  do {  \
        *(ea_addr_t *)_p = (ea_addr_t)(unsigned long)(void*)_v; \
        _p += sizeof(ea_addr_t); \
        *(arg_size_t *)_p = _unit_sz; \
        _p += sizeof(arg_size_t); \
        *(arg_size_t *)_p = _nelems; \
        _p += sizeof(arg_size_t); \
    } while(0)

    // Values: encode as VALUE
    template <typename T> struct SpeFunctionArg {
        static inline void marshal(char *&ptr, T &v) {
            CTK_MARSHAL_VALUE(ptr, v, T, sizeof(T));
        }
    };

    // Values (long): encode as 64-bit VALUE
    template <> struct SpeFunctionArg <long> {
        static inline void marshal(char *&ptr, long &v) {
            CTK_MARSHAL_VALUE(ptr, v, unsigned long long, 8);
        }
    };

    // Void pointers: decode as VALUE of ea_addr_t
    template <> struct SpeFunctionArg <void*>
    {
        static inline void marshal(char *&ptr, void *&v) {
            CTK_MARSHAL_VALUE(ptr, EAADDR(v), ea_addr_t, sizeof(ea_addr_t));
        }
    };

#ifdef __ALTIVEC__
    // Vectors: encode as VECTOR
# define DECLARE_MARSHAL_FOR_VECTOR(_vec_type) \
    template <> struct SpeFunctionArg <_vec_type> { \
        static inline void marshal(char *&ptr, _vec_type &v) { \
            CTK_MARSHAL_VECTOR(ptr, v, _vec_type); \
        } \
    }
    DECLARE_MARSHAL_FOR_VECTOR(vector unsigned char);
    DECLARE_MARSHAL_FOR_VECTOR(vector signed char);
    DECLARE_MARSHAL_FOR_VECTOR(vector unsigned short);
    DECLARE_MARSHAL_FOR_VECTOR(vector signed short);
    DECLARE_MARSHAL_FOR_VECTOR(vector unsigned int);
    DECLARE_MARSHAL_FOR_VECTOR(vector signed int);
    DECLARE_MARSHAL_FOR_VECTOR(vector float);
    //DECLARE_MARSHAL_FOR_VECTOR(vector pixel);
#endif /* __ALTIVEC__ */
    
    // References: encode as POINTER
    template <typename T> struct SpeFunctionArg <T&> {
        static inline void marshal(char *&ptr, T &v) {
            CTK_ASSERT(IS_ALIGNED((unsigned long)(void*)&v, 16));
            CTK_ASSERT(IS_ALIGNED(sizeof(T), 16));
            CTK_MARSHAL_PTR(ptr, &v, sizeof(T));
        }
    };

    // Pointers: encode as POINTER
    template <typename T> struct SpeFunctionArg <T*> {
        static inline void marshal(char *&ptr, T *v) {
            CTK_ASSERT(IS_ALIGNED((unsigned long)v, 16));
            CTK_ASSERT(IS_ALIGNED(sizeof(T), 16));
            CTK_MARSHAL_PTR(ptr, v, sizeof(T));
        }
    };

    // Aligned values: encode as POINTER
    template <typename T, int _ALIGN>
    struct SpeFunctionArg <Aligned<T, _ALIGN> &>
    {
        static inline void marshal(char *&ptr, Aligned<T, _ALIGN> &v) {
            CTK_MARSHAL_PTR(ptr, v.ptr(), (Aligned<T, _ALIGN>::size));
        }
    };

    // Aligned Array: encode as ARRAY
    template <typename T, int _ALIGN>
    struct SpeFunctionArg <AlignedArray<T, _ALIGN>&>
    {
        static inline void marshal(char *&ptr, AlignedArray<T, _ALIGN> &v) {
            CTK_MARSHAL_ARRAY(ptr, 
                v.ptr(), (Aligned<T, _ALIGN>::size), v.size());
        }
    };

#undef CTK_MARSHAL_VALUE
#undef CTK_MARSHAL_VECTOR
#undef CTK_MARSHAL_PTR
#undef CTK_MARSHAL_ARRAY
#undef DECLARE_MARSHAL_FOR_VECTOR

}; // namespace ctk

#endif /* CTK_SPE_FUNCTION_ARG_TPP */
