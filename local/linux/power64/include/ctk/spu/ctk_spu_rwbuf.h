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

#ifndef _CTK_SPU_RWBUF_H
#define _CTK_SPU_RWBUF_H

/**
 * @file ctk_spu_rwbuf.h
 * @brief Definitions for READ/WRITE multi-buffer
 */
#include "ctk_spu_common.h"

#ifdef __SPU__

#ifdef __cplusplus
extern "C" {
#endif

#define CTK_RWBUF_NAME(suffix)     _ctk_rwbuf ## suffix
#define CTK_RWBUF_READ  1
#define CTK_RWBUF_WRITE 1
#include "ctk_spu_rwbuf-impl.h"
#undef CTK_RWBUF_NAME

typedef _ctk_rwbuf_t    ctk_rwbuf_t;

/**
 * Initializes the given read/write buffer with simplified parameters
 * (buffer size == max transfer size == gap size between each in/out data unit)
 * @param[in,out] rwbuf - the read/write buffer
 * @param[in] name - the name of the buffer - mainly for debug
 * @param[in] buf_addr - the start addr of the local buffer (MUST be aligned)
 * @param[in] buf_size - the unit size of each local buffer (MUST be aligned)
 * @param[in] buf_num - # of the local buffer unit
 * @param[in] in_addr - the start addr of the EA data (MUST be aligned)
 * @param[in] in_size - the total size of the EA data
 * @param[in] out_addr - the start addr of the EA data (MUST be aligned)
 * @param[in] out_size - the total size of the EA data 
 */
static inline void ctk_rwbuf_init(ctk_rwbuf_t *rwbuf, const char *buf_name, 
      void *buf_addr, unsigned int buf_size, unsigned int buf_num,
      ea_addr_t in_addr, unsigned int in_size, 
      ea_addr_t out_addr, unsigned int out_size) __attribute__((always_inline)); 
static inline void
ctk_rwbuf_init(ctk_rwbuf_t *rwbuf, const char *buf_name, 
      void *buf_addr, unsigned int buf_size, unsigned int buf_num,
      ea_addr_t in_addr, unsigned int in_size, 
      ea_addr_t out_addr, unsigned int out_size)
{
    _ctk_rwbuf_init(rwbuf, buf_name, buf_addr, buf_size, buf_num, 
        in_addr, in_size, out_addr, out_size);
}

/**
 * Resets the internal state of the cyclic buffer
 * @param[in,out] rwbuf - the cyclic buffer
 */
static inline void ctk_rwbuf_reset(ctk_rwbuf_t *rwbuf) __attribute__((always_inline)); 
static inline void
ctk_rwbuf_reset(ctk_rwbuf_t *rwbuf)
{
    _ctk_rwbuf_reset(rwbuf);
}

/**
 * Starts a DMA get for the given buffer
 * @param[in] rwbuf - the buffer
 * @param[in] offset - the source EA offset addr (starts from #in_addr
 *                     given for #ctk_rwbuf_init)
 * @param[in] size - the transfer size
 * @return 1 if we actually issued a non-zero get, 0 otherwise
 */
static inline int ctk_rwbuf_start_get(ctk_rwbuf_t *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size) __attribute__((always_inline)); 
static inline int
ctk_rwbuf_start_get(ctk_rwbuf_t *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size)
{
    return _ctk_rwbuf_start_get(rwbuf, ea_offset, size);
}

/**
 * Starts a DMA get with an exact size (no boundary check; issue this routine
 * only when you can be sure that issuing that size of get won't cause
 * segv fault.)
 * @param[in] rwbuf - the buffer
 * @param[in] offset - the source EA offset addr (starts from #in_addr
 *                     given for #ctk_rwbuf_init)
 * @param[in] size - the transfer size
 * @return 1 if we actually issued a non-zero get, 0 otherwise
 */
static inline void ctk_rwbuf_start_get_exact(ctk_rwbuf_t *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size) __attribute__((always_inline)); 
static inline void
ctk_rwbuf_start_get_exact(ctk_rwbuf_t *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size)
{
    _ctk_rwbuf_start_get_exact(rwbuf, ea_offset, size);
}

/**
 * Waits a DMA get for the given read buffer and returns the filled buffer addr
 * @param[in] rwbuf - the buffer
 * @param[out] buf - the read buffer pointer
 * @return the actual transfer size
 */
static inline int ctk_rwbuf_wait_get(ctk_rwbuf_t *rwbuf, void **buf) __attribute__((always_inline)); 
static inline int
ctk_rwbuf_wait_get(ctk_rwbuf_t *rwbuf, void **buf)
{
    return _ctk_rwbuf_wait_get(rwbuf, buf);
}

/**
 * Starts a DMA put for the given buffer
 * @param[in] rwbuf - the buffer
 * @param[in] offset - the dest EA offset addr (starts from #out_addr
 *                     given for #ctk_rwbuf_init)
 * @param[in] size - the transfer size
 */
static inline void ctk_rwbuf_start_put(ctk_rwbuf_t *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size) __attribute__((always_inline)); 
static inline void
ctk_rwbuf_start_put(ctk_rwbuf_t *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size)
{
    _ctk_rwbuf_start_put(rwbuf, ea_offset, size);
}

/**
 * Starts a DMA put with an exact size (no boundary check; issue this routine
 * only when you can be sure that issuing that size of get won't cause
 * segv fault.)
 * @param[in] rwbuf - the buffer
 * @param[in] offset - the dest EA offset addr (starts from #out_addr
 *                     given for #ctk_rwbuf_init)
 * @param[in] size - the transfer size
 */
static inline void ctk_rwbuf_start_put_exact(ctk_rwbuf_t *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size) __attribute__((always_inline)); 
static inline void
ctk_rwbuf_start_put_exact(ctk_rwbuf_t *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size)
{
    _ctk_rwbuf_start_put_exact(rwbuf, ea_offset, size);
}

/**
 * Waits all the DMA puts which have been issued so far
 * @param[in] rwbuf - the buffer
 */
static inline void ctk_rwbuf_wait_all(ctk_rwbuf_t *rwbuf) __attribute__((always_inline)); 
static inline void
ctk_rwbuf_wait_all(ctk_rwbuf_t *rwbuf)
{
    return _ctk_rwbuf_wait_all(rwbuf);
}

/**
 * Returns the buffer address specified by the idx.  If the idx
 * exceeds the actual buffer number, the mod value is used.
 * @param[in] rwbuf - the read/write buffer
 * @param[in] idx - the buffer index to acquire
 * @param[out] buf - the address to the buffer unit specified by the given idx
 * @return the size of the returning buffer
 */
static inline int ctk_rwbuf_get_buf(ctk_rwbuf_t *rwbuf, int idx, void **buf)  __attribute__((always_inline)); 
static inline int ctk_rwbuf_get_buf(ctk_rwbuf_t *rwbuf, int idx, void **buf) {
    return _ctk_rwbuf_get_buf(rwbuf, idx, buf);
}

/**
 * Shift the buffer index so that the ctk_rwbuf_get_buf(rwbuf, n)
 * returns the next buffer address previously specified by index n + 1.
 * @param[in] rwbuf - the buffer
 */
static inline void ctk_rwbuf_shift_buf(ctk_rwbuf_t *rwbuf) __attribute__((always_inline)); 
static inline void ctk_rwbuf_shift_buf(ctk_rwbuf_t *rwbuf) {
    _ctk_rwbuf_shift_buf(rwbuf);
}

#include "ctk_spu_rbuf.h"
#include "ctk_spu_wbuf.h"

#ifdef __cplusplus
}
#endif

#endif /* __SPU__ */

#endif /* _CTK_SPU_RWBUF_H */


