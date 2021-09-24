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

#ifndef _CTK_ASSERT_H
#define _CTK_ASSERT_H

#include <stdio.h>

/**
 * @file ctk_assert.h
 * @brief Definitions for CTK_ASSERT and CTK_DEBUG_ASSERT macros.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define CTK_ASSERT(_a_) do { \
        int assertErrorA = (_a_); \
        if (!assertErrorA) { \
            printf("%s:%d: Assertion failure: (%s) != TRUE\n",\
                    __FILE__,__LINE__,#_a_); \
            fflush(stdout); \
        } } while (0) 
# define CTK_ASSERT_EXIT(_a_) do { \
        int assertErrorA = (_a_); \
        if (!assertErrorA) { \
            printf("%s:%d: Assertion failure: (%s) != TRUE\n",\
                    __FILE__,__LINE__,#_a_); \
            fflush(stdout); \
            exit (-1); \
        } } while (0) 
# define CTK_ASSERT_RETURN(_a_, _ret_) do { \
        int assertErrorA = (_a_); \
        if (!assertErrorA) { \
            printf("%s:%d: Assertion failure: (%s) != TRUE\n",\
                    __FILE__,__LINE__,#_a_); \
            fflush(stdout); \
            return (_ret_); \
        } } while (0) 

#ifdef DEBUG
# define CTK_DEBUG_ASSERT(_a_)   CTK_ASSERT(_a_)
# define CTK_DEBUG_ASSERT_EXIT(_a_) do { \
        int assertErrorA = (_a_); \
        if (!assertErrorA) { \
            printf("%s:%d: Assertion failure: (%s) != TRUE\n",\
                    __FILE__,__LINE__,#_a_); \
            fflush(stdout); \
            exit (-1); \
        } } while (0) 
# define CTK_DEBUG_ASSERT_RETURN(_a_, _ret_) do { \
        int assertErrorA = (_a_); \
        if (!assertErrorA) { \
            printf("%s:%d: Assertion failure: (%s) != TRUE\n",\
                    __FILE__,__LINE__,#_a_); \
            fflush(stdout); \
            return (_ret_); \
        } } while (0) 
#else /* !DEBUG */
# define CTK_DEBUG_ASSERT(_a_) 
# define CTK_DEBUG_ASSERT_EXIT(_a_)
# define CTK_DEBUG_ASSERT_RETURN(_a_, _ret_)
#endif /* !DEBUG */

// Helper macros to generate unique names (the uniqness is assured 
// only if those macros are placed in different lines)
#define CTK_UNIQ_NAME_HELPER(name, id) name ## id
#define CTK_MAKE_UNIQ_NAME(name, id)   CTK_UNIQ_NAME_HELPER(name, id)
#define CTK_UNIQ_NAME(name)            CTK_MAKE_UNIQ_NAME(name, __LINE__)

// Compile-time assertion (errorMsg must be a valid C++ identifier name)
// e.g. COMPILETIME_ASSERT(sizeof(bool) == 1, Your_bool_is_too_big);
#ifndef CTK_COMPILETIME_ASSERT
# define CTK_COMPILETIME_ASSERT(condition, errorMsg) \
    struct CTK_UNIQ_NAME(StaticAssert) { unsigned int errorMsg: condition; }
#endif

#ifdef __cplusplus
}
#endif

#endif /* _CTK_ASSERT_H */
