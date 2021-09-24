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

#ifndef _CTK_SPU_FUNCTION_ARG_TPP
#define _CTK_SPU_FUNCTION_ARG_TPP

#include "ctk_spu_common.h"
#include <vector>

namespace ctk {

    //typedef ea_addr_t arg_size_t;
    typedef unsigned int arg_size_t;

// Value: [value]
#define CTK_DEMARSHAL_VALUE(_p, _v, _type)  do {  \
        _v = *(_type*)_p; _p += sizeof(_type); \
    } while(0)

#if 0
// Value: [sizeof(value), value]
#define CTK_DEMARSHAL_VALUE(_p, _v, _type)  do {  \
        int _sz = *(arg_size_t*)_p; _p += sizeof(arg_size_t); \
        _v = *(_type*)_p; _p += _sz; \
    } while(0)
#endif

// SPU Vector: [(align addr) 16 bytes value]
#define CTK_DEMARSHAL_VECTOR(_p, _v, _type) do { \
        _p = (char*)ALIGN_UP_PTR(_p, 16); \
        _v = *(_type*)_p; _p += 16; \
    } while(0)

// Pointer: [address, sizeof(value)]
#define CTK_DEMARSHAL_PTR(_p, _addr, _value_sz)  do {  \
        _addr = *(ea_addr_t *)_p; _p += sizeof(ea_addr_t); \
        _value_sz = *(arg_size_t *)_p; _p += sizeof(arg_size_t); \
    } while(0)

// Array: [address, sizeof(unit), num_elements]
#define CTK_DEMARSHAL_ARRAY(_p, _addr, _unit_sz, _nelems)  do {  \
        _addr = *(ea_addr_t *)_p; _p += sizeof(ea_addr_t); \
        _unit_sz = *(arg_size_t *)_p; _p += sizeof(arg_size_t); \
        _nelems = *(arg_size_t *)_p; _p += sizeof(arg_size_t); \
    } while(0)

#define DEMARSHAL_ARGS  char *outbufp __attribute__((unused)), \
                        ea_addr_t *&cleanup_addrs __attribute__((unused)), \
                        int *&cleanup_sizes
#define CLEANUP_ARGS    ea_addr_t *&cleanup_addrs, int *&cleanup_sizes

    // Values: decode as VALUE
    template <typename T> struct SpuFunctionArg
    {
        static inline void demarshal(char *&ptr, T &v, DEMARSHAL_ARGS) {
            CTK_DEMARSHAL_VALUE(ptr, v, T);
        }
        static inline void cleanup(T &v, CLEANUP_ARGS) { }
        enum { outline_size = 0, need_cleanup_ptr = 0 };
        typedef T declare_type;
    };

    // Void pointers: decode as VALUE
    template <> struct SpuFunctionArg <void*>
    {
        static inline void demarshal(char *&ptr, void *&v, DEMARSHAL_ARGS) {
            CTK_DEMARSHAL_VALUE(ptr, v, void*);
        }
        static inline void cleanup(void *&v, CLEANUP_ARGS) { }
        enum { outline_size = 0, need_cleanup_ptr = 0 };
        typedef void *declare_type;
    };

    // Vectors: decode as VECTOR
