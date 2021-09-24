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

#ifndef _CTK_SPU_COMMON_HPP
#define _CTK_SPU_COMMON_HPP

/**
 * @file SpuCommon.hpp
 * @brief CTK++ common include file
 */
#include "ctk_spu_common.h"

#include "../Preprocessor.hpp"
#include "../StaticAssert.hpp"
#include "../Aligned.hpp"
#include "../AlignedArray.hpp"
#include "../AlignedBase.hpp"
#include "../Profile.hpp"

#include "../EAObject.hpp"
#include "../Mutex.hpp"
#include "../Spinlock.hpp"
#include "../Semaphore.hpp"
#include "../Barrier.hpp"

#include "Spu.hpp"
#include "Mfc.hpp"

// let's include this again
#include "ctk_spu_newlib.h"

#endif /* _CTK_SPU_COMMON_HPP */
