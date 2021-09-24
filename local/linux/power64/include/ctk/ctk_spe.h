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

#ifndef _CTK_SPE_H
#define _CTK_SPE_H

/**
 * @file ctk_spe.h
 * @brief Definitions for SPE management layer
 * @note  On libspe2 or tSDK, SPE context provides 'light weight' mechanism
 *   as compared with SPE threads.
 *   However, on libspe, using SPE context won't make any difference from
 *   SPE threads in terms of performance.  It still generates one PPE thread
 *   per one SPE thread.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "ctk_version.h"
#include "ctk_types.h"
#include "ctk_elf.h"
#include "ctk_env.h"

#include <stdbool.h>

enum {
    CTK_SPE_STATUS_UNKNOWN = 0, /**< SPE status - unknown or error */
    CTK_SPE_STATUS_CREATED = 1, /**< SPE status - created */
    CTK_SPE_STATUS_LOADED  = 2, /**< SPE status - program loaded */
    CTK_SPE_STATUS_READY   = 3, /**< SPE status - thread is ready to run */
    CTK_SPE_STATUS_RUNNING = 4, /**< SPE status - running */
    CTK_SPE_STATUS_STOPPED = 5, /**< SPE status - stopped */
};

/* SPE abstract structure */
typedef struct ctk_spe_context_abstract {
    unsigned int m_type;
    unsigned int m_counter;
    volatile unsigned int m_status;
    ctk_env_t *m_env;
    pthread_mutex_t m_status_lock;
    bool m_stopped;

    void *m_native_spe;
} ctk_spe_context_abstract_t;

/** SPE context ID type */
typedef unsigned long long ctk_spe_id_t;

/** SPE access hint ID type */
typedef ctk_uint64_t ctk_hint_id_t;

#define CTK_SPE_MAX_ACCESS_HINTS   16

#ifdef CELL_ENV_TSDK
# include "ctk_spe-tsdk.h"
#elif CELL_ENV_LIBSPE
# include "ctk_spe-libspe.h"
#elif CELL_ENV_LIBSPE2
# include "ctk_spe-libspe2.h"
#endif

/** SPE Program handle */
typedef _ctk_spe_program_t ctk_spe_program_t;

/** SPE context pointer */
typedef _ctk_spe_context_t ctk_spe_context_t;

/** Macro to obtain SPE context ID for the given SPE context.
 * @param[in] spe - SPE context
 * @return the SPE context ID
 * @note declared here mainly for documentation.
 */
#ifdef _CTK_SPE_ID
# define CTK_SPE_ID(spe)          _CTK_SPE_ID(spe)
#else
# define CTK_SPE_ID(spe)          (ctk_spe_id_t)CTK_SPE_NATIVE(spe)
#endif

/** Macro to obtain SPE environment structure for the given SPE context.
 * @param[in] spe - SPE context
 * @return an instance of ctk_env_t for the SPE
 * @note declared here mainly for documentation.
 */
#define CTK_SPE_ENV(spe)        (((ctk_spe_context_abstract_t *)spe)->m_env)
/* #define CTK_SPE_ENV(spe)      _CTK_SPE_ENV(spe) */

/** Macro to obtain SPE status for the given SPE context.
 * @param[in] spe - SPE context
 * @return an instance of ctk_env_t for the SPE
 * @note declared here mainly for documentation.
 */
#define CTK_SPE_STATUS(spe) (((ctk_spe_context_abstract_t *)spe)->m_status)
/* #define CTK_SPE_STATUS(spe)  _CTK_SPE_STATUS(spe) */

/*----------------------------------------------------------------*/
#ifndef INLINE_CTK_SPE_IMAGE_OPEN
/**
 * Opens the given SPE ELF image and returns the SPE program handle.
 * @param[out] program - pointer to the SPE program handle
 * @param[in] filename - ELF image file 
 * @return CTK_ERROR (0) on success, errno (non-zero) on failure.
 */
int ctk_spe_image_open(ctk_spe_program_t **program, const char *filename);
#endif /* !INLINE_CTK_SPE_IMAGE_OPEN */

#ifndef INLINE_CTK_SPE_IMAGE_CLOSE
/**
 * Closes the given SPE program handle and frees the resources.
 * @param[in] handle - the SPE program handle to be freed.
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_image_close(ctk_spe_program_t *handle);
#endif /* !INLINE_CTK_SPE_IMAGE_CLOSE */

/*---------------------------------------------------------------------*/
/*
  Flags for ctk_spe_context_create
 */
#ifdef SPE_CFG_SIGNOTIFY1_OR
# define CTK_SPE_CFG_SIGNOTIFY1_OR   SPE_CFG_SIGNOTIFY1_OR
# define CTK_SPE_CFG_SIGNOTIFY2_OR   SPE_CFG_SIGNOTIFY2_OR
#else /* !SPE_CFG_SIGNOTIFY1_OR */
/** Flag for ctk_spe_context_create.  (Borrowed from libspe spec.)
  * Configure the Signal Notification 1 Register to be in "logical OR"
  * mode instead of the default "Overwrite" mode. */
