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

#ifndef _CTK_DEFS_H
#define _CTK_DEFS_H

/**
 * @file ctk_defs.h
 * @brief CTK basic defines
 */

#include "ctk_version.h"

#ifndef DEFAULT_TIMEBASE
# ifdef CELL_ENV_TSDK
#  define DEFAULT_TIMEBASE       (1.2e8)        /**< Timebase (XXX) */
# else
#  define DEFAULT_TIMEBASE       (8.0*1.0e7)    /**< Timebase (XXX) */
# endif
#endif /* !TIMEBASE */

#ifndef MAX
# define MAX(a,b)    (((a) > (b)) ? (a) : (b))
# define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#ifndef __SPU__
# ifdef DEBUG
#  include <stdio.h>
#  ifndef DEBUG_FP
#   define DEBUG_FP    stderr
#  endif
/* debug printf enabled only when compiled with -DDEBUG */
#  define DEBUG_PRINTF(...)  do { \
          fprintf(DEBUG_FP, "[%s:%d] ", __FILE__, __LINE__); \
          fprintf(DEBUG_FP, __VA_ARGS__); \
      } while (0)
/* trace printf enabled only when compiled with -DDEBUG and specified -d from 
   the command option */
#  define TRACE_INFO(...)   trace_info(__VA_ARGS__) 
#  define TRACE_DEBUG(...)  trace_debug(__VA_ARGS__) 
#  define TRACE_TRACE(...)  trace_trace(__VA_ARGS__) 
#  define DBG(x)  do { DEBUG_PRINTF("%s\n", #x); x; } while (0)
# else /* !DEBUG */
#  define DEBUG_PRINTF(fmt, ...) 
#  define TRACE_INFO(fmt, ...) 
#  define TRACE_DEBUG(fmt, ...) 
#  define TRACE_TRACE(fmt, ...) 
#  define DBG(x)  x
# endif /* !DEBUG */
#endif /* !SPU */

#endif /* _CTK_DEFS_H */
