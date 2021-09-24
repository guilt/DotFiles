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

#ifndef _CTK_EVENT_H
#define _CTK_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Set 1 to use separate event thread to wait task finish */
#define CTK_USE_EVENT_THREAD    1

/**
 * @file ctk_event.h
 * @brief CTK event main loop (basically this does aggregated busy loop)
 */
#include "ctk_event-internal.h"
typedef ctk_event_entry_body_t* ctk_event_entry_t;

int ctk_event_wait(bool (*check_func)(void*), void *user_data);
/*
int ctk_event_create(ctk_event_entry_t *eventp, bool (*check_func)(void*), 
    void *user_data);
int ctk_event_wait(ctk_event_entry_t event);
int ctk_event_destroy(ctk_event_entry_t event);
*/

#ifdef __cplusplus
}
#endif

#endif /* _CTK_EVENT_H */
