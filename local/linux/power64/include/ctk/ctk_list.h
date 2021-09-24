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

#ifndef _CTK_LIST_H
#define _CTK_LIST_H

/**
 * @file ctk_list.h
 * @brief Definitions for PPU-side simple linked list module 
 * (thread-safe version, just a very naive implementation)
 */
#include "ctk_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * list item structure ('abstract' structure; the user-defined list item
 * structure must have the 'next' and 'backing' member variables
 * (of ea_addr_t) as the first elements of the structure.)
 */
typedef struct ctk_list_item {
    ea_addr_t next;     /**< the 'next' item of this item; automatically
                          maintained when the item is enqueued */
    ea_addr_t backing;  /**< the EA addr of this item; automatically 
                          maintained when the item is enqueued */
} ctk_list_item_t;

/** list item reference type */
typedef ea_addr_t ctk_list_item_ea_t;

/**
 * list structure
 */
typedef struct {
    unsigned int lock;
    ctk_list_item_ea_t head;
    ctk_list_item_ea_t tail;
    unsigned int item_count;
    unsigned int item_size;
    unsigned char pad[0] __attribute__((aligned(128)));
} ctk_list_t __attribute((aligned(128)));

/** list reference type */
typedef ea_addr_t ctk_list_ea_t;

/** A macro to get a list reference for the given list.
 * @param[in] list - a list variable (ctk_list_t)
 */
#define CTK_LIST_EA(list)  ((ctk_list_ea_t)(unsigned long)(&list))

/** A macro to get a list variable from the given list reference.
 * @param[in] list_ea - a list reference (ctk_list_ea_t)
 */
#define CTK_LIST_VAR(list_ea)  (*(ctk_list_t*)(unsigned long)(list_ea))

/** A macro to get a list pointer from the given list reference.
 * @param[in] list_ea - a list reference (ctk_list_ea_t)
 */
#define CTK_LIST_PTR(list_ea)  ((ctk_list_t*)(unsigned long)(list_ea))

/** A macro to get a list item pointer from the given list item reference.
 * @param[in] item_ea - a list item reference (ctk_list_item_ea_t)
 */
#define CTK_LIST_ITEM_PTR(item_ea) \
((ctk_list_item_t*)(unsigned long)(item_ea))

#include "ctk_list-impl.h"

#define ctk_list_iterate(l, type, x) \
  for ((x) = (type)CTK_LIST_ITEM_PTR(CTK_LIST_PTR(l)->head); \
       (x) != NULL; \
       (x) = (type)CTK_LIST_ITEM_PTR(((ctk_list_item_t*)(x))->next))

#define __ctk_list_iterate(l, x) \
  for ((x) = CTK_LIST_ITEM_PTR(CTK_LIST_PTR(l)->head); \
       (x) != NULL; \
       (x) = CTK_LIST_ITEM_PTR(((ctk_list_item_t*)(x))->next))

#define __ctk_list_iterate_ea(l, ea) \
  for ((ea) = CTK_LIST_PTR(l)->head; (ea) != 0;  \
        (ea) = ((ctk_list_item_t*)(ea))->next)

#define ctk_list_next(x, type) \
    (type)CTK_LIST_ITEM_PTR((CTK_LIST_ITEM_PTR(x)->next))
#define ctk_list_next_next(x, type) ctk_list_next(ctk_list_next(x, type), type)

#if EXPORT_LIST
/**
 * Creates and initializes a new list object. The created list object
 * must be destroyed by #ctk_list_destroy to release the allocated
 * resources.
 * @param[out] list - a pointer to the list reference
 * @param[in] item_size - the size of the item
 */
int ctk_list_create(ctk_list_ea_t *list, int item_size);

/**
 * Destroys the given list object.
 * @note This does not frees any queue items.  The management of 
 *   queue item objects must be done by the application.
 * @param[in] list - the list reference to be freed
 */
int ctk_list_destroy(ctk_list_ea_t list);

/**
 * Initializes a new list object.
 * @param[out] list - a pointer to the list reference
 * @param[in] item_size - the size of the item
 */
int ctk_list_init(ctk_list_ea_t list, int item_size);

/**
 * Prepends an item to the given list object.
 * @note DO NOT add an item instance which has already been in the list!!
 * @param[in] list - the list reference
 * @param[in] item - the item to be add.  The item must be of (extended)
 *   ctk_list_item type and have been allocated by the application in prior
 *   to call the method.  The item also must be aligned if the queue is shared
 *   with SPEs.
 */
int ctk_list_add_head(ctk_list_ea_t list, void *item);

/**
 * Appends an item to the given list object.
 * @note DO NOT add an item instance which has already been in the list!!
 * @param[in] list - the list reference
 * @param[in] item - the item to be add.  The item must be of (extended)
 *   ctk_list_item type and have been allocated by the application in prior
 *   to call the method.  The item also must be aligned if the queue is shared
 *   with SPEs.
 */
int ctk_list_add_tail(ctk_list_ea_t list, void *item);

/**
 * Gets an item from the tail of the given list object.
 * @param[in] list - the list reference
 * @param[out] item - pointer to the dequeued item.  
 * @return CTK_SUCCESS (0) on success, CTK_ERROR_NO_DATA if there's no data.
 */
int ctk_list_remove_head(ctk_list_ea_t list, void **item);

/** Returns the number elements in the given list. */
static inline int ctk_list_get_remaining_count(ctk_list_ea_t list) {
    return _ctk_list_get_remaining_count(list);
}
#endif /* EXPORT_LIST */

#ifdef __cplusplus
}
#endif

#endif /* _CTK_LIST_H */
