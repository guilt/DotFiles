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

#ifndef _CTK_SPU_QUEUE_H
#ifndef _CTK_QUEUE_TEMPLATE
# define _CTK_SPU_QUEUE_H
#endif /* _CTK_QUEUE_TEMPLATE */

/**
 * @file ctk_spu_queue.h
 * @brief Definitions for SPU-side queue operations
 */
#include "ctk_spu_mutex.h"
#include "ctk_spu_sem.h"

#if defined(__cplusplus) && !defined(_CTK_QUEUE_TEMPLATE)
extern "C" {
#endif

#ifndef _CTK_QITEM_SIZE
# define _CTK_QITEM_SIZE 8
#endif /* _CTK_QITEM_SIZE */

/** Queue item type */
#if (_CTK_QITEM_SIZE == 1)
typedef ctk_uint8_t ctk_queue_item_t;
#elif (_CTK_QITEM_SIZE == 2)
typedef ctk_uint16_t ctk_queue_item_t;
#elif (_CTK_QITEM_SIZE == 4)
typedef ctk_uint32_t ctk_queue_item_t;
#elif (_CTK_QITEM_SIZE == 8)
typedef ctk_uint64_t ctk_queue_item_t;  /* default */
#else
typedef char ctk_queue_item_t[_CTK_QITEM_SIZE];
#endif

/** queue item reference type */
typedef ea_addr_t ctk_queue_item_ea_t;

/**
 * queue body structure
 */
typedef struct {
    volatile int wcount:8;     /**< counter for writers */
    volatile int wp:8;         /**< write pointer */
    volatile int rcount:8;     /**< counter for readers */
    volatile int rp:8;         /**< read pointer */
    ctk_queue_item_ea_t items_ea;
    int capacity;    /**< # of items this queue can accomodate */
    unsigned char dummy[] __attribute__((aligned(128)));
} ctk_queue_t;

/** queue reference type */
typedef ea_addr_t ctk_queue_ea_t;

#include "ctk_spu_queue-impl.h"

/**
 * Enqueues an item to the given queue object.  This could <b>block</b>
 * if the queue is full.
 * @param[in] queue - the queue reference
 * @param[in] item - the item to be enqueued
 */
void ctk_queue_enq(ctk_queue_ea_t queue, ctk_queue_item_t item);

/**
 * Dequeues an item from the given queue object.  This could <b>block</b>
 * if the queue is empty.
 * @param[in] queue - the queue reference
 * @param[out] item - pointer to store the dequeued item (the memory should
 *    be allocated by the application)
 */
void ctk_queue_deq(ctk_queue_ea_t queue, ctk_queue_item_t *item);

/**
 * Tries to enqueue an item to the given queue object.  If the queue
 * is full, the call does not block and returns #CTK_ERROR_RETRY.
 * @param[in] queue - the queue reference
 * @param[in] item - the item to be enqueued
 * @return 0 (CTK_SUCCESS) if the item is successfully enqueued, 
 *   errno (CTK_ERROR_RETRY or others) on failure.
 */
int ctk_queue_tryenq(ctk_queue_ea_t queue, ctk_queue_item_t item);

/**
 * Tries to dequeue an item to the given queue object.  If the queue
 * is empty, the call does not block and returns #CTK_ERROR_RETRY.
 * @param[in] queue - the queue reference
 * @param[out] item - the dequeued item
 * @return 0 (CTK_SUCCESS) if the item is successfully enqueued, 
 *   errno (CTK_ERROR_RETRY or others) on failure.
 */
int ctk_queue_trydeq(ctk_queue_ea_t queue, ctk_queue_item_t *item);

/**
 * Peeks an item from the given queue object.  The queue itself isn't
 * modified.
 * @param[in] queue - the queue reference
 * @param[out] item - pointer to store the peeked item (the memory should
 *    be allocated by the application)
 * @return 0 (CTK_SUCCESS) if the queue is non-empty and an item is 
 *  successfully peeked, errno (CTK_ERROR_NO_DATA) otherwise.
 */
int ctk_queue_peek(ctk_queue_ea_t queue, ctk_queue_item_t *item);

/** Returns the capacity (size) of the given queue */
static inline int ctk_queue_get_capacity(ctk_queue_ea_t queue) {
    return _ctk_queue_get_capacity(queue);
}

/** Returns the number of additional elements that the given queue
    can accept without blocking. */
static inline int ctk_queue_get_remaining_capacity(ctk_queue_ea_t queue) {
    return _ctk_queue_get_remaining_capacity(queue);
}

/** Returns the number elements in the given queue. */
static inline int ctk_queue_get_remaining_count(ctk_queue_ea_t queue) {
    return _ctk_queue_get_remaining_count(queue);
}

#if defined(__cplusplus) && !defined(_CTK_QUEUE_TEMPLATE)
}
#endif

#endif /* _CTK_SPU_QUEUE_H */