#define DECLARE_DEMARSHAL_FOR_VECTOR(_vec_type) \
    template <> struct SpuFunctionArg <_vec_type> { \
        static inline void demarshal(char *&ptr, _vec_type &v, DEMARSHAL_ARGS) { \
            CTK_DEMARSHAL_VECTOR(ptr, v, _vec_type); \
        } \
        static inline void cleanup(_vec_type &v, CLEANUP_ARGS) { } \
        enum { outline_size = 0, need_cleanup_ptr = 0 }; \
        typedef _vec_type declare_type; \
    };
    DECLARE_DEMARSHAL_FOR_VECTOR(vector unsigned char);
    DECLARE_DEMARSHAL_FOR_VECTOR(vector signed char);
    DECLARE_DEMARSHAL_FOR_VECTOR(vector unsigned short);
    DECLARE_DEMARSHAL_FOR_VECTOR(vector signed short);
    DECLARE_DEMARSHAL_FOR_VECTOR(vector unsigned int);
    DECLARE_DEMARSHAL_FOR_VECTOR(vector signed int);
    DECLARE_DEMARSHAL_FOR_VECTOR(vector float);
    //DECLARE_DEMARSHAL_FOR_VECTOR(vector double);        // no double in VMX

    // References: decode as POINTER (does DMA get and put)
    template <typename T> struct SpuFunctionArg <T&>
    {
        static inline void demarshal(char *&ptr, T &v, DEMARSHAL_ARGS) {
            ea_addr_t addr;
            arg_size_t size;
            CTK_DEMARSHAL_PTR(ptr, addr, size);
            CTK_DEBUG_ASSERT(size <= (Aligned<T, 128>::size));
            CTK_DEBUG_ASSERT(IS_ALIGNED(addr, 16));
            CTK_DEBUG_ASSERT(IS_ALIGNED(size, 16));
            ::ctk_dma_get((void*)&v, addr, size, 0);
            *cleanup_addrs++ = addr;
            *cleanup_sizes++ = size;
        }
        static inline void cleanup(T &v, CLEANUP_ARGS) {
            ::ctk_dma_put((void*)&v, *cleanup_addrs++, *cleanup_sizes++, 0);
        }
        enum { outline_size = 0, need_cleanup_ptr = 1 };
        typedef T declare_type;
    };

    // Pointers: decode as POINTER (does DMA get and put)
    template <typename T> struct SpuFunctionArg <T*>
    {
        static inline void demarshal(char *&ptr, T *&v, DEMARSHAL_ARGS) {
            ea_addr_t addr;
            arg_size_t size;
            CTK_DEMARSHAL_PTR(ptr, addr, size);
            CTK_DEBUG_ASSERT(size <= outline_size);
            CTK_DEBUG_ASSERT(IS_ALIGNED(addr, 16));
            CTK_DEBUG_ASSERT(IS_ALIGNED(size, 16));
            ::ctk_dma_get((void*)outbufp, addr, size, 0);
            v = (T*)outbufp;
            outbufp += outline_size;
            *cleanup_addrs++ = addr;
            *cleanup_sizes++ = size;
        }
        static inline void cleanup(T *&v, CLEANUP_ARGS) {
            ::ctk_dma_put((void*)v, *cleanup_addrs++, *cleanup_sizes++, 0);
        }
        enum { outline_size = Aligned<T, 128>::size, need_cleanup_ptr = 1 };
        typedef T* declare_type;
    };

    // Aligned values: decode as POINTER (does DMA get and put)
    template <typename T, int _ALIGN, typename _ALLOC>
    struct SpuFunctionArg <Aligned<T, _ALIGN, _ALLOC>&>
    {
        static inline void demarshal(char *&ptr, Aligned<T,_ALIGN,_ALLOC> &v, DEMARSHAL_ARGS) {
            ea_addr_t addr;
            arg_size_t size;
            /* in SPE tasks constructor may not be called properly! */
            new((void*)&v) Aligned<T,_ALIGN,_ALLOC>();
            CTK_DEMARSHAL_PTR(ptr, addr, size);
            CTK_DEBUG_ASSERT(size <= v.size());
            CTK_DEBUG_ASSERT(IS_ALIGNED(addr, 16));
            CTK_DEBUG_ASSERT(IS_ALIGNED(size, 16));
            ::ctk_dma_get(v.ptr(), addr, size, 0);
            *cleanup_addrs++ = addr;
            *cleanup_sizes++ = size;
        }
        static inline void cleanup(Aligned<T,_ALIGN,_ALLOC> &v, CLEANUP_ARGS) {
            ea_addr_t addr = *cleanup_addrs++;
            arg_size_t size = *cleanup_sizes++;
            ::ctk_dma_put(v.ptr(), addr, size, 0);
        }
        enum { outline_size = 0, need_cleanup_ptr = 1 };
        typedef Aligned<T, _ALIGN, _ALLOC> declare_type;
    };

    // Aligned Array: decode as ARRAY (does DMA get and put)
    // (why do we need to be declared as aligned array as we are functions??)
    template <typename T, int _ALIGN, typename _ALLOC>
    struct SpuFunctionArg <AlignedArray<T, _ALIGN, _ALLOC>&>
    {
        static inline void demarshal(char *&ptr, AlignedArray<T,_ALIGN,_ALLOC> &v, DEMARSHAL_ARGS) {
            ea_addr_t addr;
            arg_size_t size, nelems;
            /* in SPE tasks constructor may not be called properly! */
            new((void*)&v) AlignedArray<T,_ALIGN,_ALLOC>();
            CTK_DEMARSHAL_ARRAY(ptr, addr, size, nelems);
            v.resize(nelems);
            CTK_DEBUG_ASSERT(IS_ALIGNED(addr, 16));
            CTK_DEBUG_ASSERT(IS_ALIGNED(size, 16));
            *cleanup_addrs++ = addr;
            *cleanup_sizes++ = size;
            for (size_t i = 0; i < v.size(); i++) {
                ::ctk_dma_get((void*)&v[i], addr, size, 0);
                addr += size;
            }
        }
        static inline void cleanup(AlignedArray<T,_ALIGN,_ALLOC> &v, CLEANUP_ARGS) {
            ea_addr_t addr = *cleanup_addrs++;
            arg_size_t size = *cleanup_sizes++;
            for (size_t i = 0; i < v.size(); i++) {
                ::ctk_dma_put((void*)&v[i], addr, size, 0);
                addr += size;
            }
        }
        enum { outline_size = 0, need_cleanup_ptr = 1 };
        typedef AlignedArray<T, _ALIGN,_ALLOC> declare_type;
    };

#undef CTK_DEMARSHAL_VALUE
#undef CTK_DEMARSHAL_VECTOR
#undef CTK_DEMARSHAL_PTR
#undef CTK_DEMARSHAL_ARRAY
#undef DECLARE_DEMARSHAL_FOR_VECTOR

}; // namespace ctk


#endif /* _CTK_SPU_FUNCTION_ARG_TPP */
