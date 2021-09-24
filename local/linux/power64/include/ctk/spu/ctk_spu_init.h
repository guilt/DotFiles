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

#ifndef _CTK_SPU_INIT_H
#define _CTK_SPU_INIT_H

/**
 * @file ctk_spu_init.h
 * @brief Definitions for SPU initialization routine
 */
/** the Timebase clock */
#if !defined(UNIT_TEST) && !defined(CTK_SPU_RUNTIME)
extern long ctk_timebase_clock;
#else /* UNIT_TEST */
static long ctk_timebase_clock = (long)DEFAULT_TIMEBASE;
#endif /* UNIT_TEST */

/**
 * A flag indicating whether we're during profiling or not.
 */
#if !defined(UNIT_TEST) && !defined(CTK_SPU_RUNTIME)
extern char ctk_do_profiling;
#else /* UNIT_TEST */
static char ctk_do_profiling = 0;
#endif /* UNIT_TEST */

#include "ctk_spu_env.h"
#include "../ctk_env.h"

#endif /* _CTK_SPU_INIT_H */