# define CTK_SPE_CFG_SIGNOTIFY1_OR   0x00000010

/** Flag for ctk_spe_context_create.  (Borrowed from libspe spec.)
  * Configure the Signal Notification 2 Register to be in "logical OR"
  * mode instead of the default "Overwrite" mode. */
# define CTK_SPE_CFG_SIGNOTIFY2_OR   0x00000020
#endif /* !SPE_CFG_SIGNOTIFY1_OR */

/*
  Flags for ctk_spe_context_run
 */
#if defined(SPE_USER_REGS)           /* libspe */
# define CTK_SPE_RUN_USER_REGS       SPE_USER_REGS
#elif defined(SPE_RUN_USER_REGS)     /* libspe2 */
# define CTK_SPE_RUN_USER_REGS       SPE_RUN_USER_REGS
#else /* !SPE_USER_REGS && !SPE_RUN_USER_REGS */
/** Flag for ctk_spe_context_run.  (Borrowed from libspe2 spec.)
  * Specifies that the SPE setup registers r3, r4, and r5 are
  * initialized with the 48 bytes pointed to by argp. */
# define CTK_SPE_RUN_USER_REGS	 0x00000001
#endif /* !SPE_USER_REGS && !SPE_RUN_USER_REGS */

#ifdef SPE_NO_CALLBACKS
# define CTK_SPE_NO_CALLBACKS        SPE_NO_CALLBACKS
# define CTK_SPE_ISOLATE             SPE_ISOLATE
# define CTK_SPE_EVENTS_ON           SPE_EVENTS_ON
#else /* !SPE_NO_CALLBACKS */
/** Flag for ctk_spe_context_run.  (Borrowed from libspe2 spec.)
  * Specifies that registered SPE library calls ("callbacks" from
  * this library's view) should not be automatically executed. If
  * a callback is encountered. This also disables callbacks that are
  * predefined in the library implementation.
  * @note this flag will take no effect with libspe1.
  */
# define CTK_SPE_NO_CALLBACKS	0x00000002
#endif /* !SPE_NO_CALLBACKS */

/* internal flag for ctk_spe_run_create */
#define CTK_SPE_PASS_RAW_ENV   0x00020000

/**
 * SPE default entry (for 2nd arg of #ctk_spe_context_run)
 */
#ifdef SPE_DEFAULT_ENTRY
# define CTK_SPE_DEFAULT_ENTRY  SPE_DEFAULT_ENTRY
#else
# define CTK_SPE_DEFAULT_ENTRY  0xffffffff
#endif

/*---------------------------------------------------------------------*/
/**
 * Returns the number of available SPEs. (May not return the
 * number of actual / physical SPEs)
 */
int ctk_env_count_spes();

#ifndef INLINE_CTK_SPE_IMAGE_OPEN
/**
 * Opens the given SPE ELF image and returns the SPE program handle.
 * @param[out] program - pointer to the SPE program handle
 * @param[in] filename - ELF image file 
 * @return CTK_ERROR (0) on success, errno (non-zero) on failure.
 */
int ctk_spe_image_open(ctk_spe_program_t **program, const char *filename);
#endif /* !INLINE_CTK_SPE_IMAGE_OPEN */

#ifndef INLINE_CTK_SPE_IMAGE_CLOSE
/**
 * Closes the given SPE program handle and frees the resources.
 * @param[in] handle - the SPE program handle to be freed.
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_image_close(ctk_spe_program_t *handle);
#endif /* !INLINE_CTK_SPE_IMAGE_CLOSE */

/*---------------------------------------------------------------------*/
/**
 * Creates a new SPE context
 * @param[out] spe - pointer to store the created SPE context
 * @param[in] flags - A bit-wise OR of modifiers that are applied
 *   when the new context is created.  0 (no flag) or any combination of
 *   #CTK_SPE_CFG_SIGNOTIFY1_OR and #CTK_SPE_CFG_SIGNOTIFY2_OR
 *   can be specified.
 * @param[in] info - [reserved]
 * @return CTK_SUCCESS (0) on success, non-zero otherwise.
 */
int ctk_spe_context_create(ctk_spe_context_t *spe, unsigned int flags, void *info);

/**
 * Loads the SPE program into the given SPE context
 * @param[in] spe - the SPE context
 * @param[in] program - the SPE program handle
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_context_load(ctk_spe_context_t spe, ctk_spe_program_t *program);

/**
 * Runs the given SPE context.
 * @param[in] spe - the SPE context
 * @param[in,out] entry - the SPE context entry point (not supported on
 *   libspe1.)
 *   If you have no idea what value to specify, use #CTK_SPE_DEFAULT_ENTRY.
 * @param[in] flags - A bit-wise OR of modifiers that are applied
 *   when the new context is created.  0 (no flag) or combination of
 *   #CTK_SPE_RUN_USER_REGS and #CTK_SPE_NO_CALLBACKS
 *   can be specified.
 * @param[in] argp - Argument pointer for the SPE (2nd arg of the SPE main)
 * @param[in] envp - Env pointer for the SPE (3rd arg of the SPE main)
 * @param[out] status - the SPE exit status.
 *   The return code of the SPE program can be obtained by a macro
 *   #CTK_SPE_EXIT_STATUS(status) if #CTK_SPE_IF_EXITED(status) is true.
 * @param[out] stop_code - the raw SPE stop code (not supported on libspe1)
 * @return CTK_SUCCESS on success, ERRNO on failure.
 *
 * @note the #entry and #stop_code parameters are not supported on Libspe1.
 */
