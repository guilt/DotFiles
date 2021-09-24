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

#ifndef _CTK_SPE_THREAD_GROUP_HPP
#define _CTK_SPE_THREAD_GROUP_HPP

#include "ctk_spe_thread.h"
#include "ctk_spe_thread_group.h"
#include "SpeImage.hpp"

/* forward declaration */
extern "C" {
extern int ctk_spe_thread_group_destroy(ctk_spe_thread_group_t group);
};

/**
 * @file SpeThreadGroup.hpp
 * @brief Definitions for CTK++ SPE thread group interface
 */
namespace ctk {

/* Forward declaration */
class SpeThread;

/**
 * SPE thread group class
 */
class SpeThreadGroup {
        SpeImage m_image;
        ctk_spe_thread_group_t m_group;
        int *m_status;
        bool m_started;
        int m_count;

    public:
        /** Constructor */
        SpeThreadGroup(int maxThreads, int options = 0) : m_image(NULL),
            m_group(NULL), m_status(NULL), m_started(false), m_count(0)
        {
            int ret = ::ctk_spe_thread_group_create(&m_group, maxThreads, 0);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            m_status = new int[maxThreads];
        }

        /** Constructor (creates SPMD threads) */
        SpeThreadGroup(int nThreads, SpeImage &image)
            : m_image(image), m_group(NULL), m_status(NULL),
              m_started(false), m_count(nThreads) { }

        /** Constructor (creates SPMD threads) */
        SpeThreadGroup(int nThreads, ctk_spe_program_t &program)
            : m_image(program), m_group(NULL), m_status(NULL), 
              m_started(false), m_count(nThreads) { }

        /** Constructor (creates SPMD threads) */
        SpeThreadGroup(int nThreads, const char *fname)
            : m_image(fname), m_group(NULL), m_status(NULL), 
              m_started(false), m_count(nThreads) { }

        /** Constructor (creates SPMD threads and starts them) */
        SpeThreadGroup(int nThreads, SpeImage &image, void *argp,
            void *envp, int flags = 0)
            : m_image(NULL), m_group(NULL), m_status(NULL), 
              m_started(false), m_count(0)
        {
            int ret = ::ctk_spe_thread_group_create_spmd(&m_group, 
                nThreads, image.getProgram(), argp, envp, flags);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            m_count = ::ctk_spe_thread_group_get_threads_count(m_group);
            m_status = new int[m_count];
            if ((flags & CTK_SPE_DONT_START) == 0)
                m_started = true;
        }

        /** Constructor (creates SPMD threads and starts them) */
        SpeThreadGroup(int nThreads, ctk_spe_program_t &program, 
            void *argp, void *envp, int flags = 0) : m_image(NULL), 
            m_group(NULL), m_status(NULL), m_started(false), m_count(0)
        {
            int ret = ::ctk_spe_thread_group_create_spmd(&m_group, 
                nThreads, &program, argp, envp, flags);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            m_count = ::ctk_spe_thread_group_get_threads_count(m_group);
            m_status = new int[m_count];
            if ((flags & CTK_SPE_DONT_START) == 0)
                m_started = true;
        }

        /** Constructor (creates SPMD threads and starts them) */
        SpeThreadGroup(int nThreads, const char *fname, 
            void *argp, void *envp, int flags = 0) : m_image(NULL),
            m_group(NULL), m_status(NULL), m_started(false), m_count(0)
        {
            SpeImage image(fname);
            int ret = ::ctk_spe_thread_group_create_spmd(&m_group, 
                nThreads, image.getProgram(), argp, envp, flags);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            m_count = ::ctk_spe_thread_group_get_threads_count(m_group);
            m_status = new int[m_count];
            if ((flags & CTK_SPE_DONT_START) == 0)
                m_started = true;
        }

        /** Destructor */
        ~SpeThreadGroup() {
            if (m_status != NULL)
                delete m_status;
            if (m_group != NULL)
                ::ctk_spe_thread_group_destroy(m_group);
        }

        /**
         * Adds a thread to the group.
         * @see #ctk_spe_thread_group_add
         * @throws #Error on error.
         */
        void add(SpeThread &thread);

        /**
         * Starts the threads in the group.
         * @see #ctk_spe_thread_group_start
         * @throws #Error on error.
         */
        void start(void *argp = NULL, void *envp = NULL, int flags = 0) {
            if (m_group != NULL) {
                if (m_started)
                    throw Error(CTK_ERROR_SPE_ALREADY_RUNNING);
                int ret = ::ctk_spe_thread_group_start(m_group, NULL);
                if (ret != CTK_SUCCESS)
                    throw Error(ret);
            } else if (m_image.getProgram() != NULL) {
                int ret = ::ctk_spe_thread_group_create_spmd(&m_group, 
                        m_count, m_image.getProgram(), argp, envp, flags);
                if (ret != CTK_SUCCESS)
                    throw Error(ret);
                m_count = ::ctk_spe_thread_group_get_threads_count(m_group);
                m_status = new int[m_count];
            } else
                throw Error(CTK_ERROR_NULL_OBJECT);
            m_started = true;
        }

