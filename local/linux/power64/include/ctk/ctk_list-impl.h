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

#ifndef _CTK_LIST_IMPL_H
#define _CTK_LIST_IMPL_H

#ifndef _CTK_LIST_H
# error do not include this file directly!
#endif

#include <string.h>
#include <pthread.h>
#include "ctk_error.h"
#include "ctk_mutex.h"

#define _CTK_LIST_MAGIC (0xce11)

static inline int _ctk_list_destroy(ctk_list_ea_t list)
{
    ctk_list_t *l;
    if (list == 0)
        return CTK_ERROR_NULL_OBJECT;
    l = CTK_LIST_PTR(list);
    ctk_sync_free_align(l);
    return CTK_SUCCESS;
}

static inline int _ctk_list_create(ctk_list_ea_t *list, int item_size)
{
    ctk_list_t *l;
    l = (ctk_list_t*)ctk_sync_alloc_align(CTK_SYNC_LIST, 
                        sizeof(ctk_list_t), 128);
    if (l == NULL)
        return CTK_ERROR_NO_MEMORY;
    memset((void*)l, 0, sizeof(ctk_list_t));
    l->head = l->tail = 0;
    l->item_count = 0;
    l->item_size = item_size;
    *list = CTK_LIST_EA(*l);
    return CTK_SUCCESS;
}

static inline int _ctk_list_init(ctk_list_ea_t list, int item_size)
{
    ctk_list_t *l = CTK_LIST_PTR(list);
    if (l == NULL)
        return CTK_ERROR_NO_MEMORY;
    memset((void*)l, 0, sizeof(ctk_list_t));
    l->head = l->tail = 0;
    l->item_count = 0;
    l->item_size = item_size;
    return CTK_SUCCESS;
}

static inline int _ctk_list_add_head(ctk_list_ea_t list, void *item) {
    ctk_list_t *l = CTK_LIST_PTR(list);
    if (list == 0)
        return CTK_ERROR_NULL_OBJECT;
    _ctk_mutex_lock(list);
    if (((ctk_list_item_t*)item)->backing == 0)
        ((ctk_list_item_t*)item)->backing = EAADDR(item);
    if (l->tail == 0)
        l->tail = EAADDR(item);
    ((ctk_list_item_t*)item)->next = l->head;
    l->head = EAADDR(item);
    l->item_count++;
    _ctk_mutex_unlock(list);
    return CTK_SUCCESS;
}

static inline int _ctk_list_add_tail(ctk_list_ea_t list, void *item) {
    ctk_list_t *l = CTK_LIST_PTR(list);
    if (list == 0)
        return CTK_ERROR_NULL_OBJECT;
    _ctk_mutex_lock(list);
    ((ctk_list_item_t*)item)->next = 0;
    ((ctk_list_item_t*)item)->backing = EAADDR(item);
    if (l->tail == 0)
        l->head = EAADDR(item);
    else
        CTK_LIST_ITEM_PTR(l->tail)->next = EAADDR(item);
    l->tail = EAADDR(item);
    l->item_count++;
    _ctk_mutex_unlock(list);
    return CTK_SUCCESS;
}

static inline int _ctk_list_remove_head(ctk_list_ea_t list, void **item) {
    ctk_list_item_t *tmp;
    ctk_list_t *l = CTK_LIST_PTR(list);
    if (list == 0)
        return CTK_ERROR_NULL_OBJECT;
    if (l->item_count == 0) {
        return CTK_ERROR_NO_DATA;
    }
    _ctk_mutex_lock(list);
    tmp = CTK_LIST_ITEM_PTR(l->head);
    if (tmp != NULL) {
        l->item_count--;
        if ((l->head = tmp->next) == 0)
            l->tail = 0;
    }
    _ctk_mutex_unlock(list);
    if (item != NULL) {
        *item = (void*)tmp;
        __asm__ __volatile__ ("eieio");
    }
    return CTK_SUCCESS;
}

#define CTK_LIST_ITEM_NEXT(x)   CTK_LIST_ITEM_PTR(CTK_LIST_ITEM_PTR(x)->next)
#define CTK_LIST_ITEM_NEXT_NEXT(x)   CTK_LIST_ITEM_NEXT(CTK_LIST_ITEM_NEXT(x))

/* PPU only */
static inline int _ctk_list_remove(ctk_list_ea_t list, void *item) {
    ctk_list_item_t *tmp;
    ctk_list_t *l = CTK_LIST_PTR(list);
    bool found = false;
    if (list == 0)
        return CTK_ERROR_NULL_OBJECT;
    _ctk_mutex_lock(list);
    if ((tmp = CTK_LIST_ITEM_PTR(l->head)) == (ctk_list_item_t*)item) {
        _ctk_mutex_unlock(list);
        _ctk_list_remove_head(list, NULL);
        found = true;
    } else if (tmp != NULL) {
        for ( ; tmp->next != 0; tmp = CTK_LIST_ITEM_NEXT(tmp)) {
            if (tmp->next == EAADDR(item)) {
                if ((tmp->next = CTK_LIST_ITEM_NEXT(tmp)->next) == 0) 
                    l->tail = EAADDR(tmp);
                else if (tmp->next == 0)
                    l->tail = CTK_LIST_ITEM_PTR(tmp)->next;
                found = true;
                break;
            }
        }
    }
    _ctk_mutex_unlock(list);
    if (!found) return CTK_ERROR_NOT_FOUND;
    return CTK_SUCCESS;
}

static inline int _ctk_list_get_remaining_count(ctk_list_ea_t list) {
    return (CTK_LIST_PTR(list)->item_count);
}

#endif /* _CTK_LIST_IMPL_H */
