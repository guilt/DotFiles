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
#if !defined(_CTK_SPU_PPE_CALLBACK_HPP) && !defined(CTK_PP_ITERATION)
# error do not include this file directly!
#endif

#if CTK_CALLBACK_NUM_ARGS == 0
# define CTK_CALLBACK_COMMA
#else
# define CTK_CALLBACK_COMMA ,
#endif

#define CTK_CALLBACK_PARM(n, x)      CTK_PP_CAT(T, n) CTK_PP_CAT(a, n)
#define CTK_CALLBACK_ARG_ARG(n, x)   union { CTK_PP_CAT(T, n) CTK_PP_CAT(a, n); unsigned int CTK_PP_CAT(pad, n)[4]; }
#define CTK_CALLBACK_SETUP_ARG(n, x)   CTK_PP_CAT(args.a, n) = CTK_PP_CAT(a, n)

#define CTK_CALLBACK_TEMPLATE_PARMS  CTK_PP_ENUM_PARMS(CTK_CALLBACK_NUM_ARGS, typename T)
#define CTK_CALLBACK_TEMPLATE_ARGS   CTK_PP_ENUM_PARMS(CTK_CALLBACK_NUM_ARGS, T)
#define CTK_CALLBACK_ARG_ARGS        CTK_PP_REPEAT(CTK_CALLBACK_NUM_ARGS, CTK_CALLBACK_ARG_ARG, CTK_PP_SEMICOLON, )
#define CTK_CALLBACK_SETUP_ARGS      CTK_PP_REPEAT(CTK_CALLBACK_NUM_ARGS, CTK_CALLBACK_SETUP_ARG, CTK_PP_SEMICOLON, )
#define CTK_CALLBACK_PARMS           CTK_PP_REPEAT(CTK_CALLBACK_NUM_ARGS, CTK_CALLBACK_PARM, CTK_PP_COMMA, )

template <typename R CTK_CALLBACK_COMMA CTK_CALLBACK_TEMPLATE_PARMS>
class PpeCallback <R (CTK_CALLBACK_TEMPLATE_ARGS)> {
    public: 
        int m_callid;
        PpeCallback(int callid) : m_callid(callid) { }

        R operator ()(CTK_CALLBACK_PARMS) {
            int id = m_callid;  // for spu-g++ workaround
            struct { CTK_CALLBACK_ARG_ARGS; } args;
            CTK_CALLBACK_SETUP_ARGS;
            ::ctk_callback_send_to_ppe(id, &args);
            return *(R*)&args;
        }
};

#undef CTK_CALLBACK_COMMA
#undef CTK_CALLBACK_PARM
#undef CTK_CALLBACK_PARMS
#undef CTK_CALLBACK_ARG_ARG
#undef CTK_CALLBACK_ARG_ARGS
#undef CTK_CALLBACK_TEMPLATE_PARMS
#undef CTK_CALLBACK_TEMPLATE_ARGS
#undef CTK_CALLBACK_SETUP_ARG
#undef CTK_CALLBACK_SETUP_ARGS
