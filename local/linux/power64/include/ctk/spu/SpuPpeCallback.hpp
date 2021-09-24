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

#ifndef _CTK_SPU_PPE_CALLBACK_HPP
#define _CTK_SPU_PPE_CALLBACK_HPP

#include "ctk_spu_common.h"
#include "ctk_spu_ppe_callback.h"

namespace ctk {

/**
 * PpeCallback template. 
 * @note This template requires '-On' flag to be compiled with spu-g++ 4.1.1!
 *   (Otherwise spu-g++ may fail with Segv fault.)
 */
template <typename R> class PpeCallback;

#define CTK_CALLBACK_NUM_ARGS     CTK_PP_ITERATION
#define CTK_PP_ITERATION_PARAMS   (0, 20, "spu/SpuPpeCallback.tpp")
#include CTK_PP_ITERATION_INCLUDE

};

#endif /* _CTK_SPU_PPE_CALLBACK_HPP */
