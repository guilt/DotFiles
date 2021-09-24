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

#ifndef _CTK_PREPROCESSOR_H
#define _CTK_PREPROCESSOR_H

/* (not so complete) preprocessor helper macros.
   major code is taken from boost/preprocessor. */

#define CTK_PP_BOOL(x)  CTK_PP_BOOL_ ## x
#define CTK_PP_BOOL_0  0
#define CTK_PP_BOOL_1  1
#define CTK_PP_BOOL_2  1
#define CTK_PP_BOOL_3  1
#define CTK_PP_BOOL_4  1
#define CTK_PP_BOOL_5  1
#define CTK_PP_BOOL_6  1
#define CTK_PP_BOOL_7  1
#define CTK_PP_BOOL_8  1
#define CTK_PP_BOOL_9  1
#define CTK_PP_BOOL_10 1
#define CTK_PP_BOOL_11 1
#define CTK_PP_BOOL_12 1
#define CTK_PP_BOOL_13 1
#define CTK_PP_BOOL_14 1
#define CTK_PP_BOOL_15 1
#define CTK_PP_BOOL_16 1
#define CTK_PP_BOOL_17 1
#define CTK_PP_BOOL_18 1
#define CTK_PP_BOOL_19 1
#define CTK_PP_BOOL_20 1

#define CTK_PP_IF(b, t, f)      CTK_PP_IF_ ## b(t, f)

#define CTK_PP_IF_0(t, f)  f
#define CTK_PP_IF_1(t, f)  t
#define CTK_PP_IF_2(t, f)  t
#define CTK_PP_IF_3(t, f)  t
#define CTK_PP_IF_4(t, f)  t
#define CTK_PP_IF_5(t, f)  t
#define CTK_PP_IF_6(t, f)  t
#define CTK_PP_IF_7(t, f)  t
#define CTK_PP_IF_8(t, f)  t
#define CTK_PP_IF_9(t, f)  t
#define CTK_PP_IF_10(t, f)  t
#define CTK_PP_IF_11(t, f)  t
#define CTK_PP_IF_12(t, f)  t
#define CTK_PP_IF_13(t, f)  t
#define CTK_PP_IF_14(t, f)  t
#define CTK_PP_IF_15(t, f)  t
#define CTK_PP_IF_16(t, f)  t
#define CTK_PP_IF_17(t, f)  t
#define CTK_PP_IF_18(t, f)  t
#define CTK_PP_IF_19(t, f)  t
#define CTK_PP_IF_20(t, f)  t

/* punctuations */
#define CTK_PP_COMMA()          ,
#define CTK_PP_LPAREN()         (
#define CTK_PP_RPAREN()         )
#define CTK_PP_SEMICOLON()      ;
#define CTK_PP_EMPTY()  
#define CTK_PP_PLUS()           +

#define CTK_PP_AND()            &
#define CTK_PP_OR()             |
#define CTK_PP_LAND()           &&
#define CTK_PP_LOR()            ||

#define CTK_PP_INC(x)           x++

/* concatenate */
#define CTK_PP_CAT(a, b)        CTK_PP_CAT_(a, b)
#define CTK_PP_CAT_(a, b)       a ## b

/* simple repetition preprocessor macros */
#define CTK_PP_REPEAT(count, macro, delim, v)      \
        CTK_PP_REPEAT_(count, macro, delim, v)

/* indirection */
#define CTK_PP_REPEAT_(count, macro, delim, v)      \
        CTK_PP_REPEAT_ ## count(macro, delim, v)

