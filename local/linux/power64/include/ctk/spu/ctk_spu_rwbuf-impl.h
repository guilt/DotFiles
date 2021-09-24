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

/**
 * @file ctk_spu_rwbuf-impl.h
 * @brief Definitions for multi-buffer/double-buffer common routines.
 * @note  all of the rwbuf, rbuf and wbuf implementation share large part of
 *        common code, but modular implementation somehow limited the
 *        performance.
 *        (Template programming might achieve both modularity and performance,
 *        but here in 'base' layer we assume simple and old C...)
 *        Therefore here we combines dirty macros and ifdefs in a tricky way.
 */
#include "ctk_spu_common.h"

#if 0
# define DEBUG
# undef DEBUG_PRINTF
# define DEBUG_PRINTF(...)    printf(__VA_ARGS__)
#endif

#ifdef DEBUG
char *strncpy(char *dest, const char *stc, size_t n);
#endif

#if !defined(CTK_RWBUF_NAME)
# error do not include this file directly!
#endif

#define CTK_RWBUF_T                CTK_RWBUF_NAME(_t)

#ifndef CTK_RWBUF_MAXNUM
# ifdef CTK_RWBUF_LARGE_BUFS
# define CTK_RWBUF_MAXNUM         128
# else /* !CTK_RWBUF_LARGE_BUFS */
# define CTK_RWBUF_MAXNUM         16
# endif /* !CTK_RWBUF_LARGE_BUFS */
# define CTK_RWBUF_NAMESIZE       64
#endif

/**
 * Structure for local multi buffer
 */
typedef struct {
    ls_addr_t buf_addr;     /**< local buffer address */
    ls_addr_t buf_end;      /**< local buffer end address */
    dma_size_t buf_size;     /**< local buffer size (= max transfer size) */
    int buf_num;      /**< # of local buffers */

    ctk_uint8_t buf_tags[CTK_RWBUF_MAXNUM];

#ifdef DEBUG
    const char name[CTK_RWBUF_NAMESIZE]; /**< for debug purpose */
#endif

#ifdef CTK_RWBUF_READ
    /* for get */
    ea_addr_t in_addr;     /**< the start of the EA in data address */
    ea_addr_t in_size;     /**< the size of the EA data */
    int get_req_idx, get_done_idx;
    int buf_idx;      /**< read index ptr */
    dma_size_t get_size[CTK_RWBUF_MAXNUM];
#endif  /* CTK_RWBUF_READ */

#ifdef CTK_RWBUF_WRITE
    /* for put */
    ea_addr_t out_addr;    /**< the start of the EA in data address */
    ea_addr_t out_size;    /**< the size of the EA data */
    int put_req_idx;
#endif /* CTK_RWBUF_WRITE */

} CTK_RWBUF_T;

/**
 * Returns the next buffer address of the given buffer
 */
static inline void *CTK_RWBUF_NAME(_next_buf)(CTK_RWBUF_T *rwbuf, void *cur) __attribute__((always_inline));
static inline void *
CTK_RWBUF_NAME(_next_buf)(CTK_RWBUF_T *rwbuf, void *cur) {
    ls_addr_t next = (ls_addr_t)cur + rwbuf->buf_size;
    if (next >= rwbuf->buf_end) next = rwbuf->buf_addr;
    return (void*)next;
}

/**
 * Waits a DMA transfer for the specified buffer
 */
static inline void CTK_RWBUF_NAME(_wait_transfer)(CTK_RWBUF_T *rwbuf, int idx) __attribute__((always_inline));
static inline void
CTK_RWBUF_NAME(_wait_transfer)(CTK_RWBUF_T *rwbuf, int idx)
{
    /* Wait DMA transfer for the buffer if there might be any */
    DEBUG_PRINTF("[%s] WAIT PUT/GET@%d\n", rwbuf->name, idx);
    ctk_dma_wait(rwbuf->buf_tags[idx]);
    rwbuf->buf_tags[idx] = 0;
}

/**
 * Clears all the DMA tags in the given rwbuf
 */
