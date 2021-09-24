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

#ifndef _CTK_SPU_TASK_H
#define _CTK_SPU_TASK_H

#if defined(UNIT_TEST) || defined(_CTKLIB)
# include "ctk_spu_task_syscalls.h"
#else
# include "ctk/spu/ctk_spu_task_syscalls.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __SPU__

/* Forbid to call _exit */
static void __ctk_task_exit() __attribute__((destructor));
static void __ctk_task_exit() {
    ctk_task_exit(0);
}

/**
 * The SPE task main function.  The task application must implement this.
 * @param[in] id - the SPE task ID
 * @param[in] arg1 - the runtime arg given by the caller
 * @param[in] arg2 - the runtime arg given by the caller
 */
int ctk_task_main(unsigned long long id, unsigned long long arg1, unsigned long long arg2) /*__attribute__((flatten, always_inline))*/;

/*-------------------------------------------------------------------------
 * XXX: we should be in a .c file
 */
int main(unsigned long long arg1, unsigned long long arg2, unsigned long long arg3)
{
    int id = ctk_task_init_env(arg1);
    int ret = ctk_task_main((unsigned long long)id, arg2, arg3);
    return ctk_task_exit(ret);
}

#define ctk_task_exit(ret)      return ctk_task_exit(ret);

#endif /* __SPU__ */

#ifdef __cplusplus
};
#endif

#endif /* _CTK_SPU_TASK_H */
