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

#ifndef _CTK_SIMPLE_LIST_H
#define _CTK_SIMPLE_LIST_H

/**
 * @file ctk_simple_list.h
 * @brief simple single-linked list utility (no-lock, no thread safe)
 */
#include "ctk_types.h"

/**
 * list structure
 */
typedef struct ctk_simple_list {
    /*
    ea_addr_t head;
    ea_addr_t tail;
     */
    struct ctk_simple_list_item *head;
    struct ctk_simple_list_item *tail;
} ctk_simple_list_t;

/**
 * list item structure ('abstract' structure; the user-defined list item
 * structure must have the 'type' and 'next' member variables as the 
 * first and second elements of the structure)
 */
typedef struct ctk_simple_list_item {
    unsigned int type;
    ea_addr_t next;
} ctk_simple_list_item_t;

/**
 * list static initializer
 */
#define CTK_SIMPLE_LIST_INITIALIZER   { NULL, NULL }

/**
 * Initializes the given list
 */
static inline void ctk_simple_list_init(ctk_simple_list_t *list) {
    (list)->head = (list)->tail = NULL;
}

/**
 * Returns true if the given list is empty
 */
static inline bool ctk_simple_list_is_empty(ctk_simple_list_t *list) {
    return ((list)->head == NULL);
}

#define _CTK_LIST_ITEM_NEXT(x) \
  ((ctk_simple_list_item_t*)(unsigned long)(((ctk_simple_list_item_t*)(x))->next))

#define ctk_simple_list_head(l, type)  ((type)((l)->head))
#define ctk_simple_list_tail(l, type)  ((type)((l)->tail))

#define ctk_simple_list_item_next(x, type) \
  ((type)(unsigned long)(((ctk_simple_list_item_t*)(x))->next))

#define ctk_simple_list_iterate(l, type, x) \
     for ((x) = (type)(l)->head; (x) != NULL; (x) = (type)_CTK_LIST_ITEM_NEXT(x))

#define __ctk_simple_list_iterate(l, x) \
     for ((x) = (l)->head; (x) != NULL; (x) = _CTK_LIST_ITEM_NEXT(x))

/**
 * Prepend an item to the given list
 */
static inline int ctk_simple_list_add_head(ctk_simple_list_t *list, void *item) {
    if (list == NULL)
        return CTK_ERROR_NULL_OBJECT;
    if (list->tail == NULL)
        list->tail = (ctk_simple_list_item_t *)item;
    ((ctk_simple_list_item_t *)item)->next = EAADDR(list->head);
    list->head = (ctk_simple_list_item_t*)item;
    return CTK_SUCCESS;
}

/**
 * Append an item to the given list
 */
static inline int ctk_simple_list_add_tail(ctk_simple_list_t *list, void *item) {
    if (list == NULL)
        return CTK_ERROR_NULL_OBJECT;
    ((ctk_simple_list_item_t*)item)->next = 0;
    if (list->tail == NULL)
        list->head = (ctk_simple_list_item_t *)item;
    else
        list->tail->next = EAADDR(item);
    list->tail = (ctk_simple_list_item_t *)item;
    return CTK_SUCCESS;
}

/**
 * Removes the first item from the given list and return the item
 */
static inline int ctk_simple_list_remove_head(ctk_simple_list_t *list, void **item)
{
    ctk_simple_list_item_t *tmp;
    if (list == NULL)
        return CTK_ERROR_NULL_OBJECT;
    if ((tmp = list->head) != NULL &&
        (list->head = _CTK_LIST_ITEM_NEXT(tmp)) == NULL)
        list->tail = NULL;  
    if (item != NULL)
        *item = (void*)tmp;
    return CTK_SUCCESS;
}

/**
 * Removes the last item from the given list and return the item
 */
static inline int ctk_simple_list_remove_tail(ctk_simple_list_t *list, void **item) {
    ctk_simple_list_item_t *tmp;
    if (list == NULL)
        return CTK_ERROR_NULL_OBJECT;
    __ctk_simple_list_iterate(list, tmp) {
        if (tmp != NULL && tmp->next != 0 && _CTK_LIST_ITEM_NEXT(tmp)->next == 0)
        {
            list->tail = tmp;
            tmp = _CTK_LIST_ITEM_NEXT(tmp);
            list->tail->next = 0;
            break;
        }
    }
    if (item != NULL)
        *item = (void*)tmp;
    return CTK_SUCCESS;
}

/**
 * Removes the specified item from the given list
 */
static inline int ctk_simple_list_remove(ctk_simple_list_t *list, void *item) {
    register ctk_simple_list_item_t *tmp;
    if (list == NULL || item == NULL)
        return CTK_ERROR_NULL_OBJECT;
    if ((tmp = list->head) == (ctk_simple_list_item_t *)item) {
        ctk_simple_list_remove_head(list, NULL);
    } else if (tmp != NULL) {
        for ( ; tmp->next != 0; tmp = _CTK_LIST_ITEM_NEXT(tmp)) {
            if (tmp->next == EAADDR(item)) {
                if ((tmp->next = _CTK_LIST_ITEM_NEXT(tmp)->next) == 0)
                    list->tail = tmp;
                else if (tmp->next == 0)
                    list->tail = _CTK_LIST_ITEM_NEXT(tmp);
                break;
            }
        }
    }
    return CTK_SUCCESS;
}

/**
 * Look up an item from the given list
 */
#define ctk_simple_list_find(l, type, field, key, x) \
do { \
    ctk_simple_list_iterate(l, type, x) {  if ((x)->field == (key)) break; } \
    if ((x) == NULL || (x)->field != (key)) (x) = NULL; \
} while (0)

/**
 * Look up an item from the given list
 */
#define ctk_simple_list_str_find(l, type, field, key, x) \
do { \
    ctk_simple_list_iterate(l, type, x) {  if (!strcmp((x)->field, (key))) break; } \
    if ((x) == NULL || strcmp((x)->field, (key))) (x) = NULL; \
} while (0)

#define ctk_simple_list_find_remove(l, type, field, key, x) \
do { \
    if (((x) = (type)(l)->head) != NULL && (x)->field == (key)) { \
        if (((l)->head = _CTK_LIST_ITEM_NEXT(x)) == NULL) \
        (l)->tail = (ctk_simple_list_item_t *)(x); \
    } else if ((x) != NULL) { \
        for( ; (x)->next != 0; (x) = _CTK_LIST_ITEM_NEXT(x)) { \
            if ((type)(x)->next->field == (key)) { \
                register ctk_simple_list_item_t *tmp = _CTK_LIST_ITEM_NEXT(x); \
                if (((x)->next = _CTK_LIST_ITEM_NEXT(x)->next) == 0) \
                    (l)->tail = (ctk_simple_list_item_t *)(x); \
                else if (_CTK_LIST_ITEM_NEXT(x)->next == 0)\
                    (l)->tail = _CTK_LIST_ITEM_NEXT(x); \
                (x) = (type)tmp; \
                break; \
            } \
        } \
    } \
    if ((x)->field != (key)) (x) = NULL; \
} while (0)


#endif /* _CTK_SIMPLE_LIST_H */
