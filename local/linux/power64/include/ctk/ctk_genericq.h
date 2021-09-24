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

#ifndef _CTK_GENERICQ_H
#define _CTK_GENERICQ_H

/**
 * @file ctk_genericq.h
 * @brief Definitions for PPU-side array-queue (a queue backed by an array)
 *   operations.
 * @note If the size of the queue (# of items the queue can accomodate)
 *   is less than 127, consider to use of ctk_queue instead.
 * @note Store only small objects in the queue; for large objects, you should
 *   consider storing only references (EA addr) of them and outlining
 *   the data.
 */
#include "ctk_mutex.h"
#include "ctk_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

/** queue item reference type */
typedef ea_addr_t ctk_genericq_item_ea_t;

/**
 * queue structure
 */
typedef struct {
    unsigned int lock;
    unsigned int count;
    unsigned int capacity;
    unsigned int rp;
    unsigned int wp;
    ctk_genericq_item_ea_t items_ea;
    unsigned int item_size;
    unsigned char dummy[] __attribute__((aligned(128)));
} ctk_genericq_t __attribute((aligned(128)));

/** queue reference type */
typedef ea_addr_t ctk_genericq_ea_t;

/** A macro to get an queue reference for the given queue.
 * @param[in] queue - an queue variable (ctk_genericq_t)
 */
#define CTK_GENERICQ_EA(queue)  ((ctk_genericq_ea_t)(unsigned long)(&queue))

/** A macro to get an queue variable from the given queue reference.
 * @param[in] queue_ea - an queue reference (ctk_genericq_ea_t)
 */
#define CTK_GENERICQ_VAR(queue_ea)  (*(ctk_genericq_t*)(unsigned long)(queue_ea))

/** A macro to get an queue pointer from the given queue reference.
 * @param[in] queue_ea - an queue reference (ctk_genericq_ea_t)
 */
#define CTK_GENERICQ_PTR(queue_ea)  ((ctk_genericq_t*)(unsigned long)(queue_ea))

#include "ctk_genericq-impl.h"

/**
 * Creates and initializes a new queue object. The created queue object
 * must be destroyed by #ctk_genericq_destroy to release the allocated
 * resources.
 * @param[out] queue - a pointer to the queue reference
 * @param[in] item_size - the size of the item
 * @param[in] capacity - the size of the queue (# of items it accomodates)
 */
int ctk_genericq_create(ctk_genericq_ea_t *queue, int item_size, int capacity);

/**
 * Destroys the given queue object.
 * @param[in] queue - the queue reference to be freed
 */
int ctk_genericq_destroy(ctk_genericq_ea_t queue);

/**
 * Enqueues an item to the given queue object.  This could <b>block</b> if the
 * queue is full (i.e. the current item count == capacity).
 * @param[in] queue - the queue reference
 * @param[in] item - the item to be enqueued
 */
int ctk_genericq_enq(ctk_genericq_ea_t queue, void *item);

/**
 * Dequeues an item from the given queue object.  This could <b>block</b>
 * if the queue is empty.
 * @param[in] queue - the queue reference
 * @param[out] item - the dequeued item
 */
int ctk_genericq_deq(ctk_genericq_ea_t queue, void *item);

/**
 * Tries to enqueue an item to the given queue object.  If the queue
 * is full, the call does not block and returns #CTK_ERROR_RETRY.
 * @param[in] queue - the queue reference
 * @param[in] item - the item to be enqueued
 * @return 0 (CTK_SUCCESS) if the item is successfully enqueued, 
 *   errno (CTK_ERROR_RETRY or others) on failure.
 */
int ctk_genericq_tryenq(ctk_genericq_ea_t queue, void *item);

/**
 * Tries to dequeue an item to the given queue object.  If the queue
 * is empty, the call does not block and returns #CTK_ERROR_RETRY.
 * @param[in] queue - the queue reference
 * @param[out] item - the dequeued item
 * @return 0 (CTK_SUCCESS) if the item is successfully enqueued, 
 *   errno (CTK_ERROR_RETRY or others) on failure.
 */
int ctk_genericq_trydeq(ctk_genericq_ea_t queue, void *item);

/**
 * Peeks an item from the given queue object.  The queue itself isn't
 * modified.
 * @param[in] queue - the queue reference
 * @param[out] item - the peeked item
 * @return 0 (CTK_SUCCESS) if the queue is non-empty and an item is successfully
 *  peeked, errno (CTK_ERROR_NO_DATA) otherwise.
 */
int ctk_genericq_peek(ctk_genericq_ea_t queue, void *item);

/** Returns the number of additional elements that the given queue
    can accept without blocking. */
static inline int ctk_genericq_get_remaining_capacity(ctk_genericq_ea_t queue) {
    return _ctk_genericq_get_remaining_capacity(queue);
}

/** Returns the number elements in the given queue. */
static inline int ctk_genericq_get_remaining_count(ctk_genericq_ea_t queue) {
    return _ctk_genericq_get_remaining_count(queue);
}

/** Returns the capacity (size) of the given queue */
static inline int ctk_genericq_get_capacity(ctk_genericq_ea_t queue) {
    return _ctk_genericq_get_capacity(queue);
}

#ifdef __cplusplus
}
#endif

#endif /* _CTK_GENERICQ_H */
