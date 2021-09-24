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

#if !defined(_CTK_QUEUE_H) && !defined(_CTK_QUEUE_TEMPLATE)
# error do not include this file directly!
#endif

#include <string.h>
#include "ctk_error.h"
#include "ctk_mutex.h"
#include "ctk_profile.h"

/*#define DEBUG*/

#ifndef DEBUG_PRINTF
#ifdef DEBUG
ctk_profile_t prof;
#define DEBUG_PRINTF(...)  do { \
          printf("[%s:%d] ", __func__, __LINE__); \
          printf(__VA_ARGS__); \
      } while (0)
#else
#define DEBUG_PRINTF(...)
#endif /* DEBUG */
#endif /* DEBUG_PRINTF */

#define _CTK_QVAL_WCOUNT(_qv)   ((_qv) >> 24)
#define _CTK_QVAL_WP(_qv)       (((_qv) >> 16) & 0xff)
#define _CTK_QVAL_RCOUNT(_qv)   (((_qv) >> 8) & 0xff)
#define _CTK_QVAL_RP(_qv)       ((_qv) & 0xff)
#define _CTK_QVAL_SET_WVALS(_qv, _wc, _wp)  \
    (((_wc) << 24) | ((_wp) << 16) | ((_qv) & 0xffff))
#define _CTK_QVAL_SET_VAL(_wc, _wp, _rc, _rp)  \
    (((_wc) << 24) | ((_wp) << 16) | ((_rc) << 8) | (_rp))
#define _CTK_QVAL_SET_RCOUNT(_qv, _rcount)  \
    (((_qv) & 0xffff00ff) | ((_rcount) << 8))

static inline int _ctk_queue_destroy(ctk_queue_ea_t queue)
{
    ctk_queue_t *q;
    if (queue == 0)
        return CTK_ERROR_NULL_OBJECT;
    q = CTK_QUEUE_PTR(queue);
    if (q->items_ea != 0)
        ctk_sync_free_align((void *)(unsigned long)q->items_ea);
    ctk_sync_free_align(q);
    return CTK_SUCCESS;
}

static inline int _ctk_queue_create(ctk_queue_ea_t *queue, int capacity)
{
    int size = ALIGN_UP(sizeof(ctk_queue_item_t) * capacity, 128);
    ctk_queue_t *q;
    if (capacity > CTK_QUEUE_MAXSIZE) {
        trace_error("The queue size cannot exceed %d.\n", CTK_QUEUE_MAXSIZE);
        return CTK_ERROR_TOO_MANY;
    }
    q = (ctk_queue_t*)ctk_sync_alloc_align(CTK_SYNC_QUEUE, 
            sizeof(ctk_queue_t), 128);
    if (q == NULL)
        return CTK_ERROR_NO_MEMORY;
    memset((void*)q, 0, sizeof(ctk_queue_t));
    q->items_ea = (ea_addr_t)(unsigned long)
                    ctk_sync_alloc_align(CTK_SYNC_QUEUE, size, 128);
    if (q->items_ea == 0) {
        ctk_sync_free_align(q);
        return CTK_ERROR_NO_MEMORY;
    }
    q->capacity = capacity;
    q->rcount = q->wcount = 0;
#ifdef DEBUG
    printf("items: %llx - %llx\n", q->items_ea, q->items_ea + size);
    ctk_profile_start(&prof);
#endif /* DEBUG */
    *queue = CTK_QUEUE_EA(*q);
    return CTK_SUCCESS;
}

