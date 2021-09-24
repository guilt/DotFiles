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

#ifndef _CTK_CTK_SPU_TASK_HPP
#define _CTK_CTK_SPU_TASK_HPP

#if defined(UNIT_TEST) || (_CTKLIB)
# include "SpuCommon.hpp"
# include "../../task/spu/ctk_spu_task.h"
#else
# include <CtkSpu.hpp>
# include "ctk_spu_task.h"
#endif

namespace ctk {

/**
 * The SPE task main function.  The task application must 
 * implement this.
 * @param[in] id - the SPE task ID
 * @param[in] arg1 - the runtime arg given by the caller
 * @param[in] arg2 - the runtime arg given by the caller
 */
#define ctkTaskMain(id, arg1, arg2)     ctk_task_main(id, arg1, arg2)

class Task {
    public:
        /**
         * Yield the SPE ask and may switch to the new task
         */
        static int yield() { return ctk_task_yield(); }

        /**
         * Exit the current SPE task (note: if the application directly calls
         * ::exit() instead of #ctk_task_exit, the SPE task runtime itself 
         * exits.Do not call ::exit() directly unless you are aware of what
         * you are doing.)
         * @param[in] ret - the return code
         */
        static int exit(unsigned long long ret) { ctk_task_exit(ret); }

        /**
         * Execs a new SPE task on this SPE.  The current task will be
         * overwritten and never returns from the call.
         * @param[in] image - the task image (created by #ctk_task_image_create
         *   or #ctk_task_image_open)
         * @param[in] arg1 - the 2nd arg for the new task
         * @param[in] arg2 - the 3rd arg for the new task
         */
        static int exec(ea_addr_t image, unsigned long long arg1, unsigned long long arg2)
        { 
            return ctk_task_exec(image, arg1, arg2);
        }

        /**
         * Spawns a new SPE task on this SPE.  The new created task will
         * be enqueued to the run queue and eventually called.
         * @param[in] task - the task reference, which should not already
         * be running or in the run queue.
         * @param[in] arg1 - the 2nd arg for the new task
         * @param[in] arg2 - the 3rd arg for the new task
         */
        static int spawn(ea_addr_t task, unsigned long long arg1, unsigned long long arg2)
        {
            return ctk_task_spawn(task, arg1, arg2);
        }
}; // class Task

/** This task - the single task instance (this header file
    is assumed to be included only once!) */
Task task;

};

#endif /* _CTK_CTK_SPU_TASK_HPP */
