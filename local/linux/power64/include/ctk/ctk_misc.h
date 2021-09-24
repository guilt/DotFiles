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

#ifndef _CTK_MISC_H
#define _CTK_MISC_H

/**
 * @file ctk_misc.h: Definitions for miscelaneous routines
 */

#ifndef __SPU__
/**
 * Issue an eieio (enforce in-order execution of I/O) instruction.
 * This creates a store-store memory barrier for cacheable memory
 * and creates a full barrier for noncacheable memory (e.g. MMIO registers).
 */
static inline void
ctk_write_barrier() {
    __asm__ __volatile__ ("eieio":::"memory");
}

/**
 * Issue an lwsync (light-weight sync) instruction to
 * create a store-store memory barrier only for cacheable accesses.
 * (alias to ctk_lwsync())
 */
static inline void
ctk_cache_write_barrier() {
    __asm__ __volatile__ ("lwsync");
}

/**
 * Issue a memory sync to create full barrier.
 * The 'sync' synchronizes everything, i.e., it syncs both
 * cacheable and noncacheable accesses.
 */
static inline void
ctk_full_barrier() {
    __asm__ __volatile__ ("sync":::"memory");
}

/**
 * Issue an eieio (enforce in-order execution of I/O) instruction.
 * This creates a store-store memory barrier for cacheable memory
 * and also creates a full barrier for noncacheable memory
 * (i.e. MMIO or PIO registers).
 */
static inline void
ctk_eieio() {
    __asm__ __volatile__ ("eieio");
}

/**
 * Issue an lwsync (light-weight sync) instruction.
 * This creates a store-store memory barrier only for cacheable accesses.
 */
static inline void
ctk_lwsync() {
    __asm__ __volatile__ ("lwsync");
}

/**
 * Issue a memory sync.
 * The 'sync' synchronizes everything, i.e., it syncs both
 * cacheable and noncacheable accesses.
 */
static inline void
ctk_sync() {
    __asm__ __volatile__ ("sync");
}

/**
 * Issue an isync (instruction sync) instruction.
 * This instruction prevents any following instructions
 * from executing until the outcome of any previous
 * conditional branches are known, and until it is known that no
 * previous instruction can generate an exception.  Thus it can be used
 * to create a one-way barrier.
 */
static inline void
ctk_isync() {
    __asm__ __volatile__ ("isync");
}

/**
 * Returns the number of leading zeros
 */
static inline int ctk_nlz(unsigned int x) __attribute__((always_inline));
static inline int ctk_nlz(unsigned int x)
{
    int ret;
    __asm__ __volatile__ (
        "cntlzw %0, %1\n"
        : "=&r"(ret)
        : "r"(x));
    return ret;
}

/**
 * Returns the number of leading zeros in the given double word
 */
static inline int ctk_nlz64(unsigned long long x) __attribute__((always_inline));
static inline int ctk_nlz64(unsigned long long x)
{
    int n = ctk_nlz((unsigned long)(x>>32));
    if (n == 32) n += ctk_nlz((unsigned long)(x & 0xffffffff));
    return n;
}

/**
 * Returns the number of leading ones
 */
static inline int ctk_nlo(unsigned int x) __attribute__((always_inline));
static inline int ctk_nlo(unsigned int x)
{
    return ctk_nlz(~x);
}

/**
 * Returns the number of leading ones in the given double word
 */
static inline int ctk_nlo64(unsigned long long x) __attribute__((always_inline));
static inline int ctk_nlo64(unsigned long long x)
{
    int n = ctk_nlo((unsigned long)(x>>32));
    if (n == 32) n += ctk_nlo((unsigned long)(x & 0xffffffff));
    return n;
}

#endif

#endif /* _CTK_MISC_H */
