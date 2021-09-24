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

#ifndef _CTK_CTK_SPU_FUNCTION_HPP
#define _CTK_CTK_SPU_FUNCTION_HPP

#if defined(UNIT_TEST) || (_CTKLIB)
# include "SpuCommon.hpp"
# include "../../task/spu/ctk_spu_task_syscalls.h"
# include "SpuFunction.hpp"
# include "../../base/spu/ctk_spu_newlib.h"
#else
# include <CtkSpu.hpp>
# include "ctk/spu/ctk_spu_task_syscalls.h"
# include "ctk/spu/SpuFunction.hpp"
# include "ctk/spu/ctk_spu_newlib.h"
#endif

template <typename T>
inline int ctkFunctionExit(T ret) { return ctk_task_exit(*(ctk_uint64_t*)&ret); }

inline int ctkFunctionExit() { return ctk_task_exit(0); }
inline int ctkFunctionExit(vec_char16 ret) { return ctk_task_exit_128(*(qword*)&ret); }
inline int ctkFunctionExit(vec_uchar16 ret) { return ctk_task_exit_128(*(qword*)&ret); }
inline int ctkFunctionExit(vec_ushort8 ret) { return ctk_task_exit_128(*(qword*)&ret); }
inline int ctkFunctionExit(vec_short8 ret) { return ctk_task_exit_128(*(qword*)&ret); }
inline int ctkFunctionExit(vec_uint4 ret) { return ctk_task_exit_128(*(qword*)&ret); }
inline int ctkFunctionExit(vec_int4 ret) { return ctk_task_exit_128(*(qword*)&ret); }
inline int ctkFunctionExit(vec_ullong2 ret) { return ctk_task_exit_128(*(qword*)&ret); }
inline int ctkFunctionExit(vec_llong2 ret) { return ctk_task_exit_128(*(qword*)&ret); }
inline int ctkFunctionExit(vec_float4 ret) { return ctk_task_exit_128(*(qword*)&ret); }
inline int ctkFunctionExit(vec_double2 ret) { return ctk_task_exit_128(*(qword*)&ret); }

/**
 * Declares a SPE function.
 * @param func - the function name
 * @param rettype - return type
 * @param types - parenthized argument types for the function (ex. "(int, int)")
 */
#define CTK_FUNCTION_DECLARE(func, rettype, types)       \
    ctk::SpuFunction<rettype types> __invoker; \
    int main(unsigned long long runtime_envp, unsigned long long arg, unsigned long long unused) \
    { \
        ctk_task_init_env(runtime_envp); \
        ctkFunctionExit(__invoker.__invoke(func, arg)); \
        return 0; \
    }
    

#endif /* _CTK_CTK_SPU_FUNCTION_HPP */
