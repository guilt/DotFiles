/*
 * Twin - A Tiny Window System
 * Copyright © 2004 Keith Packard <keithp@keithp.com>
 * All rights reserved.
 *
 * This Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the Twin Library; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _TWIN_X11_H_
#define _TWIN_X11_H_

#include <libtwin/twin.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

typedef struct _twin_x11 {
    twin_screen_t   *screen;
    Display	    *dpy;
    Window	    win;
    GC		    gc;
    Visual	    *visual;
    int		    depth;
    XImage	    *image;
    int		    image_y;
} twin_x11_t;

/*
 * twin_x11.c 
 */

twin_x11_t *
twin_x11_create (Display *dpy, int width, int height);

void
twin_x11_destroy (twin_x11_t *tx);

void
twin_x11_damage (twin_x11_t *tx, XExposeEvent *ev);

void
twin_x11_configure (twin_x11_t *tx, XConfigureEvent *ev);

void
twin_x11_update (twin_x11_t *tx);

#endif /* _TWIN_X11_H_ */
