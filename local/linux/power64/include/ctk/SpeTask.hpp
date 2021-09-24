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

#ifndef _CTK_SPE_TASK_HPP
#define _CTK_SPE_TASK_HPP

#include "Common.hpp"
#include "ctk_spe_task_status.h"
#include "ctk_spe_task.h"
#include "ctk_atomic.h"
#include "ctk_task_alloc.h"

namespace ctk {

/**
 * SPE Task Image class
 */
class SpeTaskImage {
        /** Task image body */
        ctk_spe_task_image_t m_image;

        friend class SpeTask;

        void release() {
            if (m_image != NULL && ctk_spe_task_image_release(m_image) <= 1) {
                /* the image can be destroyed */
                ::ctk_elf_free_load_image((void*)(unsigned long)
                    m_image->load_image);
                ::ctk_task_free_align((void*)m_image);
                m_image = NULL;
            }
        }

    public:
        /** Default constructor */
        SpeTaskImage() : m_image(NULL) { }

        /** Constructor */
        SpeTaskImage(SpeImage &image) {
            int ret = ::ctk_spe_task_image_create(&m_image, image.getProgram());
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            ctk_spe_task_image_add_ref(m_image);
        }

        /** Constructor */
        SpeTaskImage(const char *fname) {
            SpeImage image(fname);
            int ret = ::ctk_spe_task_image_create(&m_image, image.getProgram());
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            ctk_spe_task_image_add_ref(m_image);
        }

        /** Constructor */
        SpeTaskImage(ctk_spe_program_t &program) {
            int ret = ::ctk_spe_task_image_create(&m_image, &program);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            ctk_spe_task_image_add_ref(m_image);
        }

        /** Constructor */
        SpeTaskImage(const SpeImage &image) {
            int ret = ::ctk_spe_task_image_create(&m_image, 
                image.getProgram());
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            ctk_spe_task_image_add_ref(m_image);
        }

        /** Constructor */
        SpeTaskImage(const ctk_spe_task_image_t &image) {
            m_image = image;
            if (m_image != NULL) 
                ctk_spe_task_image_add_ref(m_image);
        }

        /** Copy constructor */
        SpeTaskImage(const SpeTaskImage &image) : m_image(image.m_image) {
            if (m_image != NULL)
                ctk_spe_task_image_add_ref(m_image);
        }

        /** Assignment */
        SpeTaskImage& operator=(const SpeTaskImage &image) {
            release();
            m_image = image.m_image;
            if (m_image != NULL)
                ctk_spe_task_image_add_ref(m_image);
            return *this;
        }

        /** Destructor */
        virtual ~SpeTaskImage() {
            release();
        }
};

class SpeTask {
        ctk_spe_task_t m_task;
        SpeTaskImage m_image;
        int m_flags;
        ctk_atomic_ea_t m_counter_ea;

        friend class SpeRuntime;

        void create(SpeTaskImage &image, int flags) {
            int ret = ::ctk_spe_task_create_by_image(&m_task, 
                            image.m_image, flags);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            m_task->class_ea = EAADDR(this);
            _ctk_atomic_create_local(&m_counter_ea);
            _ctk_atomic_inc(m_counter_ea);
        }

        void release() {
            if (m_counter_ea != 0 && _ctk_atomic_dec(m_counter_ea) <= 1) {
                ::ctk_spe_task_destroy(m_task);
                _ctk_atomic_destroy_local(m_counter_ea);
                m_task = NULL;
                m_counter_ea = 0;
            }
        }

    public:
        /** SpeTask status */
        enum SpeTaskStatus {
            INITIALIZED = CTK_SPE_TASK_STATUS_INITIALIZED, /**< initialized */
            QUEUED = CTK_SPE_TASK_STATUS_QUEUED,     /**< queued */
            RUNNABLE = CTK_SPE_TASK_STATUS_RUNNABLE, /**< runnable */
            FINISHED = CTK_SPE_TASK_STATUS_FINISHED, /**< finished */
        };

        /** Default constructor */
        SpeTask() : m_task(NULL), m_counter_ea(0) { }

        /** Constructor */
        SpeTask(SpeImage &image, int flags = 0)
            : m_task(NULL), m_image(image), m_flags(flags)
        {
            create(m_image, flags);
        }

        /** Constructor */
        SpeTask(SpeTaskImage &image, int flags = 0)
            : m_task(NULL), m_image(image), m_flags(flags)
        {
            create(m_image, flags);
        }

