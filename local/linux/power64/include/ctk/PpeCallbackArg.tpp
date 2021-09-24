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

#ifndef _CTK_PPE_CALLBACK_ARG_TPP
#define _CTK_PPE_CALLBACK_ARG_TPP

namespace ctk {

#ifndef LS_ARG_ADDR
struct spe_reg128 {
    unsigned int slot[4];
};
# define LS_ARG_ADDR(_index)                             \
        (&((struct spe_reg128 *) ((char *) ls + ls_args))[_index])
# define PUT_LS_RC(_a, _b, _c, _d)                       \
    ret->slot[0] = (unsigned int) (_a);                 \
    ret->slot[1] = (unsigned int) (_b);                 \
    ret->slot[2] = (unsigned int) (_c);                 \
    ret->slot[3] = (unsigned int) (_d);                 \
    __asm__ __volatile__ ("sync" : : : "memory")
#define GET_LS_PTR(_off)                        \
    (void *) ((char *) ls + ((_off) & LS_ADDR_MASK))
#endif /* !LS_ARG_ADDR */

/* Definitions for Callback args */
// Values (< 32-bit)
template <typename T> struct PpeCallbackArg {
    static inline T get_value(char *ls, struct spe_reg128 *arg) {
        return (T)arg->slot[0];
    }
    static inline void put_rc(struct spe_reg128 *ret, T rc) {
        PUT_LS_RC(rc, 0, 0, errno);
    }
};

// 64-bit Values
template <> struct PpeCallbackArg <long long> {
    static inline long long get_value(char *ls, struct spe_reg128 *arg) {
        return (((long long)arg->slot[0]) << 32) | arg->slot[1];
    }
    static inline void put_rc(struct spe_reg128 *ret, long long rc) {
        PUT_LS_RC(ea_hi32(rc), ea_lo32(rc), 0, errno);
    }
};

// 64-bit Values
template <> struct PpeCallbackArg <unsigned long long> {
    static inline long long get_value(char *ls, struct spe_reg128 *arg) {
        return (((unsigned long long)arg->slot[0]) << 32) | arg->slot[1];
    }
    static inline void put_rc(struct spe_reg128 *ret, unsigned long long rc) {
        PUT_LS_RC(ea_hi32(rc), ea_lo32(rc), 0, errno);
    }
};

// Pointer Values
template <typename T> struct PpeCallbackArg <T*> {
    static inline T *get_value(char *ls, struct spe_reg128 *arg) {
        return (T*)GET_LS_PTR(arg->slot[0]);
    }
    static inline void put_rc(struct spe_reg128 *ret, T *rc) {
        PUT_LS_RC(rc, 0, 0, errno);
    }
};

};

#endif /* _CTK_PPE_CALLBACK_ARG_TPP */