static inline void CTK_RWBUF_NAME(_clear_tags)(CTK_RWBUF_T *rwbuf) __attribute__((always_inline));
static inline void CTK_RWBUF_NAME(_clear_tags)(CTK_RWBUF_T *rwbuf)
{
# ifdef CTK_RWBUF_LARGE_BUFS
    memset(rwbuf->buf_tags, 0, sizeof(rwbuf->buf_tags));
# else /* !CTK_RWBUF_LARGE_BUFS */
    rwbuf->buf_tags[0] = rwbuf->buf_tags[1] = 0;
    rwbuf->buf_tags[2] = rwbuf->buf_tags[3] = 0;
    rwbuf->buf_tags[4] = rwbuf->buf_tags[5] = 0;
    rwbuf->buf_tags[6] = rwbuf->buf_tags[7] = 0;
    rwbuf->buf_tags[8] = rwbuf->buf_tags[9] = 0;
    rwbuf->buf_tags[10] = rwbuf->buf_tags[11] = 0;
    rwbuf->buf_tags[12] = rwbuf->buf_tags[13] = 0;
    rwbuf->buf_tags[14] = rwbuf->buf_tags[15] = 0;
# endif /* !CTK_RWBUF_LARGE_BUFS */
}

#ifdef CTK_RWBUF_READ
/**
 * Starts a DMA get
 * @return the size of the actual get size we requested
 */
static inline int CTK_RWBUF_NAME(_start_get)(CTK_RWBUF_T *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size) __attribute__((always_inline));
static inline int
CTK_RWBUF_NAME(_start_get)(CTK_RWBUF_T *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size)
{
    register int tag, get_flag;
    register qword tmp, tmp2, size_mask, get_mask;
    register int idx = rwbuf->get_req_idx;
    ea_addr_t ea_addr = rwbuf->in_addr + ea_offset;

    /* determines the transfer size */
    tmp = si_from_uint(ea_offset + size);
    size_mask = si_cgt(si_from_uint(rwbuf->in_size), tmp);
    tmp2 = si_selb(si_from_uint(rwbuf->in_size - ea_offset), 
                  si_from_uint(size), size_mask);
    get_mask = si_cgti(tmp2, 0);
    size = si_to_uint(si_and(tmp2, get_mask));
    rwbuf->get_size[idx] = size;

# ifdef CTK_RWBUF_WRITE
    /* Wait put for the buffer if there might be any */
    CTK_RWBUF_NAME(_wait_transfer)(rwbuf, idx);
# endif /* CTK_RWBUF_WRITE */

    /* Start get */
    tag = ctk_dma_new_tag();
    DEBUG_PRINTF("[%s] GET [%d] %p<-%llx %d\n", rwbuf->name, idx, 
                    (void*)(rwbuf->buf_addr + idx * rwbuf->buf_size),
                    ea_addr, size);
    ctk_dma_get_small((void*)(rwbuf->buf_addr + idx * rwbuf->buf_size),
                    ea_addr, size, tag);

    /* Increment get_req_idx conditionally */
    get_flag = si_to_uint(si_andi(get_mask, 0x1));
    rwbuf->buf_tags[idx] = tag;
    rwbuf->get_req_idx += get_flag;
    if (rwbuf->get_req_idx >= rwbuf->buf_num) rwbuf->get_req_idx = 0;

    return get_flag;
}

/**
 * Starts a DMA get with an exact size (no boundary check; issue this routine
 * only when you can be sure that issuing that size of get won't cause
 * segv fault.)
 */
static inline void
CTK_RWBUF_NAME(_start_get_exact)(CTK_RWBUF_T *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size)
{
    register int tag;
    register int idx = rwbuf->get_req_idx;
    ea_addr_t ea_addr = rwbuf->in_addr + ea_offset;

    rwbuf->get_size[idx] = size;

# ifdef CTK_RWBUF_WRITE
    /* Wait put for the buffer if there might be any */
    CTK_RWBUF_NAME(_wait_transfer)(rwbuf, idx);
# endif /* CTK_RWBUF_WRITE */

    /* Start get */
    tag = ctk_dma_new_tag();
    DEBUG_PRINTF("[%s] GET [%d] %p<-%x:%x %d\n", rwbuf->name, idx, 
                    (void*)(rwbuf->buf_addr + idx * rwbuf->buf_size),
                    ea_hi32(ea_addr), ea_lo32(ea_addr), size);
    ctk_dma_get_small((void*)(rwbuf->buf_addr + idx * rwbuf->buf_size),
                    ea_addr, size, tag);

    /* Increment get_req_idx */
    rwbuf->buf_tags[idx] = tag;
    rwbuf->get_req_idx++;
    if (rwbuf->get_req_idx >= rwbuf->buf_num) rwbuf->get_req_idx = 0;
}

