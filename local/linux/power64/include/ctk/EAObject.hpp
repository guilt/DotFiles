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

#ifndef _EAOBJECT_HPP
#define _EAOBJECT_HPP

#include "ctk_types.h"
#ifndef __SPU__
# include "ctk_atomic.h"
#else /* __SPU__ */
# if defined(UNIT_TEST) || defined(_CTKLIB)
#  include "ctk_spu_atomic.h"
# else
#  include "spu/ctk_spu_atomic.h"
# endif
#endif /* __SPU__ */

namespace ctk {

/** EA object interface */
class EAObject {
        ctk_atomic_ea_t m_counter_ea;

#if !defined( __SPU__)
    protected:
        // implement me
        virtual void destroy() { }

        void release() {
            if (m_counter_ea != 0) {
                if (_ctk_atomic_dec(m_counter_ea) == 1) {
                    destroy();
                    m_counter_ea = 0;
                }
            }
        }

        void alloc() {
            if (m_counter_ea == 0)
                _ctk_atomic_create_local(&m_counter_ea);
            _ctk_atomic_inc(m_counter_ea);
        }

        void copy(const EAObject *o) {
            m_counter_ea = o->m_counter_ea;
            if (m_counter_ea != 0)
                _ctk_atomic_inc(m_counter_ea);
        }

        void assign(const EAObject *o) {
            if (o == this)
                return;
            release();
            m_counter_ea = o->m_counter_ea;
            alloc();
        }
#endif /* !__SPU__ */

    public:
        EAObject() : m_counter_ea(0) { }
        virtual ~EAObject() { }
        virtual operator void*() = 0;
        virtual operator ea_addr_t() = 0;
};

};

#endif /* _EAOBJECT_HPP */
