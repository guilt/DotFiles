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

#ifndef _CTK_SPE_RUNTIME_HPP
#define _CTK_SPE_RUNTIME_HPP

#include "Common.hpp"
#include "ctk_spe_runtime.h"

namespace ctk {

class SpeRuntime {
        ctk_spe_runtime_t m_runtime;
        int m_nspe;

        /* Do not provide copy constructor and assignment operators */
        SpeRuntime(const SpeRuntime& runtime);
	SpeRuntime& operator=(const SpeRuntime& runtime);

    public:
        /** Default constructor */
        SpeRuntime(int numberOfSpe = MAX_NR_SPE, int runqSize = 0)
            : m_runtime(NULL), m_nspe(numberOfSpe)
        {
            int ret = ::ctk_spe_runtime_create(&m_runtime, m_nspe, runqSize,
                    CTK_SPE_RUNTIME_TASK_FIFO, 0);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /** Destructor */
        ~SpeRuntime()
        {
            if (m_runtime != NULL) {
                ::ctk_spe_runtime_shutdown(m_runtime);
            }
        }
        
        /** Enqueues a task to the runtime
          * @see #ctk_spe_runtime_enq_task
          * @throw Error on error
          */
        void enqTask(SpeTask &task, void *arg1 = NULL, void *arg2 = NULL)
        {
            int ret = ::ctk_spe_runtime_enq_task(m_runtime, task.m_task, 
                arg1, arg2);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /** Enqueues a task to the runtime
          * @see #ctk_spe_runtime_enq_task
          * @throw Error on error
          */
        void enqTask(SpeTask *&task, void *arg1 = NULL, void *arg2 = NULL)
        {
            int ret = ::ctk_spe_runtime_enq_task(m_runtime, task->m_task, 
                arg1, arg2);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
          * Waits a task which has been enqueued in the runtime
          * @see #ctk_spe_runtime_wait_task
          * @throw Error on error
          */
        void waitTask(SpeTask &task)
        {
            int ret = ::ctk_spe_runtime_wait_task(m_runtime, task.m_task);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
          * Waits a task which has been enqueued in the runtime
          * @see #ctk_spe_runtime_wait_task
          * @throw Error on error
          */
        void waitTask(SpeTask *&task)
        {
            int ret = ::ctk_spe_runtime_wait_task(m_runtime, task->m_task);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
         * Spin-waits a task
          * @see #ctk_spe_runtime_wait_task_spin
          * @throw Error on error
         */
        void waitTaskSpin(SpeTask &task)
        {
            int ret = ::ctk_spe_runtime_wait_task_spin(m_runtime, task.m_task);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /**
         * Spin-waits a task
          * @see #ctk_spe_runtime_wait_task_spin
          * @throw Error on error
         */
        void waitTaskSpin(SpeTask *&task)
        {
            int ret = ::ctk_spe_runtime_wait_task_spin(m_runtime, task->m_task);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /** Waits all the tasks in the runtime.
          * @see #ctk_spe_runtime_wait_all
          * @throw Error on error
          */
        void waitAllTasks(ctk_uint64_t id_mask = CTK_SPE_TASK_MASK_ALL)
        {
            int ret = ::ctk_spe_runtime_wait_all(m_runtime, id_mask);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
        }

        /** Waits any task in the runtime.
          * @see #ctk_spe_runtime_wait_any
          * @throw Error on error
          */
        void waitAnyTask(SpeTask *&task, 
                         ctk_uint64_t id_mask = CTK_SPE_TASK_MASK_ALL)
        {
            ctk_spe_task_t _task;
            int ret = ::ctk_spe_runtime_wait_any(m_runtime, id_mask, &_task);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            task = (SpeTask *)((unsigned long)_task->class_ea);
        }

        /** Waits all the tasks in the runtime and releases the runtime.
          * @see #ctk_spe_runtime_wait
          * @throw Error on error
          */
        void wait()
        {
            int ret = ::ctk_spe_runtime_wait(m_runtime);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            m_runtime = NULL;
        }

        int invokeTask(SpeTask &task, void *arg1 = NULL, void *arg2 = NULL)
        {
            enqTask(task, arg1, arg2);
            waitTask(task);
            return task.getExitCode();
        }

        /** Shuts down the runtime
          * @see #ctk_spe_runtime_shutdown
          * @throw Error on error
          */
        void shutdown()
        {
            int ret = ::ctk_spe_runtime_shutdown(m_runtime);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            m_runtime = NULL;
        }

        /**
         * Returns the number of SPEs this runtime uses
         */
        int getNumberOfSpe()
        {
            return m_nspe;
        }


}; // class SpeRuntime

}; // namespace ctk

#endif /* _CTK_SPE_RUNTIME_HPP */
