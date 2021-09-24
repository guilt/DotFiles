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

#ifndef _CTK_TYPES_H
#define _CTK_TYPES_H

/**
 * @file ctk_types.h
 * @brief Definitions of CTK common types.
 */

#include <sys/types.h>
#include <stdbool.h>

#ifdef __SPU__
# include <spu_intrinsics.h>
/*# include <spu_mfcio.h>*/
#endif

/* Basic types (PowerPC 64 bit) */
typedef char                 ctk_int8_t;
typedef short                ctk_int16_t;
typedef int                  ctk_int32_t;
typedef long long            ctk_int64_t;
typedef unsigned char        ctk_uint8_t;
typedef unsigned short       ctk_uint16_t;
typedef unsigned int         ctk_uint32_t;
typedef unsigned long long   ctk_uint64_t;

typedef ctk_uint32_t         ls_addr_t;   /**< LS address type (32-bit) */
typedef ctk_uint32_t         ls_size_t;   /**< LS size type (32-bit) */
typedef ctk_uint64_t         ea_addr_t;   /**< EA address type (64-bit) */
typedef ctk_uint64_t         ea_size_t;   /**< EA size type (64-bit) */
typedef ctk_uint32_t         ea_addr32_t; /**< EA 32-bit address type */
typedef ctk_uint32_t         dma_size_t;  /**< DMA transfer size type (32-bit) */

/** CTK SPE object types */
enum {
    CTK_SPE_CONTEXT_T, 
    CTK_SPE_THREAD_T, 
    CTK_SPE_THREAD_GROUP_T, 
    /* CTK_SPE_TASK_T, */
};

#ifdef __cplusplus
# define __cast(type, val)    (reinterpret_cast<type>(val))
# define __static_cast(type, val)    (static_cast<type>(val))
#else
# define __cast(type, val)           ((type)(val))
# define __static_cast(type, val)    ((type)(val))
#endif

#define ptr2int(p) (unsigned long)(p) /**< convert from pointer to int */
#define int2ptr(p) (void *)(p)        /**< convert from int to pointer */

/**< A macro to convert a 32/64-bit pointer to a portable ea_addr_t type */
#define EAADDR(p)     ((ea_addr_t)ptr2int(p))

/**< A macro to convert an ea_addr_t value to a 32/64-bit pointer */
#define __PTR(p)      ((void*)(unsigned long)(p))

/* Union type for ull (unsigned long long, 64-bit) and
 * ui (unsigned int, 32-bit) addresses */
typedef union {
    ctk_uint64_t ull;
    ctk_uint32_t ui[2];
} ctk_addr64_t;

/* 64-bit union type for various sizes of integers */
typedef union {
    ctk_uint8_t  val8[8];
    ctk_uint16_t val16[4];
    ctk_uint32_t val32[2];
    ctk_uint64_t val64;

    unsigned long long ull[2];
    unsigned int ui[4];
    unsigned short us[8];
    unsigned char uc[16];
    float f[4];
    double d[2];

    ea_addr_t ea_addr;
} ctk_val64_t;

#ifdef __ALTIVEC__
# undef __APPLE_ALTIVEC__
# include <altivec.h>
# undef bool
# define bool _Bool
#endif /* __ALTIVEC__ */

/* 128-bit union type for various sizes of integers */
typedef union {
    ctk_uint8_t  val8[16];
    ctk_uint16_t val16[8];
    ctk_uint32_t val32[4];
    ctk_uint64_t val64[2];
#if defined(__ALTIVEC__) || defined(__SPU__)
    vector signed char qwval;
#endif
} ctk_val128_t __attribute__((aligned(16)));

#ifndef LS_SIZE
#define LS_SIZE         0x40000         /**< SPE LS size (256K) */
#define LS_ADDR_SHIFT   18
#define LS_ADDR_MASK    (LS_SIZE - 1)   /**< SPE LS addr mask */
#endif /* LS_SIZE */

#define ALIGN_DOWN(v, align) ((v)&(-align)) /**< 2^n round*/
#define ALIGN_UP(v, align)   ALIGN_DOWN((v)+((align)-1), align) /**< 2^n ceil*/
#define IS_ALIGNED(v, align) (((v)&((align)-1)) == 0) /**< is 2^n aligned? */

#define ALIGN_DOWN_PTR(v, align) (void*)ALIGN_DOWN((unsigned long)(v), align) /**< 2^n round*/
#define ALIGN_UP_PTR(v, align)   (void*)ALIGN_UP((unsigned long)(v), align) /**< 2^n ceil*/
#define IS_ALIGNED_PTR(v, align) (((unsigned long)(v)&((align)-1)) == 0) /**< is 2^n aligned? */

#define IS_POWEROF2(v)       (((v)&(v-1)) == 0)              /**< is power of 2? */

#define ROUND_DOWN(v, align) ((int)((int)(v) / (align)) * (align))
#define ROUND_UP(v, align)   ((int)(((int)(v) + ((align) - 1)) / (align)) * (align))

#if defined(_LP64)
  /** returns high 32-bit of the given EA addr */
# define ea_hi32(x)  (ctk_uint32_t)(((ctk_uint64_t)ptr2int(x))>>32 & 0xffffffff)
#elif defined(__SPU__)
  /** returns high 32-bit of the given EA addr */
# define ea_hi32(x)  (ctk_uint32_t)(((ctk_uint64_t)(x))>>32 & 0xffffffff)
#else /* !SPU && !LP64 */
  /** returns high 32-bit of the given EA addr */
# define ea_hi32(x)  0x0
#endif /* !SPU && !LP64 */

/** returns low 32-bit of the given EA addr */
# define ea_lo32(x)  (ctk_uint32_t)((ptr2int(x) & 0xffffffff))

#endif /* _CTK_TYPES_H */
