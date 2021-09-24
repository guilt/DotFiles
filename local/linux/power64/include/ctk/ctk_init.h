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

#ifndef _CTK_INIT_H
#define _CTK_INIT_H

/**
 * @file ctk_init.h
 * @brief Definitions for CTK initialization.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SPU__

/** the program name (argv[0]) */
extern char *ctk_command_name;

/** timebase clock */
extern ctk_uint32_t ctk_timebase_clock;

/**
 * CTK initialization routine.
 * Call this function before using any other
 * @param[in,out] argc - pointer to the argument counter
 * @param[in,out] argv - pointer to the argument string array
 */
void ctk_init(int *argc, char ***argv);

/**
 * CTK finalization routine.
 */
void ctk_destroy();

#endif /* !__SPU__ */

#ifdef __cplusplus
}
#endif

#endif /* _CTK_INIT_H */