        /**
         * Waits the threads in the group.
         * @see #ctk_spe_thread_group_wait
         * @throws #Error on error.
         */
        void wait() {
            int ret = ::ctk_spe_thread_group_wait(m_group, m_status);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            m_group = NULL;
        }

        /**
         * Returns the number of SPE threads in the group
         * @see #ctk_spe_thread_group_get_threads_count
         */
        int size() {
            return m_count;
        }

        /**
         * Returns the exit code of a thread specified by the thread #index.
         * @throws #Error on error.
         */
        int getExitCode(int index) {
            if (index < 0 || index >= m_count)
                throw Error(CTK_ERROR_OUT_OF_RANGE);
            if (m_started == false)
                throw Error(CTK_ERROR_BAD_SPE_STATUS);
            return CTK_SPE_EXIT_STATUS(m_status[index]);
        }

        /**
         * Returns true if a thread specified by the thread rank #index
         * exited normally.
         * @throws #Error on error.
         */
        bool isExited(int index) {
            if (index < 0 || index >= m_count)
                throw Error(CTK_ERROR_OUT_OF_RANGE);
            if (m_started == false)
                throw Error(CTK_ERROR_BAD_SPE_STATUS);
            return CTK_SPE_IF_EXITED(m_status[index]);
        }

        /**
         * Writes a 32-bit data to IN mbox of each SPE in the group 
         * [Non-blocking]
         * @param[in] the data to be written
         * @throws #Error on error
         */
        void writeMbox(unsigned int data) {
            int ret = ::ctk_mbox_group_write(m_group, data);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
         * Reads a 32-bit data from OUT mbox of each SPE in the group [Blocking]
         * @param[out] arrya of the read data
         * @see #ctk_mbox_read_block
         * @throws #Error on error
         */
        void readMboxBlocking(unsigned int data[]) {
            int ret = ::ctk_mbox_group_read_block(m_group, data);
            if (ret != CTK_SUCCESS) 
                throw Error(ret);
        }

        /**
         * Writes a 32-bit data to IN mbox of each SPE in the group [Blocking]
         * @param[in] the data to be written
         * @see #ctk_mbox_write_block
         * @throws #Error on error
         */
        void writeMboxBlocking(unsigned int data) {
            int ret = ::ctk_mbox_group_write_block(m_group, data);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
         * Writes the #data to the signal notification register specified 
         * by #reg. 
         * @param[in] reg - specifies the signal notification register to be 
         *   written. The value should be one of either SignalRegister::reg1
         *   or SignalRegister::reg2.
         * @param[in] data - the 32-bit data to be written
         * @see #ctk_signal_write
         * @throws #Error on error
         */
        inline void
        writeSignalRegister(SignalRegister::reg reg, ctk_uint32_t data)
        {
            int ret = ::ctk_signal_group_write(m_group, (unsigned int)reg, data);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
         * Writes the #data to the signal notification register 1.
         * @param[in] data - the 32-bit data to be written
         * @throws #Error on error
         * @see #ctk_signal_write_reg1
         */
        inline void
        writeSignalRegister1(unsigned int data)
        {
            int ret = ::ctk_signal_group_write_reg1(m_group, data);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
         * Writes the #data to the signal notification register 2.
         * @param[in] data - the 32-bit data to be written
         * @throws #Error on error
         * @see #ctk_signal_write_reg2
         */
        inline void
        writeSignalRegister2(unsigned int data)
        {
            int ret = ::ctk_signal_group_write_reg2(m_group, data);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /** Forward iterator */
        class iterator {
            public:
                ctk_spe_thread_t m_cursor;
                explicit iterator(ctk_spe_thread_t p = 0) : m_cursor(p) { }
                iterator& operator++() {
                    m_cursor = (ctk_spe_thread_t)(unsigned long)m_cursor->next;
                    return *this;
                }
                iterator operator++(int) {
                    ctk_spe_thread_t old = m_cursor;
                    m_cursor = (ctk_spe_thread_t)(unsigned long)m_cursor->next;
                    return iterator(old);
                }
                SpeThread operator*();
                friend bool operator==(const iterator& x, const iterator& y) {
                    return x.m_cursor == y.m_cursor;
                }
                friend bool operator!=(const iterator& x, const iterator& y) {
                    return x.m_cursor != y.m_cursor;
                }
        };

        iterator begin() { 
            if (m_group == NULL)
                return iterator(NULL);
            return iterator((ctk_spe_thread_t)m_group->m_threads.head);
        }
        iterator end() {
            return iterator(NULL);
        }
};

};

#include "SpeThread.hpp"

namespace ctk {

inline void SpeThreadGroup::add(SpeThread &thread) {
    if (m_started)
        throw Error(CTK_ERROR_SPE_ALREADY_RUNNING);
    int ret = ::ctk_spe_thread_group_add(m_group, thread.m_thread);
    if (ret != CTK_SUCCESS)
        throw Error(ret);
}

inline SpeThread SpeThreadGroup::iterator::operator*() { 
    return SpeThread(m_cursor);
}

}; // namespace ctk

#endif /* _CTK_SPE_THREAD_GROUP_HPP */
