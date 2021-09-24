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

#ifndef _CTK_SPE_THREAD_HPP
#define _CTK_SPE_THREAD_HPP

#include "ctk_spe_thread.h"
#include "Common.hpp"
#include "SpeContext.hpp"
#include "SpeImage.hpp"

/**
 * @file SpeThread.hpp
 * @brief Definitions for CTK++ SPE thread interface
 */
namespace ctk {

/**
 * SPE thread class
 */
class SpeThread : public Spe {
        SpeImage m_image;
        ctk_spe_thread_t m_thread;
        bool *m_started_p;
        int *m_ret_status_p;
        ctk_atomic_ea_t m_thread_counter_ea;
        ctk_atomic_ea_t m_info_counter_ea;

        friend class SpeThreadGroup;
        friend class SpeThreadGroup::iterator;

        void release() {
            if (m_thread != NULL && m_thread_counter_ea != 0) {
                if (_ctk_atomic_dec(m_thread_counter_ea) == 1) {
                    // we were the last one
                    ::ctk_spe_thread_terminate(m_thread);
                    _ctk_atomic_destroy_local(m_thread_counter_ea);
                    m_thread = NULL;
                    m_thread_counter_ea = 0;
                }
            }
            if (m_info_counter_ea != 0) {
                if (_ctk_atomic_dec(m_info_counter_ea) == 1) {
                    // we were the last one
                    free(m_started_p);
                    free(m_ret_status_p);
                    _ctk_atomic_destroy_local(m_info_counter_ea);
                    m_info_counter_ea = 0;
                }
            }
        }

        void alloc() {
            m_started_p = (bool*)malloc(sizeof(bool*));
            m_ret_status_p = (int*)malloc(sizeof(int*));
            *m_started_p = false;
            *m_ret_status_p = 0;
            _ctk_atomic_create_local(&m_thread_counter_ea);
            _ctk_atomic_inc(m_thread_counter_ea);
            _ctk_atomic_create_local(&m_info_counter_ea);
            _ctk_atomic_inc(m_info_counter_ea);
        }

        /** Private constructor (for SpeThreadGroup::iterator) */
        SpeThread(ctk_spe_thread_t &thread) : m_thread(thread)
        {
            m_started_p = (bool*)malloc(sizeof(bool*));
            m_ret_status_p = (int*)malloc(sizeof(int*));
            _ctk_atomic_create_local(&m_info_counter_ea);
            _ctk_atomic_inc(m_info_counter_ea);
            if (ctk_spe_get_status(m_thread) >= CTK_SPE_STATUS_READY &&
                ctk_spe_get_status(m_thread) <= CTK_SPE_STATUS_RUNNING)
                *m_started_p = true;
            else
                *m_started_p = false;
            m_thread_counter_ea = 0;
        }

    protected:
        // implement
        virtual const ctk_spe_context_t& getInternalContext() const {
            if (m_thread == NULL)
                throw Error(CTK_ERROR_SPE_NOT_RUNNING);
            return m_thread->m_context;
        }

    public:
        /** Default constructor */
        SpeThread() : m_image(NULL), m_thread(NULL),
            m_thread_counter_ea(0), m_info_counter_ea(0) { }

        /** Constructor */
        SpeThread(const char *fname)
            : m_image(fname), m_thread(NULL), 
              m_thread_counter_ea(0), m_info_counter_ea(0) { }

        /** Constructor */
        SpeThread(ctk_spe_program_t &program)
            : m_image(program), m_thread(NULL), 
              m_thread_counter_ea(0), m_info_counter_ea(0) { }

        /** Constructor */
        SpeThread(ctk_spe_program_t *program)
            : m_image(*program), m_thread(NULL), 
              m_thread_counter_ea(0), m_info_counter_ea(0) { }

        /** Constructor */
        SpeThread(SpeImage &image)
            : m_image(image), m_thread(NULL), 
              m_thread_counter_ea(0), m_info_counter_ea(0) { }

        /** Constructor */
        SpeThread(const char *fname, void *argp, void *envp, int flags = 0)
            : m_image(fname), m_thread(NULL),
              m_thread_counter_ea(0), m_info_counter_ea(0)
        {
            int ret = ::ctk_spe_thread_create(&m_thread, m_image.getProgram(),
                            argp, envp, flags | CTK_SPE_DONT_START);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            alloc();
        }

        /** Constructor */
        SpeThread(ctk_spe_program_t &program, void *argp, void *envp, 
            int flags = 0) : m_image(program), m_thread(NULL),
              m_thread_counter_ea(0), m_info_counter_ea(0)
        {
            int ret = ::ctk_spe_thread_create(&m_thread, m_image.getProgram(),
                            argp, envp, flags | CTK_SPE_DONT_START);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            alloc();
        }

        /** Constructor */
        SpeThread(SpeImage &image, void *argp, void *envp, int flags = 0) 
            : m_image(image), m_thread(NULL)
        {
            int ret = ::ctk_spe_thread_create(&m_thread, m_image.getProgram(),
                            argp, envp, flags | CTK_SPE_DONT_START);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            alloc();
        }