static inline int _ctk_queue_enq(ctk_queue_ea_t queue, ctk_queue_item_t item) {
    ctk_queue_t *q = CTK_QUEUE_PTR(queue);
    ctk_queue_item_t *items = (ctk_queue_item_t*)(unsigned long)q->items_ea;
    volatile unsigned int status = 0;
    unsigned int oldqval, newqval;
    int capacity;
    volatile int wcount, rcount, wp;
#ifdef DEBUG
    unsigned int time;
#endif /* DEBUG */

    if (queue == 0)
        return CTK_ERROR_NULL_OBJECT;

    capacity = q->capacity;

    do {
        /* load with reservation */
        __asm__ __volatile__ ("# ctk_queue_enq load \n"
            "lwarx %0, 0, %1 \n\t"
            : "=r" (oldqval)
            : "r" (q)
        );
        wcount = _CTK_QVAL_WCOUNT(oldqval);
        if (wcount >= capacity || wcount != (int)_CTK_QVAL_RCOUNT(oldqval)) {
            sched_yield();
            continue;
        }
        wp = _CTK_QVAL_WP(oldqval);

        /* update the (local) counter values */
        if (++wp == capacity) wp = 0;
        wcount++;
        newqval = _CTK_QVAL_SET_WVALS(oldqval, wcount, wp);

#ifdef DEBUG
        time = ctk_profile_read_in_clock(&prof);
#endif /* DEBUG */
        
        /* store conditional */
        __asm__ __volatile__ ("# ctk_queue_enq store\n\t"
            "li %0, 0\n"
            "stwcx. %2, 0, %1 \n\t"
            "bne 1f \n\t"
            "li %0, 1 # store 1 to status \n"
        "1:	\n"
            "isync\n"
            : "=&r" (status)
            : "r" (q), "r" (newqval)
            : "cr0");

    } while (status == 0);

    /* store the value */
#if (_CTK_QITEM_SIZE > 8)
    memcpy(items[_CTK_QVAL_WP(oldqval)], item, sizeof(ctk_queue_item_t));
#else
    items[_CTK_QVAL_WP(oldqval)] = item;
#endif

    DEBUG_PRINTF("%6d ENQ: %2d %2d, %2d %2d -> %2d %2d, %2d %2d "
                 "(%d:%lld) %lld\n",
        time, 
        _CTK_QVAL_RCOUNT(oldqval), 
        _CTK_QVAL_RP(oldqval), 
        _CTK_QVAL_WCOUNT(oldqval), 
        _CTK_QVAL_WP(oldqval), 
        _CTK_QVAL_RCOUNT(newqval), 
        _CTK_QVAL_RP(oldqval), 
        _CTK_QVAL_WCOUNT(newqval), 
        _CTK_QVAL_WP(newqval),
        _CTK_QVAL_WP(oldqval), item, items[_CTK_QVAL_WP(oldqval)]);

    /* increment the counter for readers */
    do {
        /* load with reservation */
        __asm__ __volatile__ ("# ctk_queue_enq load rcount\n"
            "lwarx %0, 0, %1 \n\t"
            : "=r" (oldqval)
            : "r" (q)
        );
        rcount = _CTK_QVAL_RCOUNT(oldqval);
        if (_CTK_QVAL_WCOUNT(oldqval) - rcount > 1) {
            sched_yield();
            continue;
        }
        rcount++;
        newqval = _CTK_QVAL_SET_RCOUNT(oldqval, rcount);

        /* store conditional */
        __asm__ __volatile__ ("# ctk_queue_enq store rcount\n\t"
            "li %0, 0\n"
            "stwcx. %2, 0, %1 \n\t"
            "bne 1f \n\t"
            "li %0, 1 # store 1 to status \n"
        "1:	\n"
            "isync\n"
            : "=&r" (status)
            : "r" (q), "r" (newqval)
            : "cr0");
    } while (status == 0);

    return CTK_SUCCESS;
}

static inline int _ctk_queue_deq(ctk_queue_ea_t queue, ctk_queue_item_t *item) {
    ctk_queue_t *q = CTK_QUEUE_PTR(queue);
    ctk_queue_item_t *items = (ctk_queue_item_t*)(unsigned long)q->items_ea;
    volatile int status = 0;
    volatile unsigned int oldqval, newqval;
    int capacity;
    volatile int rcount, wcount, rp;
#ifdef DEBUG
    unsigned int time;
#endif /* DEBUG */

    if (queue == 0)
        return CTK_ERROR_NULL_OBJECT;

    capacity = q->capacity;

    do {
        /* load with reservation */
        __asm__ __volatile__ ("# ctk_queue_deq load \n"
            "lwarx %0, 0, %1 \n\t"
            : "=r" (oldqval)
            : "r" (q)
        );

        rcount = _CTK_QVAL_RCOUNT(oldqval);
        if (rcount <= 0) {
            sched_yield();
            continue;
        }

        rp = _CTK_QVAL_RP(oldqval);
        wcount = _CTK_QVAL_WCOUNT(oldqval);

#if (_CTK_QITEM_SIZE > 8)
        memcpy(*item, items[rp], sizeof(ctk_queue_item_t));
#else
        *item = items[rp];
#endif
        if (++rp == capacity) rp = 0;
        rcount--;
        wcount--;
#ifdef DEBUG
        time = ctk_profile_read_in_clock(&prof);
#endif /* DEBUG */

        newqval = _CTK_QVAL_SET_VAL(wcount, _CTK_QVAL_WP(oldqval), rcount, rp);

        /* store conditional */
        __asm__ __volatile__ ("# ctk_queue_deq store\n\t"
            "li %0, 0\n"
            "stwcx. %2, 0, %1 \n\t"
            "bne 1f \n\t"
            "li %0, 1 # store 1 to status \n"
        "1:	\n"
            : "=&r" (status)
            : "r" (q), "r" (newqval)
            : "cr0");
    } while (status == 0);

    DEBUG_PRINTF("%6d DEQ: %2d %2d, %2d %2d -> %2d %2d, %2d %2d "
                 "(%d:%lld) %lld\n",
        time, 
        _CTK_QVAL_RCOUNT(oldqval), 
        _CTK_QVAL_RP(oldqval), 
        _CTK_QVAL_WCOUNT(oldqval), 
        _CTK_QVAL_WP(oldqval), 
        _CTK_QVAL_RCOUNT(newqval), 
        _CTK_QVAL_RP(oldqval), 
        _CTK_QVAL_WCOUNT(newqval), 
        _CTK_QVAL_WP(newqval),
        _CTK_QVAL_RP(oldqval), *item, items[_CTK_QVAL_RP(oldqval)]);

    return CTK_SUCCESS;
}