/**
 * Waits a DMA get for the given read buffer and returns the filled buffer addr
 * @return the actual transfer size
 */
static inline int CTK_RWBUF_NAME(_wait_get)(CTK_RWBUF_T *rwbuf, void **buf) __attribute__((always_inline));
static inline int
CTK_RWBUF_NAME(_wait_get)(CTK_RWBUF_T *rwbuf, void **buf)
{
    register int idx = rwbuf->get_done_idx;

    CTK_RWBUF_NAME(_wait_transfer)(rwbuf, idx);

    rwbuf->get_done_idx++;
    if (rwbuf->get_done_idx >= rwbuf->buf_num) rwbuf->get_done_idx = 0;
    *buf = (void*)(rwbuf->buf_addr + idx * rwbuf->buf_size);

    return rwbuf->get_size[idx];
}

static inline int CTK_RWBUF_NAME(_get_buf)(CTK_RWBUF_T *rwbuf, int idx, void **buf) __attribute__((always_inline));
static inline int
CTK_RWBUF_NAME(_get_buf)(CTK_RWBUF_T *rwbuf, int idx, void **buf)
{
    idx += rwbuf->buf_idx;
    if (idx >= rwbuf->buf_num) idx -= rwbuf->buf_num;
    *buf = (void*)(rwbuf->buf_addr + idx * rwbuf->buf_size);
    return rwbuf->get_size[idx];
}

static inline void CTK_RWBUF_NAME(_shift_buf)(CTK_RWBUF_T *rwbuf) __attribute__((always_inline));
static inline void
CTK_RWBUF_NAME(_shift_buf)(CTK_RWBUF_T *rwbuf)
{
    rwbuf->buf_idx++;
    if (rwbuf->buf_idx >= rwbuf->buf_num) rwbuf->buf_idx = 0;
}

#endif /* CTK_RWBUF_READ */

#ifdef CTK_RWBUF_WRITE
/**
 * Starts a DMA put 
 */
static inline void CTK_RWBUF_NAME(_start_put)(CTK_RWBUF_T *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size) __attribute__((always_inline));
static inline void
CTK_RWBUF_NAME(_start_put)(CTK_RWBUF_T *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size)
{
    register qword tmp, tmp2, size_mask, put_mask;
    register int idx = rwbuf->put_req_idx;
    register int tag = ctk_dma_new_tag();
    ea_addr_t ea_addr = rwbuf->out_addr + ea_offset;

    /* determines the transfer size */
    tmp = si_from_uint(ea_offset + size);
    size_mask = si_cgt(si_from_uint(rwbuf->out_size), tmp);
    tmp2 = si_selb(si_from_uint(rwbuf->out_size - ea_offset), 
                   si_from_uint(size), size_mask);
    put_mask = si_cgti(tmp2, 0);
    size = si_to_uint(si_and(tmp2, put_mask));

    DEBUG_PRINTF("[%s] PUT [%d] %x->%llx %d\n", rwbuf->name, idx, 
                    (rwbuf->buf_addr + idx * rwbuf->buf_size), ea_addr, size);
    ctk_dma_put_small((void*)(rwbuf->buf_addr + idx * rwbuf->buf_size),
                    ea_addr, size, tag);
    rwbuf->buf_tags[idx] = tag;

    rwbuf->put_req_idx++;
    if (rwbuf->put_req_idx >= rwbuf->buf_num) rwbuf->put_req_idx = 0;
}

/**
 * Starts a DMA put with an exact size (no boundary check; issue this routine
 * only when you can be sure that issuing that size of get won't cause
 * segv fault.)
 */
static inline void
CTK_RWBUF_NAME(_start_put_exact)(CTK_RWBUF_T *rwbuf, ctk_uint32_t ea_offset, ctk_uint32_t size)
{
    register int idx = rwbuf->put_req_idx;
    register int tag = ctk_dma_new_tag();
    ea_addr_t ea_addr = rwbuf->out_addr + ea_offset;

    DEBUG_PRINTF("[%s] PUT [%d] %x->%x:%x %d\n", rwbuf->name, idx, 
                    (rwbuf->buf_addr + idx * rwbuf->buf_size),
                    ea_hi32(ea_addr), ea_lo32(ea_addr), size);
    ctk_dma_put_small((void*)(rwbuf->buf_addr + idx * rwbuf->buf_size),
                    ea_addr, size, tag);
    rwbuf->buf_tags[idx] = tag;
    rwbuf->put_req_idx++;
    if (rwbuf->put_req_idx >= rwbuf->buf_num) rwbuf->put_req_idx = 0;
}

