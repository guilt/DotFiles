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

#ifndef _CTK_SPINLOCK_IMPL_H
#define _CTK_SPINLOCK_IMPL_H

#ifndef _CTK_SPINLOCK_H
# error do not include this file directly!
#endif

static inline void _ctk_spinlock_init(ctk_spinlock_ea_t lock)
{
    CTK_SPINLOCK_VAR(lock) = 0;
}

static inline int _ctk_spin_lock(ctk_spinlock_ea_t lock)
{
    ctk_spinlock_t *var = CTK_SPINLOCK_PTR(lock);
    unsigned int tmp;

   __asm__ __volatile__ ("# ctk_spin_lock\n"
"1:	lwarx   %[tmp], 0, %[lock] # load with reservation \n\t"
        "cmpwi  %[tmp], 0      #                      \n\t"
        "bne    1b             # if not zero, retry   \n\t"
        "li     %2, 1          \n\t"
        "stwcx. %2, 0, %[lock] # attempt to store  \n\t"
        "bne    1b             # spin if failed \n\t"
        "isync                  \n"
        : [tmp] "=&r" (tmp)
        : [lock] "r" (var), "r" (1)
        : "cr0", "memory"
    );
    return 0;
}

static inline int _ctk_spin_trylock(ctk_spinlock_ea_t lock)
{
    volatile ctk_spinlock_t *var = CTK_SPINLOCK_PTR(lock);
    unsigned int tmp;
    int err = CTK_ERROR_BUSY;

    __asm__ __volatile__  ("# ctk_spin_trylock\n"
"1:      lwarx   %0, 0, %2      # load with reservation\n\t"
        "cmpwi   0, %0, 0       # compare with 0 \n\t"
        "bne     2f             # if not zero, done \n\t"
        "stwcx.  %3, 0, %2      # attempt to store \n\t"
        "bne-    1b\n\t"
        "li      %1, 0          # load 0 to err \n\t"
        "isync\n\t"
        "2:      "
        : "=&r" (tmp), "=&r" (err)
        : "r" (var), "r" (1), "1" (err)
        : "cr0", "memory"
    );

    return err;
}

static inline void _ctk_spin_unlock(ctk_spinlock_ea_t lock)
{
    _ctk_atomic_set(lock, 0);
}

#endif /* _CTK_SPINLOCK_IMPL_H */