static inline int _ctk_queue_tryenq(ctk_queue_ea_t queue, ctk_queue_item_t item)
{
    ctk_queue_t *q = CTK_QUEUE_PTR(queue);
    ctk_queue_item_t *items = (ctk_queue_item_t*)(unsigned long)q->items_ea;
    volatile unsigned int status = 0;
    unsigned int oldqval, newqval;
    int capacity;
    volatile int wcount, rcount, wp;
#ifdef DEBUG
    unsigned int time;
#endif /* DEBUG */

    if (queue == 0)
        return CTK_ERROR_NULL_OBJECT;

    capacity = q->capacity;

    do {
        /* load with reservation */
        __asm__ __volatile__ ("# ctk_queue_enq load \n"
            "lwarx %0, 0, %1 \n\t"
            : "=r" (oldqval)
            : "r" (q)
        );
        wcount = _CTK_QVAL_WCOUNT(oldqval);
        if (wcount >= capacity || wcount != (int)_CTK_QVAL_RCOUNT(oldqval)) {
            return CTK_ERROR_RETRY;
        }
        wp = _CTK_QVAL_WP(oldqval);

        /* update the (local) counter values */
        if (++wp == capacity) wp = 0;
        wcount++;
        newqval = _CTK_QVAL_SET_WVALS(oldqval, wcount, wp);

#ifdef DEBUG
        time = ctk_profile_read_in_clock(&prof);
#endif /* DEBUG */
        
        /* store conditional */
        __asm__ __volatile__ ("# ctk_queue_enq store\n\t"
            "li %0, 0\n"
            "stwcx. %2, 0, %1 \n\t"
            "bne 1f \n\t"
            "li %0, 1 # store 1 to status \n"
        "1:	\n"
            "isync\n"
            : "=&r" (status)
            : "r" (q), "r" (newqval)
            : "cr0");

    } while (status == 0);

    /* store the value */
#if (_CTK_QITEM_SIZE > 8)
    memcpy(items[_CTK_QVAL_WP(oldqval)], item, sizeof(ctk_queue_item_t));
#else
    items[_CTK_QVAL_WP(oldqval)] = item;
#endif

    DEBUG_PRINTF("%6d ENQ: %2d %2d, %2d %2d -> %2d %2d, %2d %2d "
                 "(%d:%lld) %lld\n",
        time, 
        _CTK_QVAL_RCOUNT(oldqval), 
        _CTK_QVAL_RP(oldqval), 
        _CTK_QVAL_WCOUNT(oldqval), 
        _CTK_QVAL_WP(oldqval), 
        _CTK_QVAL_RCOUNT(newqval), 
        _CTK_QVAL_RP(oldqval), 
        _CTK_QVAL_WCOUNT(newqval), 
        _CTK_QVAL_WP(newqval),
        _CTK_QVAL_WP(oldqval), item, items[_CTK_QVAL_WP(oldqval)]);

    /* increment the counter for readers */
    do {
        /* load with reservation */
        __asm__ __volatile__ ("# ctk_queue_enq load rcount\n"
            "lwarx %0, 0, %1 \n\t"
            : "=r" (oldqval)
            : "r" (q)
        );
        rcount = _CTK_QVAL_RCOUNT(oldqval);
        if (_CTK_QVAL_WCOUNT(oldqval) - rcount > 1) {
            sched_yield();
            continue;
        }
        rcount++;
        newqval = _CTK_QVAL_SET_RCOUNT(oldqval, rcount);

        /* store conditional */
        __asm__ __volatile__ ("# ctk_queue_enq store rcount\n\t"
            "li %0, 0\n"
            "stwcx. %2, 0, %1 \n\t"
            "bne 1f \n\t"
            "li %0, 1 # store 1 to status \n"
        "1:	\n"
            "isync\n"
            : "=&r" (status)
            : "r" (q), "r" (newqval)
            : "cr0");
    } while (status == 0);

    return CTK_SUCCESS;
}

