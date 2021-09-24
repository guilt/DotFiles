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

#ifndef _CTK_PS3FB_H
#define _CTK_PS3FB_H

/**
 * @file ctk_ps3fb.h
 * @brief Utilities for PS3 Framebuffer
 */

#define CTK_PS3FB_BYTES_PER_PIXEL  4
#define CTK_PS3FB_DEV_PATH         "/dev/fb0"

/**
 * PS3 Screen info structure
 */
typedef struct {
    unsigned int xres;	/**< X resolution */
    unsigned int yres;	/**< Y resolution */
    unsigned int xoff;	/**< X offset */
    unsigned int yoff;	/**< Y offset */
    unsigned int num_frames;	/**< # of frames */
} ctk_ps3screen_t;

/**
 * Opens and initializes the PS3 Framebuffer device.  This also
 * gets the controll of FB by stopping the flipping in the kernel
 * (i.e. by calling #ctk_ps3fb_get()).
 * This must be called in prior to calling any other PS3FB routines.
 * Typical call sequence would look like this:
 * <pre>
 *  ctk_ps3fb_open();
 *
 *  ctk_ps3screen_t screen;
 *  ctk_ps3fb_get_screen_info(&screen);
 *  // set initial parameters by using screen sizes
 *
 *  while (1) {
 *      // gets the current buffer frame
 *      void *fb_buf = ctk_ps3fb_get_current_frame();
 *      
 *      // fill the fb_buf memory region
 *      
 *      // displays the filled buffer and advances the frame
 *      ctk_ps3fb_advance_current_frame();
 *  }
 *
 *  ctk_ps3fb_close();
 * </pre>
 * @return 0 (CTK_SUCCESS) on success, errno on failure.
 */
int ctk_ps3fb_open();

/**
 * Closes the PS3 Framebuffer device.
 * (Normally this would be called automatically when the program exit)
 */
void ctk_ps3fb_close();

/**
 * Takes the control of FB by stopping the ps3fbd flipping.
 * @return 0 (CTK_SUCCESS) on success, errno on failure.
 */
int ctk_ps3fb_get();

/**
 * Releases the FB control by re-starting the ps3fbd flipping.
 * @return 0 (CTK_SUCCESS) on success, errno on failure.
 */
int ctk_ps3fb_release();

/**
 * Clears the frame buffer.
 * @return 0 (CTK_SUCCESS) on success, errno on failure.
 */
int ctk_ps3fb_clear();

/**
 * Waits the next VSync.
 * @return 0 (CTK_SUCCESS) on success, errno on failure.
 */
int ctk_ps3fb_wait_vsync();

/**
 * Returns the # of available frame count.
 * @return the number of frames (>0) on success, -errno (<0) on failure.
 */
int ctk_ps3fb_get_frame_count();

/**
 * Transfers the frame buffer in the user space and flips the frame.
 * @return 0 (CTK_SUCCESS) on success, errno on failure.
 */
int ctk_ps3fb_flip(unsigned int frame);

/**
 * Returns the screen information.
 * @return 0 (CTK_SUCCESS) on success, errno on failure.
 */
int ctk_ps3fb_get_screen_info(ctk_ps3screen_t *screen);

/**
 * Returns the screen size.
 * @return 0 (CTK_SUCCESS) on success, errno on failure.
 */
int ctk_ps3fb_get_screen_size(unsigned int *xres, unsigned int *yres, unsigned int *xoff, unsigned int *yoff);

/**
 * Returns the FB memory region for the frame buffer specified 
 * by the given #frame.
 * @return non-null addr on success, NULL on failure.
 */
void *ctk_ps3fb_get_frame(unsigned int frame);

/**
 * Returns the FB memory region for the current frame.
 * (This internally calls #ctk_ps3fb_wait_vsync() and
 * ctk_ps3fb_get_frame().)
 * @return non-null addr on success, NULL on failure.
 */
void *ctk_ps3fb_get_current_frame();

/**
 * Displays (sets) the current frame (returned by ctk_ps3fb_
 * get_current_frame()) and advances the current frame number.
 * (This internally calls #ctk_ps3fb_flip().)
 * @return 0 (CTK_SUCCESS) on success, errno on failure.
 */
int ctk_ps3fb_advance_current_frame();

#endif /* _CTK_PS3FB_H */