#define CTK_PP_REPEAT_0(macro, delim, v)   
#define CTK_PP_REPEAT_1(macro, delim, v)   macro(0, v)
#define CTK_PP_REPEAT_2(macro, delim, v)   CTK_PP_REPEAT_1(macro, delim, v) delim() macro(1, v)
#define CTK_PP_REPEAT_3(macro, delim, v)   CTK_PP_REPEAT_2(macro, delim, v) delim() macro(2, v)
#define CTK_PP_REPEAT_4(macro, delim, v)   CTK_PP_REPEAT_3(macro, delim, v) delim() macro(3, v)
#define CTK_PP_REPEAT_5(macro, delim, v)   CTK_PP_REPEAT_4(macro, delim, v) delim() macro(4, v)
#define CTK_PP_REPEAT_6(macro, delim, v)   CTK_PP_REPEAT_5(macro, delim, v) delim() macro(5, v)
#define CTK_PP_REPEAT_7(macro, delim, v)   CTK_PP_REPEAT_6(macro, delim, v) delim() macro(6, v)
#define CTK_PP_REPEAT_8(macro, delim, v)   CTK_PP_REPEAT_7(macro, delim, v) delim() macro(7, v)
#define CTK_PP_REPEAT_9(macro, delim, v)   CTK_PP_REPEAT_8(macro, delim, v) delim() macro(8, v)
#define CTK_PP_REPEAT_10(macro, delim, v)  CTK_PP_REPEAT_9(macro, delim, v) delim() macro(9, v)
#define CTK_PP_REPEAT_11(macro, delim, v)  CTK_PP_REPEAT_10(macro, delim, v) delim() macro(10, v)
#define CTK_PP_REPEAT_12(macro, delim, v)  CTK_PP_REPEAT_11(macro, delim, v) delim() macro(11, v)
#define CTK_PP_REPEAT_13(macro, delim, v)  CTK_PP_REPEAT_12(macro, delim, v) delim() macro(12, v)
#define CTK_PP_REPEAT_14(macro, delim, v)  CTK_PP_REPEAT_13(macro, delim, v) delim() macro(13, v)
#define CTK_PP_REPEAT_15(macro, delim, v)  CTK_PP_REPEAT_14(macro, delim, v) delim() macro(14, v)
#define CTK_PP_REPEAT_16(macro, delim, v)  CTK_PP_REPEAT_15(macro, delim, v) delim() macro(15, v)
#define CTK_PP_REPEAT_17(macro, delim, v)  CTK_PP_REPEAT_16(macro, delim, v) delim() macro(16, v)
#define CTK_PP_REPEAT_18(macro, delim, v)  CTK_PP_REPEAT_17(macro, delim, v) delim() macro(17, v)
#define CTK_PP_REPEAT_19(macro, delim, v)  CTK_PP_REPEAT_18(macro, delim, v) delim() macro(18, v)
#define CTK_PP_REPEAT_20(macro, delim, v)  CTK_PP_REPEAT_19(macro, delim, v) delim() macro(19, v)

/* 1 origin repeat macros */
#define CTK_PP_REPEAT_1ORIGIN(count, macro, delim, v)      \
        CTK_PP_REPEAT_1ORIGIN_(count, macro, delim, v)

/* indirection */
#define CTK_PP_REPEAT_1ORIGIN_(count, macro, delim, v)      \
        CTK_PP_REPEAT_1ORIGIN_ ## count(macro, delim, v)

