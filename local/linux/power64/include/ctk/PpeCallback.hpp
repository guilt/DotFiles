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

#ifndef _CTK_PPE_CALLBACK_HPP
#define _CTK_PPE_CALLBACK_HPP

#include "ctk_ppe_callback.h"
#include "Common.hpp"
#include "Preprocessor.hpp"
#include "PpeCallbackArg.tpp"
#include <list>

namespace ctk {

/**
 * Declares and registers a PPE callback function.
 * @param func - the function name
 * @param callid - an integer ID for the callback function (must be 1 to 255)
 * @param rettype - return type
 * @param types - parenthized argument types for the function (ex. "(int, int)")
 */
#define CTK_CALLBACK_REGISTER(func, callid, rettype, types)     \
    int CTK_PP_CAT(_ctk_ppe_callback_, func)(char *ls, unsigned long op) { \
        return ctk::PpeCallback<rettype types>::invoke(func, ls, op); \
    } \
    struct CTK_PP_CAT(_ctk_ppe_callback_invoker_, func) { \
        CTK_PP_CAT(_ctk_ppe_callback_invoker_, func)() { \
            ::ctk_spe_register_callback(CTK_PP_CAT(_ctk_ppe_callback_, func), callid); \
        } \
    }; \
    CTK_PP_CAT(_ctk_ppe_callback_invoker_, func) CTK_PP_CAT(_invoker_, func);

template <typename R>
class PpeCallback;

#define CTK_CALLBACK_NUM_ARGS     CTK_PP_ITERATION
#define CTK_PP_ITERATION_PARAMS   (0, 20, "PpeCallback.tpp")
#include CTK_PP_ITERATION_INCLUDE

#undef LS_ARG_ADDR
#undef PUT_LS_RC
#undef GET_LS_PTR

}; // namespace ctk

#endif /* _CTK_PPE_CALLBACK_HPP */