static inline int _ctk_queue_trydeq(ctk_queue_ea_t queue, ctk_queue_item_t *item) {
    ctk_queue_t *q = CTK_QUEUE_PTR(queue);
    ctk_queue_item_t *items = (ctk_queue_item_t*)(unsigned long)q->items_ea;
    volatile int status = 0;
    volatile unsigned int oldqval, newqval;
    int capacity;
    volatile int rcount, wcount, rp;
#ifdef DEBUG
    unsigned int time;
#endif /* DEBUG */

    if (queue == 0)
        return CTK_ERROR_NULL_OBJECT;

    capacity = q->capacity;

    do {
        /* load with reservation */
        __asm__ __volatile__ ("# ctk_queue_deq load \n"
            "lwarx %0, 0, %1 \n\t"
            : "=r" (oldqval)
            : "r" (q)
        );

        rcount = _CTK_QVAL_RCOUNT(oldqval);
        if (rcount <= 0) {
            return CTK_ERROR_RETRY;
        }

        rp = _CTK_QVAL_RP(oldqval);
        wcount = _CTK_QVAL_WCOUNT(oldqval);

#if (_CTK_QITEM_SIZE > 8)
        memcpy(*item, items[rp], sizeof(ctk_queue_item_t));
#else
        *item = items[rp];
#endif
        if (++rp == capacity) rp = 0;
        rcount--;
        wcount--;
#ifdef DEBUG
        time = ctk_profile_read_in_clock(&prof);
#endif /* DEBUG */

        newqval = _CTK_QVAL_SET_VAL(wcount, _CTK_QVAL_WP(oldqval), rcount, rp);

        /* store conditional */
        __asm__ __volatile__ ("# ctk_queue_deq store\n\t"
            "li %0, 0\n"
            "stwcx. %2, 0, %1 \n\t"
            "bne 1f \n\t"
            "li %0, 1 # store 1 to status \n"
        "1:	\n"
            : "=&r" (status)
            : "r" (q), "r" (newqval)
            : "cr0");
    } while (status == 0);

    DEBUG_PRINTF("%6d DEQ: %2d %2d, %2d %2d -> %2d %2d, %2d %2d "
                 "(%d:%lld) %lld\n",
        time, 
        _CTK_QVAL_RCOUNT(oldqval), 
        _CTK_QVAL_RP(oldqval), 
        _CTK_QVAL_WCOUNT(oldqval), 
        _CTK_QVAL_WP(oldqval), 
        _CTK_QVAL_RCOUNT(newqval), 
        _CTK_QVAL_RP(oldqval), 
        _CTK_QVAL_WCOUNT(newqval), 
        _CTK_QVAL_WP(newqval),
        _CTK_QVAL_RP(oldqval), *item, items[_CTK_QVAL_RP(oldqval)]);

    return CTK_SUCCESS;
}

static inline int _ctk_queue_peek(ctk_queue_ea_t queue, ctk_queue_item_t *item)
{
    ctk_queue_t *q = CTK_QUEUE_PTR(queue);
    ctk_queue_item_t *items = (ctk_queue_item_t*)(unsigned long)q->items_ea;
    volatile unsigned int qval;
    volatile int rcount, rp;

    if (queue == 0)
        return CTK_ERROR_NULL_OBJECT;

    qval = *(unsigned int*)q;
    rcount = _CTK_QVAL_RCOUNT(qval);
    if (rcount <= 0) {
        return CTK_ERROR_NO_DATA;
    }
    rp = _CTK_QVAL_RP(qval);
#if (_CTK_QITEM_SIZE > 8)
    memcpy(*item, items[_CTK_QVAL_WP(qval)], sizeof(ctk_queue_item_t));
#else
    *item = items[rp];
#endif

    return CTK_SUCCESS;
}

static inline int _ctk_queue_get_capacity(ctk_queue_ea_t queue) {
    ctk_queue_t *q = CTK_QUEUE_PTR(queue);
    return q->capacity;
}

static inline int _ctk_queue_get_remaining_capacity(ctk_queue_ea_t queue) {
    ctk_queue_t *q = CTK_QUEUE_PTR(queue);
    return q->capacity - q->wcount;
}

static inline int _ctk_queue_get_remaining_count(ctk_queue_ea_t queue) {
    return (CTK_QUEUE_PTR(queue)->rcount);
}

