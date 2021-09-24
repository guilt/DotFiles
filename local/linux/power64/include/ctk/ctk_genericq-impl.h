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

#ifndef _CTK_GENERICQ_IMPL_H
#define _CTK_GENERICQ_IMPL_H

#ifndef _CTK_GENERICQ_H
# error do not include this file directly!
#endif

#include <string.h>
#include "ctk_error.h"
#include "ctk_mutex.h"

#ifndef DEBUG_PRINTF
#define DEBUG_PRINTF(...)
#endif

static inline int _ctk_genericq_destroy(ctk_genericq_ea_t queue)
{
    ctk_genericq_t *q;
    if (queue == 0)
        return CTK_ERROR_NULL_OBJECT;
    q = CTK_GENERICQ_PTR(queue);
    if (q->items_ea != 0)
        ctk_sync_free_align((void *)(unsigned long)q->items_ea);
    ctk_sync_free_align(q);
    return CTK_SUCCESS;
}

static inline int _ctk_genericq_create(ctk_genericq_ea_t *queue, int item_size, int capacity)
{
    int size = ALIGN_UP(item_size * capacity, 128);
    ctk_genericq_t *q;
    q = (ctk_genericq_t*)ctk_sync_alloc_align(CTK_SYNC_GENERICQ, 
            sizeof(ctk_genericq_t), 128);
    if (q == NULL)
        return CTK_ERROR_NO_MEMORY;
    memset((void*)q, 0, sizeof(ctk_genericq_t));
    q->items_ea = (ea_addr_t)(unsigned long)
                        ctk_sync_alloc_align(CTK_SYNC_GENERICQ, size, 128);
    if (q->items_ea == 0)
        return CTK_ERROR_NO_MEMORY;
    q->capacity = capacity;
    q->count = 0;
    q->item_size = item_size;
    *queue = CTK_GENERICQ_EA(*q);
    return CTK_SUCCESS;
}

static inline int _ctk_genericq_enq(ctk_genericq_ea_t queue, void *item) {
    ctk_genericq_t *q = CTK_GENERICQ_PTR(queue);
    char *items = (char*)(unsigned long)q->items_ea;
    _ctk_mutex_lock(queue);
    while (q->count == q->capacity) {
        _ctk_mutex_unlock(queue);
        sched_yield();
        _ctk_mutex_lock(queue);
    }
    memcpy(&items[(q->wp++)*(q->item_size)], item, q->item_size);
    if (q->wp == q->capacity) q->wp = 0;
    q->count++;
    _ctk_mutex_unlock(queue);
    return CTK_SUCCESS;
}

static inline int _ctk_genericq_deq(ctk_genericq_ea_t queue, void *item) {
    ctk_genericq_t *q = CTK_GENERICQ_PTR(queue);
    char *items = (char*)(unsigned long)q->items_ea;
    if (queue == 0)
        return CTK_ERROR_NULL_OBJECT;
    _ctk_mutex_lock(queue);
    while (q->count == 0) {
        _ctk_mutex_unlock(queue);
        sched_yield();
        _ctk_mutex_lock(queue);
    }
    memcpy(item, &items[(q->rp++)*(q->item_size)], q->item_size);
    if (q->rp == q->capacity) q->rp = 0;
    q->count--;
    _ctk_mutex_unlock(queue);
    return CTK_SUCCESS;
}

static inline int _ctk_genericq_tryenq(ctk_genericq_ea_t queue, void *item) {
    ctk_genericq_t *q = CTK_GENERICQ_PTR(queue);
    char *items = (char*)(unsigned long)q->items_ea;
    _ctk_mutex_lock(queue);
    if (q->count == q->capacity) {
        _ctk_mutex_unlock(queue);
        return CTK_ERROR_RETRY;
    }
    memcpy(&items[(q->wp++)*(q->item_size)], item, q->item_size);
    if (q->wp == q->capacity) q->wp = 0;
    q->count++;
    _ctk_mutex_unlock(queue);
    return CTK_SUCCESS;
}

static inline int _ctk_genericq_trydeq(ctk_genericq_ea_t queue, void *item) {
    ctk_genericq_t *q = CTK_GENERICQ_PTR(queue);
    char *items = (char*)(unsigned long)q->items_ea;
    if (queue == 0)
        return CTK_ERROR_NULL_OBJECT;
    _ctk_mutex_lock(queue);
    while (q->count == 0) {
        _ctk_mutex_unlock(queue);
        return CTK_ERROR_RETRY;
    }
    memcpy(item, &items[(q->rp++)*(q->item_size)], q->item_size);
    if (q->rp == q->capacity) q->rp = 0;
    q->count--;
    _ctk_mutex_unlock(queue);
    return CTK_SUCCESS;
}

static inline int _ctk_genericq_peek(ctk_genericq_ea_t queue, void *item) {
    ctk_genericq_t *q = CTK_GENERICQ_PTR(queue);
    char *items = (char*)(unsigned long)q->items_ea;
    _ctk_mutex_lock(queue);
    if (q->count == 0) {
        _ctk_mutex_unlock(queue);
        return CTK_ERROR_NO_DATA;
    }
    memcpy(item, &items[q->rp * q->item_size], q->item_size);
    _ctk_mutex_unlock(queue);
    return CTK_SUCCESS;
}

static inline int _ctk_genericq_get_remaining_capacity(ctk_genericq_ea_t queue) {
    ctk_genericq_t *q = CTK_GENERICQ_PTR(queue);
    return q->capacity - q->count;
}

static inline int _ctk_genericq_get_remaining_count(ctk_genericq_ea_t queue) {
    return (CTK_GENERICQ_PTR(queue)->count);
}

/** Returns the capacity (size) of the given queue */
static inline int _ctk_genericq_get_capacity(ctk_genericq_ea_t queue) {
    return (CTK_GENERICQ_PTR(queue)->capacity);
}

#endif /* _CTK_GENERICQ_IMPL_H */