        /** Copy constructor */
        SpeThread(const SpeThread &thread) : m_image(thread.m_image), 
            m_thread(thread.m_thread), m_started_p(thread.m_started_p),
            m_ret_status_p(thread.m_ret_status_p),
            m_thread_counter_ea(thread.m_thread_counter_ea),
            m_info_counter_ea(thread.m_info_counter_ea)
        {
            if (m_thread_counter_ea != 0)
                _ctk_atomic_inc(m_thread_counter_ea);
            if (m_info_counter_ea != 0)
                _ctk_atomic_inc(m_info_counter_ea);
        }

        /* assignment operator */
        SpeThread &operator=(const SpeThread &thread) {
            if (m_thread == thread.m_thread)
                return *this;
            release();
            m_image = thread.m_image;
            m_ret_status_p = thread.m_ret_status_p;
            m_started_p = thread.m_started_p;
            m_thread_counter_ea = thread.m_thread_counter_ea;
            m_info_counter_ea = thread.m_info_counter_ea;
            if (m_thread_counter_ea != 0)
                _ctk_atomic_inc(m_thread_counter_ea);
            if (m_info_counter_ea != 0)
                _ctk_atomic_inc(m_info_counter_ea);
            return *this;
        }

        /** Loads (sets) the SPE image. */
        void load(SpeImage &image) { m_image = image; }

        /**
         * Starts the thread.
         * @see #ctk_spe_thread_create
         * @throws #Error on error.
         */
        void start(void *argp = NULL, void *envp = NULL, int flags = 0) {
            if (m_thread_counter_ea != 0) {
                if (*m_started_p)
                    throw Error(CTK_ERROR_SPE_ALREADY_RUNNING);
                else {
                    int ret = ::ctk_spe_thread_start(m_thread);
                    if (ret != CTK_SUCCESS)
                        throw Error(ret);
                    *m_started_p = true;
                }
                return;
            }
            int ret = ::ctk_spe_thread_create(&m_thread, 
                        m_image.getProgram(), argp, envp,
                        flags &= ~CTK_SPE_DONT_START);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            alloc();
            *m_started_p = true;
        }

        /**
         * Waits the thread.
         * @see #ctk_spe_thread_wait
         * @throws #Error on error.
         */
        void wait(int *status = NULL) {
            if (m_thread_counter_ea == 0)
                throw Error(CTK_ERROR_SPE_NOT_RUNNING);
            int ret = ::ctk_spe_thread_wait(m_thread, m_ret_status_p);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            if (status != NULL)
                *status = *m_ret_status_p;
            _ctk_atomic_destroy_local(m_thread_counter_ea);
            m_thread_counter_ea = 0;
            m_thread = NULL;
            *m_started_p = false;
        }

        /**
         * Terminates the thread.
         * @see #ctk_spe_thread_terminate
         * @throws #Error on error.
         */
        void terminate() {
            if (m_thread_counter_ea == 0)
                throw Error(CTK_ERROR_SPE_NOT_RUNNING);
            int ret = ::ctk_spe_thread_terminate(m_thread);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            _ctk_atomic_destroy_local(m_thread_counter_ea);
            m_thread_counter_ea = 0;
            m_thread = NULL;
            *m_started_p = false;
            *m_ret_status_p |= (SIGSTOP >> 8);
        }

        /** Returns the SPE context of this thread
         * @throws Error if the thread is not running
         */
        const SpeContext getContext() const {
            if (m_thread == NULL)
                throw Error(CTK_ERROR_SPE_NOT_RUNNING);
            if (m_thread->m_context == NULL) {
                if (m_info_counter_ea == 0 || *m_started_p == false)
                    throw Error(CTK_ERROR_SPE_NOT_RUNNING);
                else {
                    while (ctk_spe_get_status(m_thread) == CTK_SPE_STATUS_READY)
                        ;
                    if (ctk_spe_get_status(m_thread) != CTK_SPE_STATUS_RUNNING)
                    {
                        throw Error(CTK_ERROR_SPE_NOT_RUNNING);
                    }
                }
            }
            return SpeContext(m_thread->m_context);
        }


        /** Returns the last exit code */
        int getExitCode() const { 
            if (m_info_counter_ea == 0)
                throw Error(CTK_ERROR_SPE_NOT_STARTED);
            return CTK_SPE_EXIT_STATUS(*m_ret_status_p);
        }

        /** Returns true if the last run exited normally */
        bool isExited() const {
            if (m_info_counter_ea == 0)
                return false;
            if (m_thread_counter_ea != 0)
                if (ctk_spe_get_status(m_thread) <= CTK_SPE_STATUS_RUNNING)
                    return false;
            return CTK_SPE_IF_EXITED(*m_ret_status_p);
        }

        /** Destructor */
        virtual ~SpeThread() {
            release();
        }
}; 

}; // namespace ctk

#endif /* _CTK_SPE_THREAD_HPP */
