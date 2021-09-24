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

#ifndef _CTK_SPU_WBUF_H
#define _CTK_SPU_WBUF_H

/**
 * @file ctk_spu_wbuf.h
 * @brief Definitions for WRITE multi-buffer
 */
#include "ctk_spu_common.h"

#ifdef __SPU__

#ifdef __cplusplus
extern "C" {
#endif

#define CTK_RWBUF_NAME(suffix)     _ctk_wbuf ## suffix
#define CTK_RWBUF_WRITE 1
#undef CTK_RWBUF_READ
#include "ctk_spu_rwbuf-impl.h"
#undef CTK_RWBUF_NAME

typedef _ctk_wbuf_t    ctk_wbuf_t;

/**
 * Initializes the given read/write buffer with simplified parameters
 * (buffer size == max transfer size == gap size between each in/out data unit)
 * @param[in,out] wbuf - the write buffer
 * @param[in] name - the name of the buffer - mainly for debug
 * @param[in] buf_addr - the start addr of the local buffer (MUST be aligned)
 * @param[in] buf_size - the unit size of each local buffer (MUST be aligned)
 * @param[in] buf_num - # of the local buffer unit
 * @param[in] out_addr - the start addr of the EA data (MUST be aligned)
 * @param[in] out_size - the total size of the EA data 
 */
static inline void ctk_wbuf_init(ctk_wbuf_t *wbuf, const char *buf_name, 
      void *buf_addr, unsigned int buf_size, unsigned int buf_num,
      ea_addr_t out_addr, unsigned int out_size) __attribute__((always_inline)); 
static inline void
ctk_wbuf_init(ctk_wbuf_t *wbuf, const char *buf_name, 
      void *buf_addr, unsigned int buf_size, unsigned int buf_num,
      ea_addr_t out_addr, unsigned int out_size)
{
    _ctk_wbuf_init(wbuf, buf_name, buf_addr, buf_size, buf_num, 
        out_addr, out_size);
}

/**
 * Resets the internal state of the cyclic buffer
 * @param[in,out] wbuf - the cyclic buffer
 */
static inline void ctk_wbuf_reset(ctk_wbuf_t *wbuf) __attribute__((always_inline)); 
static inline void
ctk_wbuf_reset(ctk_wbuf_t *wbuf)
{
    _ctk_wbuf_reset(wbuf);
}

/**
 * Starts a DMA put for the given write buffer
 * @param[in] wbuf - the write buffer
 * @param[in] offset - the dest EA offset addr (starts from #out_addr
 *                     given for #ctk_wbuf_init)
 * @param[in] size - the transfer size
 */
static inline void ctk_wbuf_start_put(ctk_wbuf_t *wbuf, ctk_uint32_t ea_offset, ctk_uint32_t size) __attribute__((always_inline)); 
static inline void
ctk_wbuf_start_put(ctk_wbuf_t *wbuf, ctk_uint32_t ea_offset, ctk_uint32_t size)
{
    _ctk_wbuf_start_put(wbuf, ea_offset, size);
}

/**
 * Starts a DMA put with an exact size (no boundary check; issue this routine
 * only when you can be sure that issuing that size of get won't cause
 * segv fault.)
 * @param[in] wbuf - the write buffer
 * @param[in] offset - the dest EA offset addr (starts from #out_addr
 *                     given for #ctk_wbuf_init)
 * @param[in] size - the transfer size
 */
static inline void ctk_wbuf_start_put_exact(ctk_wbuf_t *wbuf, ctk_uint32_t ea_offset, ctk_uint32_t size) __attribute__((always_inline)); 
static inline void
ctk_wbuf_start_put_exact(ctk_wbuf_t *wbuf, ctk_uint32_t ea_offset, ctk_uint32_t size)
{
    _ctk_wbuf_start_put_exact(wbuf, ea_offset, size);
}

/**
 * Returns a next buffer address
 * @param[in] wbuf - the write buffer
 * @param[out] buf - address to store the put data
 * @return the size of the returning buffer
 */
static inline int ctk_wbuf_request_put_buf(ctk_wbuf_t *wbuf, void **buf) __attribute__((always_inline)); 
static inline int
ctk_wbuf_request_put_buf(ctk_wbuf_t *wbuf, void **buf)
{
    return _ctk_wbuf_request_put_buf(wbuf, buf);
}

/**
 * Waits all the DMA puts which have been issued so far
 * @param[in] wbuf - the write buffer
 */
static inline void ctk_wbuf_wait_all(ctk_wbuf_t *wbuf) __attribute__((always_inline)); 
static inline void
ctk_wbuf_wait_all(ctk_wbuf_t *wbuf)
{
    _ctk_wbuf_wait_all(wbuf);
}

#ifdef __cplusplus
}
#endif

#endif /* __SPU__ */

#endif /* _CTK_SPU_WBUF_H */
