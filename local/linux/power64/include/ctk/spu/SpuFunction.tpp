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

/*
 * Internal templates for SpuFunction.
 */
#if !defined(_CTK_SPU_FUNCTION_HPP) && !defined(CTK_PP_ITERATION)
# error do not include this file directly!
#endif

#include <vector>

/* for SpuFunction params/args */
#if CTK_FUNCTION_NUM_ARGS == 0
# define CTK_FUNCTION_COMMA
#else
# define CTK_FUNCTION_COMMA ,
#endif

#define CTK_FUNCTION_PARM(n, x)      CTK_PP_CAT(T, n) CTK_PP_CAT(a, n)
#define CTK_FUNCTION_VAR(n, x)       typename SpuFunctionArg<CTK_PP_CAT(T, n)>::declare_type CTK_PP_CAT(a, n)
#define CTK_FUNCTION_DEMARSHAL(n, x) SpuFunctionArg<CTK_PP_CAT(T, n)>::demarshal(argp, CTK_PP_CAT(a, n), outbufp, cleanup_addrs, cleanup_sizes)
#define CTK_FUNCTION_CLEANUP(n, x)   SpuFunctionArg<CTK_PP_CAT(T, n)>::cleanup(CTK_PP_CAT(a, n), cleanup_addrs, cleanup_sizes)
#define CTK_FUNCTION_OUTLINE_ARG_SIZE(n, x)  SpuFunctionArg<CTK_PP_CAT(T, n)>::outline_size
#define CTK_FUNCTION_PUT_ARG(n, x)      SpuFunctionArg<CTK_PP_CAT(T, n)>::need_cleanup_ptr

#define CTK_FUNCTION_TEMPLATE_PARMS  CTK_PP_ENUM_PARMS(CTK_FUNCTION_NUM_ARGS, typename T)
#define CTK_FUNCTION_TEMPLATE_ARGS   CTK_PP_ENUM_PARMS(CTK_FUNCTION_NUM_ARGS, T)
#define CTK_FUNCTION_PARMS           CTK_PP_REPEAT(CTK_FUNCTION_NUM_ARGS, CTK_FUNCTION_PARM, CTK_PP_COMMA, )
#define CTK_FUNCTION_ARGS            CTK_PP_ENUM_PARMS(CTK_FUNCTION_NUM_ARGS, a)
#define CTK_FUNCTION_VARS            CTK_PP_REPEAT(CTK_FUNCTION_NUM_ARGS, CTK_FUNCTION_VAR, CTK_PP_SEMICOLON, )
#define CTK_FUNCTION_DEMARSHAL_ARGS  CTK_PP_REPEAT(CTK_FUNCTION_NUM_ARGS, CTK_FUNCTION_DEMARSHAL, CTK_PP_SEMICOLON, )
#define CTK_FUNCTION_CLEANUP_ARGS    CTK_PP_REPEAT(CTK_FUNCTION_NUM_ARGS, CTK_FUNCTION_CLEANUP, CTK_PP_SEMICOLON, )
#define CTK_FUNCTION_OUTLINE_ARGS_BUFSIZ     CTK_PP_REPEAT(CTK_FUNCTION_NUM_ARGS, CTK_FUNCTION_OUTLINE_ARG_SIZE, CTK_PP_PLUS, )
#define CTK_FUNCTION_PUT_ARGS    CTK_PP_REPEAT(CTK_FUNCTION_NUM_ARGS, CTK_FUNCTION_PUT_ARG, CTK_PP_PLUS, )

/* implementation */
template <typename R CTK_FUNCTION_COMMA CTK_FUNCTION_TEMPLATE_PARMS>
class SpuFunction <R (CTK_FUNCTION_TEMPLATE_ARGS)> {
        typedef R (&functor_type)(CTK_FUNCTION_PARMS);
        typedef R (*functor_ptr_type)(CTK_FUNCTION_PARMS);
        CTK_FUNCTION_VARS;

        // inline and outline buffers
        char __arg_buffer[CTK_FUNCTION_PARMS_BUFSIZ]
            __attribute__((aligned(128)));
        char __outline_buffer[CTK_FUNCTION_OUTLINE_ARGS_BUFSIZ + 1]
            __attribute__((aligned(128)));