        /** Constructor */
        SpeTask(const char *fname, int flags = 0)
            : m_task(NULL), m_image(fname), m_flags(flags)
        {
            create(m_image, flags);
        }

        /** Constructor */
        SpeTask(ctk_spe_program_t &program, int flags = 0)
            : m_task(NULL), m_image(program), m_flags(flags)
        {
            create(m_image, flags);
        }

        /** Constructor: just creates a new Task reference to the
            given task */
        SpeTask(const ctk_spe_task_t &task) : m_task(task), m_counter_ea(0)
        {
            if (m_task != NULL) {
                m_flags = task->flag;
                m_image = ((SpeTask *)(unsigned long)task->class_ea)->m_image;
                m_counter_ea = ((SpeTask *)(unsigned long)
                                    task->class_ea)->m_counter_ea;
                _ctk_atomic_inc(m_counter_ea);
            }
        }

        /** Copy constructor */
        SpeTask(const SpeTask &task)
            : m_task(NULL), m_image(task.m_image), 
              m_flags(task.m_flags)
        {
            /* creates a new one */
            create(m_image, m_flags);
        }

        /** Assignment operator */
        SpeTask& operator=(const SpeTask &task) {
            if (m_task == task.m_task)
                return *this;
            release();
            m_flags = task.m_flags;
            m_image = task.m_image;
            create(m_image, m_flags);
            /*
            m_task = task.m_task;
            m_flags = task.m_flags;
            m_counter_ea = task.m_counter_ea;
            if (m_counter_ea != 0)
                _ctk_atomic_inc(m_counter_ea);
            */
            return *this;
        }

        /** Destructor */
        virtual ~SpeTask() {
            release();
        }

        /** Operator */
        friend bool operator==(const SpeTask &x, const SpeTask &y) {
            return (x.m_task == y.m_task);
        }
        /** Operator */
        friend bool operator!=(const SpeTask &x, const SpeTask &y) {
            return (x.m_task != y.m_task);
        }

        /** Gets the exit code */
        int getExitCode() {
            int rc, ret = ::ctk_spe_task_get_exit_code(m_task, &rc);
            if (ret != CTK_SUCCESS)
                throw Error(ret);
            return rc;
        }

#ifdef __ALTIVEC__
        /** Gets the return value */
        template <typename R> R getReturnValue() {
            if (m_task == NULL)
                throw Error(CTK_ERROR_NULL_OBJECT);
            if (m_task->status >= CTK_SPE_TASK_STATUS_QUEUED &&
                m_task->status < CTK_SPE_TASK_STATUS_FINISHED) {
                throw Error(CTK_ERROR_BAD_TASK_STATUS);
            }
            return *(R*)&m_task->rc.qwval;
        }
#else /* !__ALTIVEC__ */
        /** Gets the return value */
        template <typename R> R getReturnValue() {
            if (m_task == NULL)
                throw Error(CTK_ERROR_NULL_OBJECT);
            if (m_task->status >= CTK_SPE_TASK_STATUS_QUEUED &&
                m_task->status < CTK_SPE_TASK_STATUS_FINISHED) {
                throw Error(CTK_ERROR_BAD_TASK_STATUS);
            }
            return (R)(m_task->rc.val64[0] >> ((8 - sizeof(R))*8));
        }
#endif /* !__ALTIVEC__ */

        /** Gets the status */
        SpeTaskStatus getStatus() {
            if (m_task == NULL)
                throw Error(CTK_ERROR_NULL_OBJECT);
            return (SpeTaskStatus)m_task->status;
        }

        /** Gets the task ID */
        int getId() {
            if (m_task == NULL)
                throw Error(CTK_ERROR_NULL_OBJECT);
            return m_task->id;
        }

        /** Accesses the task arguments */
        void *getArg(int index) {
            if (m_task == NULL)
                throw Error(CTK_ERROR_NULL_OBJECT);
            if (index < 0 || index >= 2)
                throw Error(CTK_ERROR_OUT_OF_RANGE);
            return (void*)(unsigned long)m_task->arg[index];
        }

        /** Returns true if the task has not an actual task image yet */
        bool empty() { return (m_task == NULL); }

        /** Loads a new task image */
        void load(const SpeTaskImage &image) {
            release();
            m_image = image;
            create(m_image, m_flags);
        }

}; // class SpeTask

}; // namespace ctk

#endif /* _CTK_SPE_TASK_HPP */
