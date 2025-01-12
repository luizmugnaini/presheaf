///                             Presheaf library
/// Copyright (C) 2024 Luiz Gustavo Mugnaini Anselmo
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of
/// this software and associated documentation files (the “Software”), to deal in
/// the Software without restriction, including without limitation the rights to
/// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
/// of the Software, and to permit persons to whom the Software is furnished to do
/// so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///
/// Description: Implementation of debugging utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include "psh_debug.hpp"

#include "psh_platform.hpp"

#if PSH_ENABLE_USE_STB_SPRINTF
#    include "psh_string.hpp"
#endif
#include <stdarg.h>
#include <stdio.h>

// -------------------------------------------------------------------------------------------------
// Implementation of the abortion procedures
// -------------------------------------------------------------------------------------------------

// Implementation details.
namespace psh::impl {
    psh_internal void default_abort_function(void* arg) psh_no_except {
        psh_discard_value(arg);

#if PSH_COMPILER_MSVC
        __debugbreak();
#elif PSH_COMPILER_CLANG || PSH_COMPILER_GCC
        __builtin_trap();
#else
        // Stall the program if we don't have a sane default.
        for (;;) {}
#endif
    };

    psh_internal void*          abort_context  = nullptr;
    psh_internal AbortFunction* abort_function = default_abort_function;
}  // namespace psh::impl

namespace psh {
    void set_abort_function(AbortFunction* abort_function, void* abort_context) psh_no_except {
        impl::abort_context  = abort_context;
        impl::abort_function = abort_function;
    }

    psh_proc void abort_program() psh_no_except {
        impl::abort_function(impl::abort_context);
    }
}  // namespace psh

// -------------------------------------------------------------------------------------------------
// Tweakable log message length parameters.
//
// If your logs are being truncated, you may have to tweak the parameters found in this section.
// -------------------------------------------------------------------------------------------------

#if !defined(PSH_LOG_MSG_MAX_LENGTH)
#    define PSH_LOG_MSG_MAX_LENGTH 8192
#endif

/// The log header hast to have enough space to fit the log level string and the caller file name,
/// line number, and function name.
#if !defined(PSH_LOG_HEADER_MAX_LENGTH)
#    define PSH_LOG_HEADER_MAX_LENGTH 128
#endif

// -------------------------------------------------------------------------------------------------
// Logging implementation.
// -------------------------------------------------------------------------------------------------

#define PSH_LOG_HEADER_FMT "%s [%s:%u:%s]"

namespace psh::impl {
    // @TODO:
    // - Enable the user to set a different output stream for the logs.
    // - Remove dependency on stdio.h for windows by using WriteFile.
    // - Use stb_sprintf for formatting.
    constexpr usize LOG_RESULT_MSG_MAX_LENGTH = PSH_LOG_HEADER_MAX_LENGTH + PSH_LOG_MSG_MAX_LENGTH;

    constexpr Buffer<cstring, LOG_LEVEL_COUNT> LOG_LEVEL_CSTRING = {
#if PSH_ENABLE_ANSI_COLOURS
        "\x1b[1;41m[FATAL]\x1b[0m",
        "\x1b[1;31m[ERROR]\x1b[0m",
        "\x1b[1;33m[WARNING]\x1b[0m",
        "\x1b[1;32m[INFO]\x1b[0m",
        "\x1b[1;34m[DEBUG]\x1b[0m",
#else
        "[FATAL]",
        "[ERROR]",
        "[WARNING]",
        "[INFO]",
        "[DEBUG]",
#endif
    };

#if PSH_ENABLE_USE_STB_SPRINTF
    psh_proc void log_msg(LogInfo info, cstring msg) psh_no_except {
        Buffer<char, LOG_RESULT_MSG_MAX_LENGTH> result_msg;
        {
            i32 result_msg_length = string_format(
                result_msg.buf,
                result_msg.count,
                PSH_LOG_HEADER_FMT " %s\n",
                LOG_LEVEL_CSTRING[info.level],
                info.file_name,
                info.line,
                info.function_name,
                msg);
            if (psh_unlikely(result_msg_length < 0)) {
                fprintf(stderr, "[ERROR] Failed to format logging message.");
                return;
            }
        }
        psh_discard_value(fprintf(stdout, "%s", result_msg.buf));
    }

    psh_proc void log_fmt(LogInfo const& info, cstring fmt, ...) psh_no_except {
        Buffer<char, LOG_RESULT_MSG_MAX_LENGTH> result_msg;
        {
            i32 header_length = string_format(
                result_msg.buf,
                PSH_LOG_HEADER_MAX_LENGTH,
                PSH_LOG_HEADER_FMT,
                LOG_LEVEL_CSTRING[info.level],
                info.file_name,
                info.line,
                info.function_name);
            if (psh_unlikely(header_length < 0)) {
                psh_log_fatal("Failed to format the header of the logging message.");
            }

            // The resulting log message will have an additional space between the header and
            // message, as well as a new-line escape sequence at the end of the end of the message
            // and a null-terminator.
            constexpr usize LOG_MSG_MAX_EFFECTIVE_LENGTH = PSH_LOG_MSG_MAX_LENGTH - 3;

            result_msg.buf[header_length] = ' ';

            char* msg_buf = result_msg.buf + (header_length + 1);

            va_list args;
            va_start(args, fmt);
            {
                i32 msg_length = string_format_list(
                    msg_buf,
                    LOG_MSG_MAX_EFFECTIVE_LENGTH,
                    fmt,
                    args);
                if (psh_unlikely(msg_length < 0)) {
                    psh_log_error("Failed to parse user the formatted message string.");
                    return;
                }

                msg_buf[msg_length]     = '\n';
                msg_buf[msg_length + 1] = 0;
            }
            va_end(args);
        }

        psh_discard_value(fprintf(stdout, "%s", result_msg.buf));
    }
#else   // !PSH_ENABLE_USE_STB_SPRINTF - use libc functions.
    psh_proc void log_msg(LogInfo info, cstring msg) psh_no_except {
        psh_discard_value(fprintf(
            stderr,
            PSH_LOG_HEADER_FMT " %s\n",
            LOG_LEVEL_CSTRING[info.level],
            info.file_name,
            info.line,
            info.function_name,
            msg));
    }

    psh_internal ph_attribute_fmt(2) void log_fmt(LogInfo const& info, cstring fmt, ...) psh_no_except {
        Buffer<char, LOG_RESULT_MSG_MAX_LENGTH> result_msg;

        va_list args;
        va_start(args, fmt);
        {
            // Format the message with the given arguments.
            i32 result_length = vsnprintf(result_msg.buf, result_msg.count, fmt, args);
            if (result_length < 0) {
                psh_log_fatal("snptrintf unable to parse the format string and arguments");
                abort_program();
            }

            // Stamp the message with a null-terminator.
            usize effective_msg_length       = psh_min_value(static_cast<usize>(result_length), result_msg.count);
            result_msg[effective_msg_length] = 0;
        }
        va_end(args);

        psh_discard_value(fprintf(
            stdout,
            PSH_LOG_HEADER_FMT " %s\n",
            LOG_LEVEL_CSTRING[info.level],
            info.file_name,
            info.line,
            info.function_name,
            result_msg.buf));
    }
#endif  // PSH_ENABLE_USE_STB_SPRINTF
}  // namespace psh::impl