        // for cleanups 
        ea_addr_t __cleanup_addrs[CTK_FUNCTION_PUT_ARGS + 1];
        int __cleanup_sizes[CTK_FUNCTION_PUT_ARGS + 1];

    public:
        R __invoke(functor_type func, ea_addr_t argAddr) {
            functor_ptr_type functor = &func;
            char *argp, *outbufp;
            ea_addr_t *cleanup_addrs;
            int *cleanup_sizes;
            argp = __arg_buffer;          // inline parameter buffer
            outbufp = __outline_buffer;   // outline buffer
            cleanup_addrs = __cleanup_addrs;
            cleanup_sizes = __cleanup_sizes;
            ::ctk_dma_get_small_block(argp, argAddr, 128);
            CTK_FUNCTION_DEMARSHAL_ARGS;
            ::ctk_dma_wait(0);

            R ret = functor(CTK_FUNCTION_ARGS);

            cleanup_addrs = __cleanup_addrs;
            cleanup_sizes = __cleanup_sizes;
            CTK_FUNCTION_CLEANUP_ARGS;
            ::ctk_dma_wait(0);
            return (R)ret;
        }
};

template <CTK_FUNCTION_TEMPLATE_PARMS>
class SpuFunction <void (CTK_FUNCTION_TEMPLATE_ARGS)> {
        typedef void (&functor_type)(CTK_FUNCTION_PARMS);
        typedef void (*functor_ptr_type)(CTK_FUNCTION_PARMS);
        CTK_FUNCTION_VARS;

        // inline and outline buffers
        char __arg_buffer[CTK_FUNCTION_PARMS_BUFSIZ]
            __attribute__((aligned(128)));
        char __outline_buffer[CTK_FUNCTION_OUTLINE_ARGS_BUFSIZ + 1]
            __attribute__((aligned(128)));

        // for cleanups 
        ea_addr_t __cleanup_addrs[CTK_FUNCTION_PUT_ARGS + 1];
        int __cleanup_sizes[CTK_FUNCTION_PUT_ARGS + 1];

    public:
        void __invoke(functor_type func, ea_addr_t argAddr) {
            functor_ptr_type functor = &func;
            char *argp, *outbufp;
            ea_addr_t *cleanup_addrs;
            int *cleanup_sizes;
            argp = __arg_buffer;          // inline parameter buffer
            outbufp = __outline_buffer;   // outline buffer
            cleanup_addrs = __cleanup_addrs;
            cleanup_sizes = __cleanup_sizes;
            ::ctk_dma_get_small_block(argp, argAddr, 128);
            CTK_FUNCTION_DEMARSHAL_ARGS;
            ::ctk_dma_wait(0);

            functor(CTK_FUNCTION_ARGS);

            cleanup_addrs = __cleanup_addrs;
            cleanup_sizes = __cleanup_sizes;
            CTK_FUNCTION_CLEANUP_ARGS;
            ::ctk_dma_wait(0);
        }
};
#undef CTK_FUNCTION_COMMA
#undef CTK_FUNCTION_PARM
#undef CTK_FUNCTION_VAR
#undef CTK_FUNCTION_DEMARSHAL
#undef CTK_FUNCTION_CLEANUP
#undef CTK_FUNCTION_OUTLINE_ARG_SIZE
#undef CTK_FUNCTION_PUT_ARG
#undef CTK_FUNCTION_DEMARSHAL_ARGS
#undef CTK_FUNCTION_CLEANUP_ARGS
#undef CTK_FUNCTION_TEMPLATE_PARMS
#undef CTK_FUNCTION_TEMPLATE_ARGS
#undef CTK_FUNCTION_PARMS
#undef CTK_FUNCTION_VARS
#undef CTK_FUNCTION_ARGS
#undef CTK_FUNCTION_OUTLINE_ARGS_BUFSIZ
#undef CTK_FUNCTION_PUT_ARGS
#undef CTK_FUNCTION_DECLARE_CONSTRUCTOR
#undef CTK_FUNCTION_DECLARE_CONSTRUCTORS
