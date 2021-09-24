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
 * Internal templates for PpeCallback.
 */
#if !defined(_CTK_PPE_CALLBACK_HPP) && !defined(CTK_PP_ITERATION)
# error do not include this file directly!
#endif

#if CTK_CALLBACK_NUM_ARGS == 0
# define CTK_CALLBACK_COMMA
#else
# define CTK_CALLBACK_COMMA ,
#endif

#define CTK_CALLBACK_PARM(n, x)      CTK_PP_CAT(T, n) CTK_PP_CAT(a, n)
#define CTK_CALLBACK_VAR(n, x)       CTK_PP_CAT(T, n) CTK_PP_CAT(a, n) = PpeCallbackArg<CTK_PP_CAT(T, n)>::get_value(ls, CTK_PP_CAT(arg, n))
#define CTK_CALLBACK_DECL_ARG(n, x)  struct spe_reg128 CTK_PP_CAT(*arg, n) = LS_ARG_ADDR(n)

#define CTK_CALLBACK_TEMPLATE_PARMS  CTK_PP_ENUM_PARMS(CTK_CALLBACK_NUM_ARGS, typename T)
#define CTK_CALLBACK_TEMPLATE_ARGS   CTK_PP_ENUM_PARMS(CTK_CALLBACK_NUM_ARGS, T)
#define CTK_CALLBACK_PARMS           CTK_PP_REPEAT(CTK_CALLBACK_NUM_ARGS, CTK_CALLBACK_PARM, CTK_PP_COMMA, )
#define CTK_CALLBACK_ARGS            CTK_PP_ENUM_PARMS(CTK_CALLBACK_NUM_ARGS, a)
#define CTK_CALLBACK_VARS            CTK_PP_REPEAT(CTK_CALLBACK_NUM_ARGS, CTK_CALLBACK_VAR, CTK_PP_SEMICOLON, )
#define CTK_CALLBACK_DECL_ARGS       CTK_PP_REPEAT(CTK_CALLBACK_NUM_ARGS, CTK_CALLBACK_DECL_ARG, CTK_PP_SEMICOLON, )

template <typename R CTK_CALLBACK_COMMA CTK_CALLBACK_TEMPLATE_PARMS>
struct PpeCallback <R (CTK_CALLBACK_TEMPLATE_ARGS)> {
    typedef R (&functor_type)(CTK_CALLBACK_PARMS);
    static int invoke(functor_type func, char *ls, unsigned long opdata) {
        unsigned int ls_args = (opdata & 0xffffff);
        CTK_CALLBACK_DECL_ARGS;
        struct spe_reg128 *ret = LS_ARG_ADDR(0);
        CTK_CALLBACK_VARS;
        R rc = func(CTK_CALLBACK_ARGS);
        PpeCallbackArg<R>::put_rc(ret, rc);
        return 0;
    }
};

#undef CTK_CALLBACK_COMMA
#undef CTK_CALLBACK_PARM
#undef CTK_CALLBACK_VAR
#undef CTK_CALLBACK_DECL_ARG
#undef CTK_CALLBACK_TEMPLATE_PARMS
#undef CTK_CALLBACK_TEMPLATE_ARGS
#undef CTK_CALLBACK_PARMS
#undef CTK_CALLBACK_ARGS
#undef CTK_CALLBACK_VARS
#undef CTK_CALLBACK_DECL_ARGS
