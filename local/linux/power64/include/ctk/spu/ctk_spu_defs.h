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

#ifndef _CTK_SPU_DEFS_H
#define _CTK_SPU_DEFS_H

/**
 * @file ctk_spu_defs.h
 * @brief Common definitions for CTK SPU headers and sources
 */

#ifndef DEFAULT_TIMEBASE
# ifdef CELL_ENV_TSDK
#  define DEFAULT_TIMEBASE       (1.2e8)        /**< Timebase */
# else
#  define DEFAULT_TIMEBASE       (8.0*1.0e7)    /**< Timebase */
# endif
#endif /* !TIMEBASE */

/* makes a small jitter */
#define DELAY() do { volatile int __z = (ctk_rand() & 0xfff); while (__z--); } while (0)

#ifdef DEBUG
# include <stdio.h>
# ifdef CELL_ENV_TSDK
#  define printf        ctk_printf
# endif /* CELL_ENV_TSDK */
# define DEBUG_PRINTF(...)  do { \
      printf("[%s:%d] ", __FILE__, __LINE__); \
      printf(__VA_ARGS__); \
    } while (0)
# define DBG(x)  do { DEBUG_PRINTF("%s\n", #x); x; } while (0)
#else /* !DEBUG */
# define DEBUG_PRINTF(fmt, ...) 
# define DBG(x)  x
#endif /* !DEBUG */

/** Combine hi32 and lo32 addr into 64-bit addr */
#define hl2ea(_h, _l)   (spu_extract((vector unsigned long long) \
  spu_sel(spu_promote(_h, 0), spu_promote(_l, 1), \
        ((vector unsigned int){0,-1,0,0})), 0))

#ifndef likely
/** indicates the condition _c 'likely' occurs */
# define likely(_c)      __builtin_expect((_c), 1)
/** indicates the condition _c 'unlikely' occurs */
# define unlikely(_c)    __builtin_expect((_c), 0)
#endif

#endif /* _CTK_SPU_DEFS_H */
