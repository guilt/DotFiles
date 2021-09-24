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

#ifndef _CTK_STATIC_ASSERT_H
#define _CTK_STATIC_ASSERT_H

namespace ctk {
    template <bool> struct staticAssert { staticAssert(...); };
    template <> struct staticAssert <false> { };
};

#define CTK_STATIC_ASSERT(expr, msg) \
    { \
        class ERROR_##msg {}; \
        (void)sizeof(ctk::staticAssert<(expr) != 0>((new ERROR_##msg())));\
    }

#endif /* _CTK_STATIC_ASSERT_H */
