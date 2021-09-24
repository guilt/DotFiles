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

#ifndef _CTK_SPU_GENERICQ_H
#define _CTK_SPU_GENERICQ_H

/**
 * @file ctk_spu_genericq.h
 * @brief Definitions for SPU-side queue operations
 */
#include "ctk_spu_mutex.h"
#include "ctk_spu_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

/** queue item reference type */
typedef ea_addr_t ctk_genericq_item_ea_t;

/**
 * queue body structure
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
} ctk_genericq_t;

/** queue reference type */
typedef ea_addr_t ctk_genericq_ea_t;

#include "ctk_spu_genericq-impl.h"

/**
 * Enqueues an item to the given queue object.  This could <b>block</b>
 * if the queue is full.
 * @param[in] queue - the queue reference
 * @param[in] item - the item to be enqueued
 */
void ctk_genericq_enq(ctk_genericq_ea_t queue, void *item);

/**
 * Dequeues an item from the given queue object.  This could <b>block</b>
 * if the queue is empty.
 * @param[in] queue - the queue reference
 * @param[out] item - pointer to store the dequeued item (the memory should
 *    be allocated by the application)
 */
void ctk_genericq_deq(ctk_genericq_ea_t queue, void *item);

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
 * @param[out] item - pointer to store the peeked item (the memory should
 *    be allocated by the application)
 * @return 0 (CTK_SUCCESS) if the queue is non-empty and an item is 
 *  successfully peeked, errno (CTK_ERROR_NO_DATA) otherwise.
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

#endif /* _CTK_SPU_GENERICQ_H */
