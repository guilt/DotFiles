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

#ifndef _CTK_IMAGE_H
#define _CTK_IMAGE_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file ctk_image.h
 * @brief Definitions for handling generic 2d images (mainly for testings).
 * @note This module does not have any relationship to Cell.
 */

/**
 * RGB structure
 */
typedef struct {
    u_char r;
    u_char g;
    u_char b;
} ctk_rgb_t;

/**
 * Generic RGB image structure (ZPixmap type)
 */
typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned int xoffset;
    unsigned int pixels_per_line;
    unsigned int bytes_per_pixel;
    unsigned int data_size;
    unsigned char data_aligned_p;
    ctk_rgb_t *data;
} ctk_image_t;

/**
 * Creates a new RGB image with given parameters.
 * @param[in] img - pointer of the image structure.
 * @param[in] w - the image width
 * @param[in] h - the image height
 * @param[in] xoffset - number of pixels offset in X direction
 * @param[in] pixels_per_line - # of pixels per line (-1 to use default (= w))
 * @param[in] bytes_per_pixel - # of bytes per pixel (-1 to use default (= 3))
 * @param[in] data_size - size of the image buffer (-1 to use default)
 * @return CTK_SUCCESS (0) if succeeded, errno on failure.
 */
int ctk_image_create(ctk_image_t* img, int w, int h, int xoffset, int pixels_per_line, int bytes_per_pixel, int data_size);

/**
 * Creates a new RGB image with <b>aligned</b> image buffer.
 * @param[in] img - pointer of the image structure.
 * @param[in] w - the image width
 * @param[in] h - the image height
 * @param[in] xoffset - number of pixels offset in X direction
 * @param[in] pixels_per_line - # of pixels per line (-1 to use default (= w))
 * @param[in] bytes_per_pixel - # of bytes per pixel (-1 to use default (= 3))
 * @param[in] data_size - size of the image buffer (-1 to use default)
 * @param[in] align - an alignment size (must be power of 2).
 *           The allocated buffer size will be multiple of this value.
 * @return CTK_SUCCESS (0) if succeeded, errno on failure.
 */
int ctk_image_create_align(ctk_image_t* img, int w, int h, int xoffset, int pixels_per_line, int bytes_per_pixel, int data_size, int align);

/**
 * Destroys the image buffer.
 * @return CTK_SUCCESS (0) if succeeded, errno on failure.
 */
int ctk_image_destroy(ctk_image_t *img);

#ifdef __cplusplus
};
#endif

#endif /* _CTK_IMAGE_H */
