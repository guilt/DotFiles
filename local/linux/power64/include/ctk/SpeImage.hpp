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

#ifndef _CTK_SPE_PROGRAM_HPP
#define _CTK_SPE_PROGRAM_HPP

#include "ctk_common.h"
#include "ctk_spe.h"
#include "ctk_atomic.h"

/**
 * @file SpeImage.hpp
 * @brief Definitions for CTK++ SPE program image interface
 */
namespace ctk {

/**
 * SPE program handle class
 */
class SpeImage {
        ctk_spe_program_t *m_program;
        ctk_atomic_ea_t m_counter_ea;

        void release() {
            if (m_counter_ea != 0) {
                if (_ctk_atomic_dec(m_counter_ea) == 1) {
                    // we were the last one
                    ::ctk_spe_image_close(m_program);
                    _ctk_atomic_destroy_local(m_counter_ea);
                }
            }
        }

        friend class SpeThread;

        /* default constructor (private) */
        SpeImage() : m_program(NULL), m_counter_ea(0) { }

    public:
        /** Constructor */
        SpeImage(ctk_spe_program_t &program)
            : m_program(&program), m_counter_ea(0) { }

        /** Constructor
         * @see #ctk_spe_image_open
         * @throws #Error on error
         */
        SpeImage(const char *filename) {
            if (filename == NULL)
                throw Error(CTK_ERROR_NULL_OBJECT);
            int ret = ::ctk_spe_image_open(&m_program, filename);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            _ctk_atomic_create_local(&m_counter_ea);
            _ctk_atomic_inc(m_counter_ea);
        }

        /** Copy constructor
         * @see #ctk_spe_image_open
         * @throws #Error on error
         */
        SpeImage(const SpeImage &image)
            : m_program(image.m_program), m_counter_ea(image.m_counter_ea)
        {
            if (m_counter_ea != 0)
                _ctk_atomic_inc(m_counter_ea);
        }

        /** Assignment operator */
        SpeImage &operator=(const SpeImage &image) {
            if (m_program == image.m_program)
                return *this;
            release();

            m_program = image.m_program;
            m_counter_ea = image.m_counter_ea;
            _ctk_atomic_inc(m_counter_ea);
            return *this;
        }

        /** Destructor
         * @see #ctk_spe_image_close
         */
        virtual ~SpeImage() {
            release();
        }

        /** Returns the SPE program handle */
        ctk_spe_program_t *getProgram() const {
            return m_program;
        }
};

}; // namespace ctk

#endif /* _CTK_SPE_PROGRAM_HPP */
