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

#ifndef _CTK_SPU_RBUF_H
#define _CTK_SPU_RBUF_H

/**
 * @file ctk_spu_rbuf.h
 * @brief Definitions for READ multi-buffer
 */
#include "ctk_spu_common.h"

#ifdef __SPU__

#ifdef __cplusplus
extern "C" {
#endif

#define CTK_RWBUF_NAME(suffix)     _ctk_rbuf ## suffix
#define CTK_RWBUF_READ  1
#undef CTK_RWBUF_WRITE
#include "ctk_spu_rwbuf-impl.h"
#undef CTK_RWBUF_NAME

typedef _ctk_rbuf_t   ctk_rbuf_t;

/**
 * Initializes the given read/write buffer with simplified parameters
 * (buffer size == max transfer size == gap size between each in/out data unit)
 * @param[in,out] rbuf - the read buffer
 * @param[in] name - the name of the buffer - mainly for debug
 * @param[in] buf_addr - the start addr of the local buffer (MUST be aligned)
 * @param[in] buf_size - the unit size of each local buffer (MUST be aligned)
 * @param[in] buf_num - # of the local buffer unit
 * @param[in] in_addr - the start addr of the EA data (MUST be aligned)
 * @param[in] in_size - the total size of the EA data
 */
static inline void ctk_rbuf_init(ctk_rbuf_t *rbuf, const char *buf_name, 
      void *buf_addr, unsigned int buf_size, unsigned int buf_num,
      ea_addr_t in_addr, unsigned int in_size) __attribute__((always_inline)); 
static inline void
ctk_rbuf_init(ctk_rbuf_t *rbuf, const char *buf_name, 
      void *buf_addr, unsigned int buf_size, unsigned int buf_num,
      ea_addr_t in_addr, unsigned int in_size)
{
    _ctk_rbuf_init(rbuf, buf_name, buf_addr, buf_size, buf_num, in_addr, in_size);
}

/**
 * Resets the internal state of the cyclic buffer
 * @param[in,out] rbuf - the cyclic buffer
 */
static inline void ctk_rbuf_reset(ctk_rbuf_t *rbuf) __attribute__((always_inline)); 
static inline void
ctk_rbuf_reset(ctk_rbuf_t *rbuf)
{
    _ctk_rbuf_reset(rbuf);
}

/**
 * Starts a DMA get for the given read buffer
 * @param[in] rbuf - the read buffer
 * @param[in] offset - the source EA offset addr (starts from #in_addr
 *                     given for #ctk_rbuf_init)
 * @param[in] size - the transfer size
 * @return 1 if we actually issued a non-zero get, 0 otherwise
 */
static inline int ctk_rbuf_start_get(ctk_rbuf_t *rbuf, ctk_uint32_t ea_offset, ctk_uint32_t size) __attribute__((always_inline)); 
static inline int
ctk_rbuf_start_get(ctk_rbuf_t *rbuf, ctk_uint32_t ea_offset, ctk_uint32_t size)
{
    return _ctk_rbuf_start_get(rbuf, ea_offset, size);
}

/**
 * Starts a DMA get with an exact size (no boundary check; issue this routine
 * only when you can be sure that issuing that size of get won't cause
 * segv fault.)
 * @param[in] rbuf - the read buffer
 * @param[in] offset - the source EA offset addr (starts from #in_addr
 *                     given for #ctk_rbuf_init)
 * @param[in] size - the transfer size
 * @return 1 if we actually issued a non-zero get, 0 otherwise
 */
static inline void ctk_rbuf_start_get_exact(ctk_rbuf_t *rbuf, ctk_uint32_t ea_offset, ctk_uint32_t size) __attribute__((always_inline)); 
static inline void
ctk_rbuf_start_get_exact(ctk_rbuf_t *rbuf, ctk_uint32_t ea_offset, ctk_uint32_t size)
{
    _ctk_rbuf_start_get_exact(rbuf, ea_offset, size);
}

/**
 * Waits a DMA get for the given read buffer and returns the filled buffer addr
 * @param[in] rbuf - the read buffer
 * @param[out] buf - the read buffer pointer
 * @return the actual transfer size
 */
static inline int ctk_rbuf_wait_get(ctk_rbuf_t *rbuf, void **buf) __attribute__((always_inline)); 
static inline int
ctk_rbuf_wait_get(ctk_rbuf_t *rbuf, void **buf)
{
    return _ctk_rbuf_wait_get(rbuf, buf);
}

/**
 * Returns the buffer address specified by the given index
 * @param[in] rbuf - the read buffer
 * @param[in] idx - the buffer index to acquire
 * @param[out] buf - the address to the buffer unit specified by the given idx
 * @return the size of the returning buffer
 */
static inline int ctk_rbuf_get_buf(ctk_rbuf_t *rbuf, int idx, void **buf) __attribute__((always_inline)); 
static inline int ctk_rbuf_get_buf(ctk_rbuf_t *rbuf, int idx, void **buf) {
    return _ctk_rbuf_get_buf(rbuf, idx, buf);
}

/**
 * Shift the buffer index so that the next ctk_rwbuf_get_buf(rwbuf, n)
 * will return the next buffer address previously specified by index n + 1.
 * @param[in] rbuf - the read buffer
 */
static inline void ctk_rbuf_shift_buf(ctk_rbuf_t *rbuf) __attribute__((always_inline)); 
static inline void ctk_rbuf_shift_buf(ctk_rbuf_t *rbuf) {
    _ctk_rbuf_shift_buf(rbuf);
}

#ifdef __cplusplus
}
#endif

#endif /* __SPU__ */

#endif /* _CTK_SPU_RBUF_H */