int ctk_spe_context_run(ctk_spe_context_t spe, ls_addr_t *entry, unsigned int runflags, void *argp, void *envp, int *status, int *stop_code);

/**
 * Stops the given SPE context. (This operaton may not be support
 * by low level libraries)
 * @param[in] spe - the SPE context
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_context_stop(ctk_spe_context_t spe);

/**
 * Destroys the given SPE context.
 * @param[in] spe - the SPE context
 * @return CTK_SUCCESS on success, ERRNO on failure.
 */
int ctk_spe_context_destroy(ctk_spe_context_t spe);

/**
 * Returns SPE status of the given SPE context.
 * @param[in] spe - the SPE context
 * @return either one of #CTK_SPE_STATUS_CREATED, #CTK_SPE_STATUS_LOADED,
 *   #CTK_SPE_STATUS_READY, #CTK_SPE_STATUS_RUNNING, #CTK_SPE_STATUS_STOPPED
 *   or #CTK_SPE_STATUS_UNKNOWN.
 */
static inline int ctk_spe_context_get_status(ctk_spe_context_t spe) __attribute__((always_inline)); 
static inline int ctk_spe_context_get_status(ctk_spe_context_t spe) {
    return CTK_SPE_STATUS(spe);
}

#include <sys/wait.h>

/** Returns 1 (true) if the SPE has exited normally
 * @param[in] status - the SPE exit/stop status code returned via
 * the 5th arg of #ctk_spe_context_run.
 */
#define CTK_SPE_IF_EXITED(status)       WIFEXITED(status)

/** Returns the SPE exit code (meaningful only if #CTK_SPE_IF_EXITED(status)
 * is true)
 * @param[in] status - the SPE exit/stop status code returned via
 * the 5th arg of #ctk_spe_context_run.
 */
#define CTK_SPE_EXIT_STATUS(status)     WEXITSTATUS(status)

/** Returns the signal code which stopped the SPE
 * (meaningful only if #CTK_SPE_IF_EXITED(status) is false
 * @param[in] status - the SPE exit/stop status code returned via
 * the 5th arg of #ctk_spe_context_run.
 */
#define CTK_SPE_EXIT_SIGNAL(status)     WTERMSIG(status)

/*-----------------------------------------------------------------------*/

#if !defined(DOXYGEN)
/* internal functions */
int ctk_spe_context_map_ls(ctk_spe_context_t spe, ea_addr_t *map_addr);
int ctk_spe_context_map_signal1(ctk_spe_context_t spe, ea_addr_t *map_addr);
int ctk_spe_context_map_signal2(ctk_spe_context_t spe, ea_addr_t *map_addr);
#if !defined(CELL_ENV_TSDK)
int ctk_spe_context_map_ps(ctk_spe_context_t spe, void **ps, enum ps_area area);
#endif
#endif /* !DOXYGEN */

/**
 * Add access hint from the given SPE context (meaningful only for tSDK env)
 * @see #ctk_spe_add_access_hint
 */
#ifdef CELL_ENV_TSDK
int ctk_spe_context_add_access_hint(ctk_spe_context_t spe, void *addr, ea_size_t size);
#else /* !CELL_ENV_TSDK */
static inline int ctk_spe_context_add_access_hint(ctk_spe_context_t spe, void *addr, ea_size_t size)
{
    /* just touch the area */
    int i = 0;
    int lines = size / 16;
    volatile char c[lines];
    for (i = 0; i < lines; i++) {
        c[i] = *(char*)((unsigned long)addr + i * 16);
    }
    return CTK_SUCCESS;
}
#endif /*! CELL_ENV_TSDK */

/**
 * Remove access hint from the given SPE context (meaningful only for tSDK env)
 * @see #ctk_spe_remove_access_hint
 */
#ifdef CELL_ENV_TSDK
int ctk_spe_context_remove_access_hint(ctk_spe_context_t spe, void *addr);
#else /* !CELL_ENV_TSDK */
static inline int ctk_spe_context_remove_access_hint(ctk_spe_context_t spe, void *addr)
{
    /* not supported */
    /* return CTK_ERROR_NOT_SUPPORTED; */
    return CTK_SUCCESS;
}
#endif /*! CELL_ENV_TSDK */

/**
 * Registers a new spu-library handler function.
 * @param[in] handler - the handler function
 * @param[in] callnum - the callnum with which to register the handler
 */
int ctk_spe_register_handler(void *handler, unsigned int callnum);

#ifdef __cplusplus
}
#endif

#endif /* _CTK_SPE_H */