/**
 * Returns a local buffer addr for the next put
 */
static inline int CTK_RWBUF_NAME(_request_put_buf)(CTK_RWBUF_T *rwbuf, void **buf) __attribute__((always_inline));
static inline int
CTK_RWBUF_NAME(_request_put_buf)(CTK_RWBUF_T *rwbuf, void **buf)
{
    register int idx = rwbuf->put_req_idx;

    /* wait previous transfer if there might be any */
    CTK_RWBUF_NAME(_wait_transfer)(rwbuf, idx);

    *buf = (void*)(rwbuf->buf_addr + rwbuf->put_req_idx * rwbuf->buf_size);
    return rwbuf->buf_size;
}

#endif /* CTK_RWBUF_WRITE */

/**
 * Initializes the rwbuf structure
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
static inline void
CTK_RWBUF_NAME(_init)(CTK_RWBUF_T *rwbuf, const char *name,
      void *buf_addr, ls_size_t buf_size, int buf_num
#ifdef CTK_RWBUF_READ
      , ea_addr_t in_addr, ea_addr_t in_size
#endif /* CTK_RWBUF_READ */
#ifdef CTK_RWBUF_WRITE
      , ea_addr_t out_addr, ea_addr_t out_size
#endif /* CTK_RWBUF_WRITE */
                      )
{
    CTK_DEBUG_ASSERT(buf_num >= 2);
#ifdef DEBUG
    strncpy((char*)rwbuf->name, name, CTK_RWBUF_NAMESIZE);
#endif
    rwbuf->buf_addr = (ls_addr_t)buf_addr;
    rwbuf->buf_end = rwbuf->buf_addr + buf_size * buf_num;
    rwbuf->buf_size = buf_size;
    rwbuf->buf_num = buf_num;
    CTK_RWBUF_NAME(_clear_tags)(rwbuf);

#ifdef CTK_RWBUF_READ
    rwbuf->buf_idx = 0;
    rwbuf->in_addr = in_addr;
    rwbuf->in_size = in_size;
    rwbuf->get_req_idx = rwbuf->get_done_idx = 0;
#endif /* CTK_RWBUF_READ */

#ifdef CTK_RWBUF_WRITE
    rwbuf->out_addr = out_addr;
    rwbuf->out_size = out_size;
    rwbuf->put_req_idx = 0;
#endif /* CTK_RWBUF_WRITE */
}

static inline void CTK_RWBUF_NAME(_reset)(CTK_RWBUF_T *rwbuf) __attribute__((always_inline));
static inline void
CTK_RWBUF_NAME(_reset)(CTK_RWBUF_T *rwbuf)
{
#ifdef CTK_RWBUF_READ
    rwbuf->buf_idx = 0;
    rwbuf->get_req_idx = rwbuf->get_done_idx = 0;
#endif /* CTK_RWBUF_READ */

#ifdef CTK_RWBUF_WRITE
    rwbuf->put_req_idx = 0;
#endif /* CTK_RWBUF_WRITE */
}

/**
 * Waits all the DMA puts which have been issued so far
 */
static inline void CTK_RWBUF_NAME(_wait_all)(CTK_RWBUF_T *rwbuf) __attribute__((always_inline));
static inline void
CTK_RWBUF_NAME(_wait_all)(CTK_RWBUF_T *rwbuf)
{
    DEBUG_PRINTF("[%s] WAIT ALL\n", rwbuf->name);
    unsigned int tag_mask = 0;
#ifdef CTK_RWBUF_LARGE_BUFS
    int i;
    for (i = 0; i < CTK_RWBUF_MAXNUM; i++) {
        CTK_DMA_TAG_SET(rwbuf->buf_tags[i], &tag_mask);
    }
#else /* !CTK_RWBUF_LARGE_BUFS */
    CTK_DMA_TAG_SET(rwbuf->buf_tags[0], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[1], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[2], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[3], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[4], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[5], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[6], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[7], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[8], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[9], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[10], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[11], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[12], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[13], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[14], &tag_mask);
    CTK_DMA_TAG_SET(rwbuf->buf_tags[15], &tag_mask);
#endif /* !CTK_RWBUF_LARGE_BUFS */
    ctk_dma_wait_all(tag_mask);
    CTK_RWBUF_NAME(_clear_tags)(rwbuf);
}