#define CTK_PP_REPEAT_1ORIGIN_0(macro, delim, v)   
#define CTK_PP_REPEAT_1ORIGIN_1(macro, delim, v)   
#define CTK_PP_REPEAT_1ORIGIN_2(macro, delim, v)   CTK_PP_REPEAT_1ORIGIN_1(macro, delim, v) delim() macro(1, v)
#define CTK_PP_REPEAT_1ORIGIN_3(macro, delim, v)   CTK_PP_REPEAT_1ORIGIN_2(macro, delim, v) delim() macro(2, v)
#define CTK_PP_REPEAT_1ORIGIN_4(macro, delim, v)   CTK_PP_REPEAT_1ORIGIN_3(macro, delim, v) delim() macro(3, v)
#define CTK_PP_REPEAT_1ORIGIN_5(macro, delim, v)   CTK_PP_REPEAT_1ORIGIN_4(macro, delim, v) delim() macro(4, v)
#define CTK_PP_REPEAT_1ORIGIN_6(macro, delim, v)   CTK_PP_REPEAT_1ORIGIN_5(macro, delim, v) delim() macro(5, v)
#define CTK_PP_REPEAT_1ORIGIN_7(macro, delim, v)   CTK_PP_REPEAT_1ORIGIN_6(macro, delim, v) delim() macro(6, v)
#define CTK_PP_REPEAT_1ORIGIN_8(macro, delim, v)   CTK_PP_REPEAT_1ORIGIN_7(macro, delim, v) delim() macro(7, v)
#define CTK_PP_REPEAT_1ORIGIN_9(macro, delim, v)   CTK_PP_REPEAT_1ORIGIN_8(macro, delim, v) delim() macro(8, v)
#define CTK_PP_REPEAT_1ORIGIN_10(macro, delim, v)  CTK_PP_REPEAT_1ORIGIN_9(macro, delim, v) delim() macro(9, v)
#define CTK_PP_REPEAT_1ORIGIN_11(macro, delim, v)  CTK_PP_REPEAT_1ORIGIN_10(macro, delim, v) delim() macro(10, v)
#define CTK_PP_REPEAT_1ORIGIN_12(macro, delim, v)  CTK_PP_REPEAT_1ORIGIN_11(macro, delim, v) delim() macro(11, v)
#define CTK_PP_REPEAT_1ORIGIN_13(macro, delim, v)  CTK_PP_REPEAT_1ORIGIN_12(macro, delim, v) delim() macro(12, v)
#define CTK_PP_REPEAT_1ORIGIN_14(macro, delim, v)  CTK_PP_REPEAT_1ORIGIN_13(macro, delim, v) delim() macro(13, v)
#define CTK_PP_REPEAT_1ORIGIN_15(macro, delim, v)  CTK_PP_REPEAT_1ORIGIN_14(macro, delim, v) delim() macro(14, v)
#define CTK_PP_REPEAT_1ORIGIN_16(macro, delim, v)  CTK_PP_REPEAT_1ORIGIN_15(macro, delim, v) delim() macro(15, v)
#define CTK_PP_REPEAT_1ORIGIN_17(macro, delim, v)  CTK_PP_REPEAT_1ORIGIN_16(macro, delim, v) delim() macro(16, v)
#define CTK_PP_REPEAT_1ORIGIN_18(macro, delim, v)  CTK_PP_REPEAT_1ORIGIN_17(macro, delim, v) delim() macro(17, v)
#define CTK_PP_REPEAT_1ORIGIN_19(macro, delim, v)  CTK_PP_REPEAT_1ORIGIN_18(macro, delim, v) delim() macro(18, v)
#define CTK_PP_REPEAT_1ORIGIN_20(macro, delim, v)  CTK_PP_REPEAT_1ORIGIN_19(macro, delim, v) delim() macro(19, v)

#define CTK_PP_ENUM_PARMS(count, param)      \
        CTK_PP_REPEAT(count, CTK_PP_ENUM_PARMS_M, CTK_PP_COMMA, param)

#define CTK_PP_ENUM_PARMS_M(n, param)  param ## n

#define CTK_PP_ARRAY_ELEM(i,array)  \
    CTK_PP_CAT(CTK_PP_ARRAY_ELEM_,i) array

#define CTK_PP_ARRAY_ELEM_0(a,...)                   a
#define CTK_PP_ARRAY_ELEM_1(a,b,...)                 b
#define CTK_PP_ARRAY_ELEM_2(a,b,c,...)               c
#define CTK_PP_ARRAY_ELEM_3(a,b,c,d,...)             d
#define CTK_PP_ARRAY_ELEM_4(a,b,c,d,e,...)           e
#define CTK_PP_ARRAY_ELEM_5(a,b,c,d,e,f,...)         f
#define CTK_PP_ARRAY_ELEM_6(a,b,c,d,e,f,g,...)       g
#define CTK_PP_ARRAY_ELEM_7(a,b,c,d,e,f,g,h,...)     h
#define CTK_PP_ARRAY_ELEM_8(a,b,c,d,e,f,g,h,i,...)   i
#define CTK_PP_ARRAY_ELEM_9(a,b,c,d,e,f,g,h,i,j,...) j

#if defined(_CTKLIB) || defined(UNIT_TEST)
# define CTK_PP_ITERATION_INCLUDE     "Iteration.tpp"
#else
# define CTK_PP_ITERATION_INCLUDE     "ctk/Iteration.tpp"
#endif

#endif /* _CTK_PREPROCESSOR_H */
