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

#ifndef _CTK_EXCEPTION_HPP
#define _CTK_EXCEPTION_HPP

#include <execinfo.h>
#include <errno.h>
#include <stdio.h>

#include <exception>
#include <stdexcept>
#include <string>
#include <iostream>

#include "ctk_error.h"

/**
 * @file Exception.hpp
 * @brief Definitions for CTK++ exception class
 */
namespace ctk {

/**
 * CTK Exception base class
 */
class Exception : public std::exception {
    public:
        Exception() : m_msg("") {
            void *array[25];
            m_symSize  = backtrace(array, 25);
            m_symbols = backtrace_symbols(array, m_symSize);
        }
        Exception(const std::string& msg) : m_msg(msg) { }
        ~Exception() throw() { }
        virtual const char *what() const throw() { return m_msg.c_str(); }

        /**
         * Returns the CTK error code if the exception was caused by error
         */
        virtual const int reason() const throw() { return CTK_ERROR_INTERNAL; }

        /** 
         * Outputs the stack trace info to the given ostream
         * (default = std::cout).  Include "-rdynamic" in the
         * compiling options to get some readable output.
         */
        void trace(std::ostream &os = std::cout) {
            os << "#Exception: " << what() << std::endl;
            for (int i = 0; i < m_symSize; i++)
                os << "[trace " << i << "] " << m_symbols[i] << std::endl;
        }

    private:
        int m_symSize;
        char **m_symbols;
        std::string m_msg;
};

class Warning : public Exception {
};

class Error : public Exception {
    public:
        Error(int code) : m_code(code) {
            ::ctk_strerror_r(code, m_buf, sizeof(m_buf));
        }
        Error(const char *msg, int code) : m_code(code) {
            int ret = ::snprintf(m_buf, sizeof(m_buf)-100, msg);
            ::ctk_strerror_r(code, (char*)&m_buf[ret], sizeof(m_buf)-ret);
        }
        ~Error() throw() { }
        const char *what() const throw() { return (const char*)m_buf; }
        const int reason() const throw() { return m_code; }

    private:
        char m_buf[256];
        int m_code;
};

}; // namespace ctk

#endif /* _CTK_EXCEPTION_HPP */
