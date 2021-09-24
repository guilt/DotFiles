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

#ifndef _CTK_TRACE_H
#define _CTK_TRACE_H

/**
 * @file ctk_trace.h
 * @author kinuko@fixstars.com
 * @brief Definitions for CTK trace logging routines
 *
 * Any error, debug or informative messages can be displayed via one of
 * either trace macros; #trace_log(level, fmt, ...), 
 * #trace_fatal(fmt, ...), #trace_error(fmt, ...), #trace_warn(fmt, ...), 
 * #trace_warn(fmt, ...), #trace_info(fmt, ...), #trace_debug(fmt, ...) or
 * #trace_trace(fmt, ...).
 * All the macros takes variable number of arguments like printf.  trace_log
 * is the most primary logging function which is called from other macros
 * internally.  It takes log level as the first argument, which can be
 * either one of #LOGLEVEL_FATAL, #LOGLEVEL_ERROR, #LOGLEVEL_WARN,
 * #LOGLEVEL_INFO, #LOGLEVEL_DEBUG or #LOGLEVEL_TRACE. 
 * Alternatively you can call one of other convenient macros corresponding
 * to each log level (e.g. #trace_error()).
 * The base logging level, which specifies what level of log messages should
 * be shown, can be specified by #trace_set_log_level().  The printf callback
 * function which is used for displaying the message can also be set by calling
 * #trace_set_log_vprintf() (e.g. to let it call syslog or other external
 * logging faciility).
 */

#define LOGLEVEL_FATAL	1	/**< log level for fatal errors */
#define LOGLEVEL_ERROR	2	/**< log level for errors */
#define LOGLEVEL_WARN	3	/**< log level for warnings */
#define LOGLEVEL_INFO	4	/**< log level for informative messages */
#define LOGLEVEL_DEBUG	5	/**< log level for debug messages */
#define LOGLEVEL_TRACE	6	/**< log level for trace messages */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __STDC__
/** A macro to log trace messages */
# define	trace_log	(trace_log_locate2(__FILE__, __func__, __LINE__), trace_log_body)

/** A macro to log fatal error messages. */
# define	trace_fatal	(trace_log_locate2(__FILE__, __func__, __LINE__), trace_fatal_body)
/** A macro to log error messages. */
# define	trace_error	(trace_log_locate2(__FILE__, __func__, __LINE__), trace_error_body)
/** A macro to log warning messages. */
# define	trace_warn	(trace_log_locate2(__FILE__, __func__, __LINE__), trace_warn_body)
/** A macro to log info messages. */
# define	trace_info	(trace_log_locate2(__FILE__, __func__, __LINE__), trace_info_body)
/** A macro to log debug messages. */
# define	trace_debug	(trace_log_locate2(__FILE__, __func__, __LINE__), trace_debug_body)
/** A macro to log trace messages. */
# define	trace_trace	(trace_log_locate2(__FILE__, __func__, __LINE__), trace_trace_body)
#else /* ! __STDC__ */
/** A macro to log trace messages. */
# define	trace_log	(trace_log_locate(__FILE__, __LINE__), trace_log_body)

/** A macro to log fatal error messages. */
# define	trace_fatal	(trace_log_locate(__FILE__, __LINE__), trace_fatal_body)
/** A macro to log error messages. */
# define	trace_error	(trace_log_locate(__FILE__, __LINE__), trace_error_body)
/** A macro to log warning messages. */
# define	trace_warn	(trace_log_locate(__FILE__, __LINE__), trace_warn_body)
/** A macro to log info messages. */
# define	trace_info	(trace_log_locate(__FILE__, __LINE__), trace_info_body)
/** A macro to log debug messages. */
# define	trace_debug	(trace_log_locate(__FILE__, __LINE__), trace_debug_body)
/** A macro to log trace messages. */
# define	trace_trace	(trace_log_locate(__FILE__, __LINE__), trace_trace_body)
#endif /* __STDC__ */

/*
#ifdef DEBUG
# define	trace_debug	(trace_log_locate(__FILE__, __LINE__), trace_debug_body)
# define	trace_trace	(trace_log_locate(__FILE__, __LINE__), trace_trace_body)
#else
# define	trace_debug	
# define	trace_trace
#endif
*/

# include <stdarg.h>

/**
 * A typedef of printf callback function for logging.
 */
typedef int (*vprintf_callback_t)(const char *fmt, va_list ap);

void trace_set_log_level(int level);
int trace_get_log_level();
void trace_set_log_vprintf(vprintf_callback_t callback);
vprintf_callback_t trace_get_log_vprintf();

void trace_log_locate(const char *file, int line);
void trace_log_locate2(const char *file, const char *func, int line);
void trace_log_body(int level, const char *fmt, ...);

/* Logger function for fatal error messages.  The macro #trace_fatal() should
   be used instead of this function. */
void trace_fatal_body(const char *fmt, ...);

/* Logger function for error messages.  The macro #trace_error() should
   be used instead of this function.  */
void trace_error_body(const char *fmt, ...);

/* Logger function for warning messages.  The macro #trace_warn() should
   be used instead of this function.  */
void trace_warn_body(const char *fmt, ...);

/* Logger function for info messages.  The macro #trace_info() should
   be used instead of this function.  */
void trace_info_body(const char *fmt, ...);

/* Logger function for debug messages.  The macro #trace_debug() should
   be used instead of this function.  */
void trace_debug_body(const char *fmt, ...);

/* Logger function for trace messages.  The macro #trace_trace() should
   be used instead of this function.  */
void trace_trace_body(const char *fmt, ...);

/** A flag variable which determines whether the logger should print out
    timestamp or not. (default=1) */
extern unsigned int trace_log_timestamp;

#ifdef __cplusplus
}
#endif

#endif /* _CTK_TRACE_H */
