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

#ifndef _CTK_PPM_H
#define _CTK_PPM_H

/**
 * @file ctk_ppm.h
 * @brief Definitions for handling PPM files with various alignment
 *  options. (mainly for testings)
 * @note This module does not have any relationship to Cell.
 */

#define CTK_PPM_P3 "P3"
#define CTK_PPM_P6 "P6"

#include "ctk_image.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reads the PPM(P6) image from the input file specified by #fname,
 * allocates a image buffer in the given image structure and
 * fills it.
 * @param[in] img - pointer of the image structure to be filled.
 *  The image buffer in the image structure will be allocated by the callee.
 * @param[in] fname - the file name to be read.
 * @return CTK_SUCCESS (0) on success, errno otherwise.
 */
int ctk_ppm_p6_read(ctk_image_t *img, const char *fname);

/**
 * Reads the PPM(P6) image from the input file specified by #fname,
 * allocates an aligned image buffer in the given image structure and
 * fills it.
 * @param[in] img - pointer of the image structure to be filled.
 *  The image buffer in the image structure will be allocated by the callee.
 * @param[in] fname - the file name to be read.
 * @param[in] xoffset - number of pixels offset in X direction
 * @param[in] align_of_line - an alignment for line size (must be power of 2).
 * @param[in] align_of_pixel - an alignment for pixel size (musg be power of 2).
 * @param[in] align_of_buf - an alignment for buffer size (must be power of 2).
 *             The allocated buffer size will be multiple of this value.
 * @return CTK_SUCCESS (0) on success, errno otherwise.
 */
int ctk_ppm_p6_read_align(ctk_image_t *img, const char *fname, int xoffset, int align_of_line, int bytes_per_pixel, int align_of_buf);

/**
 * Writes out the PPM(P6) image to the output file specified by #fname.
 * @param[in] img - the image structure to be written.
 * @param[in] fname - the file name to be written.
 * @return CTK_SUCCESS (0) on success, errno otherwise.
 */
int ctk_ppm_p6_write(ctk_image_t* img, const char* fname);

#ifdef __cplusplus
};
#endif

#endif /* _CTK_PPM_H */
