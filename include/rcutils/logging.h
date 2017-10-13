// Copyright 2017 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RCUTILS__LOGGING_H_
#define RCUTILS__LOGGING_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "rcutils/allocator.h"
#include "rcutils/visibility_control.h"

#if __cplusplus
extern "C"
{
#endif

/// The flag if the logging system has been initialized.
RCUTILS_PUBLIC
extern bool g_rcutils_logging_initialized;

/// Initialize the logging system using the specified allocator.
/**
 * This function must be called before any configuration of the system can be
 * performed, and before any log calls can be made.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param allocator rcutils_allocator_t to be used.
 */
RCUTILS_PUBLIC
void rcutils_logging_initialize_with_allocator(rcutils_allocator_t allocator);

/// Initialize the logging system.
/**
 * Call rcutils_logging_initialize_with_allocator() using the default allocator.
 * This function is called automatically when using the logging macros.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 */
RCUTILS_PUBLIC
void rcutils_logging_initialize();

/// Shutdown the logging system.
/**
 * Free the resources allocated for the logging system.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 */
RCUTILS_PUBLIC
void rcutils_logging_shutdown();

/// The structure identifying the caller location in the source code.
typedef struct rcutils_log_location_t
{
  /// The name of the function containing the log call.
  const char * function_name;
  /// The name of the source file containing the log call.
  const char * file_name;
  /// The line number containing the log call.
  size_t line_number;
} rcutils_log_location_t;

/// The severity levels of log messages / logger thresholds.
enum RCUTILS_LOG_SEVERITY
{
  RCUTILS_LOG_SEVERITY_DEBUG = 0,  ///< The debug log level
  RCUTILS_LOG_SEVERITY_INFO = 1,  ///< The info log level
  RCUTILS_LOG_SEVERITY_WARN = 2,  ///< The warn log level
  RCUTILS_LOG_SEVERITY_ERROR = 3,  ///< The error log level
  RCUTILS_LOG_SEVERITY_FATAL = 4,  ///< The fatal log level
  RCUTILS_LOG_SEVERITY_UNSET = 100,  ///< The unset log level
};

/// The function signature to log messages.
/**
 * \param The pointer to the location struct
 * \param The severity level
 * \param The name of the logger
 * \param The format string
 * \param The variable argument list
 */
typedef void (* rcutils_logging_output_handler_t)(
  rcutils_log_location_t *,  // location
  int,  // severity
  const char *,  // name
  const char *,  // format
  va_list *  // args
);

/// The function pointer of the current output handler.
RCUTILS_PUBLIC
extern rcutils_logging_output_handler_t g_rcutils_logging_output_handler;

/// Get the current output handler.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \return The function pointer of the current output handler.
 */
RCUTILS_PUBLIC
rcutils_logging_output_handler_t rcutils_logging_get_output_handler();

/// Set the current output handler.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param function The function pointer of the output handler to be used.
 */
RCUTILS_PUBLIC
void rcutils_logging_set_output_handler(rcutils_logging_output_handler_t function);

/// The default severity threshold for log calls.
/**
 * This severity threshold is used for nameless log calls and loggers with
 * effective severity thresholds that are otherwise unspecified.
 */
RCUTILS_PUBLIC
extern int g_rcutils_logging_default_severity_threshold;

/// Get the default severity threshold for loggers.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \return The severity threshold.
 */
RCUTILS_PUBLIC
int rcutils_logging_get_default_severity_threshold();

/// Set the default severity threshold for loggers.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param severity The severity threshold to be used.
 */
RCUTILS_PUBLIC
void rcutils_logging_set_default_severity_threshold(int severity);

/// Get the severity threshold for a logger.
/**
 * This considers the severity threshold of the specifed logger only.
 * To get the effective severity threshold of a logger given the severity
 * threshold of its ancestors, see
 * rcutils_logging_get_logger_effective_threshold().
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param name The name of the logger, must be null terminated c string
 * \return The severity threshold if it has been set, or
 * \return `RCUTILS_LOG_SEVERITY_UNSET` if unset
 */
RCUTILS_PUBLIC
int rcutils_logging_get_logger_severity_threshold(const char * name);

/// Get the severity threshold for a logger and its name length.
/**
 * Identical to rcutils_logging_get_logger_severity_threshold() but without
 * relying on the logger name to be a null terminated c string.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param name The name of the logger
 * \param name_length Logger name length
 * \return The severity threshold if it has been set, or
 * \return `RCUTILS_LOG_SEVERITY_UNSET` if unset
 */
RCUTILS_PUBLIC
int rcutils_logging_get_logger_severity_thresholdn(const char * name, size_t name_length);

/// Set the severity threshold for a logger.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | Yes
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param name The name of the logger, must be null terminated c string.
 * \param severity The severity threshold to be used.
 */
RCUTILS_PUBLIC
void rcutils_logging_set_logger_severity_threshold(const char * name, int severity);

/// Determine if a logger is enabled for a severity.
/**
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param The name of the logger, must be null terminated c string or NULL.
 * \param The severity level.
 *
 * \return True if the logger is enabled for the severity; false otherwise.
 */
RCUTILS_PUBLIC
bool rcutils_logging_is_enabled_for(const char * name, int severity);

/// Determine the effective severity threshold for a logger.
/**
 * The effective severity threshold is determined as the severity threshold of
 * the logger if it is set, otherwise it is the first specified severity
 * threshold of the logger's ancestors, starting with its closest ancestor.
 * The ancestor hierarchy is signified by logger names being separated by dots:
 * a logger named `x` is an ancestor of `x.y`, and both `x` and `x.y` are
 * ancestors of `x.y.z`, etc.
 * If the severity threshold has not been set for the logger nor any of its
 * ancestors, the default severity threshold is used.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param The name of the logger, must be null terminated c string.
 *
 * \return The severity threshold.
 */
RCUTILS_PUBLIC
int rcutils_logging_get_logger_effective_threshold(const char * name);

/// Log a message.
/**
 * The attributes of this function are also being influenced by the currently
 * set output handler.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No
 * Thread-Safe        | No
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param location The pointer to the location struct or NULL
 * \param severity The severity level
 * \param name The name of the logger, must be null terminated c string or NULL
 * \param format The format string
 * \param ... The variable arguments
 */
RCUTILS_PUBLIC
void rcutils_log(
  rcutils_log_location_t * location,
  int severity,
  const char * name,
  const char * format,
  ...);

/// The default output handler outputs log messages to the standard streams.
/**
 * The messages with a severity `DEBUG` and `INFO` are written to `stdout`.
 * The messages with a severity `WARN`, `ERROR`, and `FATAL` are written to
 * `stderr`.
 * The console output format of the logged message can be configured through
 * the `RCUTILS_CONSOLE_OUTPUT_FORMAT` environment variable.
 * By default, the severity and name is prepended and the location
 * information is appended.
 *
 * <hr>
 * Attribute          | Adherence
 * ------------------ | -------------
 * Allocates Memory   | No, for formatted outputs <= 1023 characters
 *                    | Yes, for formatted outputs >= 1024 characters
 * Thread-Safe        | Yes, if the underlying *printf functions are
 * Uses Atomics       | No
 * Lock-Free          | Yes
 *
 * \param location The pointer to the location struct or NULL
 * \param severity The severity level
 * \param name The name of the logger, must be null terminated c string
 * \param format The format string for the message contents
 * \param args The variable argument list for the message format string
 */
RCUTILS_PUBLIC
void rcutils_logging_console_output_handler(
  rcutils_log_location_t * location,
  int severity, const char * name, const char * format, va_list * args);

// Provide the compiler with branch prediction information
#ifndef _WIN32
/**
 * \def RCUTILS_LIKELY
 * Instruct the compiler to optimize for the case where the argument equals 1.
 */
# define RCUTILS_LIKELY(x) __builtin_expect((x), 1)
/**
 * \def RCUTILS_UNLIKELY
 * Instruct the compiler to optimize for the case where the argument equals 0.
 */
# define RCUTILS_UNLIKELY(x) __builtin_expect((x), 0)
#else
/**
 * \def RCUTILS_LIKELY
 * No op since Windows doesn't support providing branch prediction information.
 */
# define RCUTILS_LIKELY(x) (x)
/**
 * \def RCUTILS_UNLIKELY
 * No op since Windows doesn't support providing branch prediction information.
 */
# define RCUTILS_UNLIKELY(x) (x)
#endif  // _WIN32

/**
 * \def RCUTILS_LOGGING_AUTOINIT
 * \brief Initialize the rcl logging library.
 * Usually it is unnecessary to call the macro directly.
 * All logging macros ensure that this has been called once.
 */
#define RCUTILS_LOGGING_AUTOINIT \
  if (RCUTILS_UNLIKELY(!g_rcutils_logging_initialized)) { \
    rcutils_logging_initialize(); \
  }

#if __cplusplus
}
#endif

#endif  // RCUTILS__LOGGING_H_
