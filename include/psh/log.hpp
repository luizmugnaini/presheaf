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
/// Description: IO stream utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/core.hpp>

namespace psh::impl::log {
    /// Log level.
    ///
    /// The levels are set in an increasing level of vebosity, where `LogLevel::Fatal` is the lowest
    /// and `LogLevel::Debug` is the highest.
    enum struct LogLevel {
        LEVEL_FATAL = 0,
        LEVEL_ERROR,
        LEVEL_WARNING,
        LEVEL_INFO,
        LEVEL_DEBUG,
        LEVEL_COUNT,
    };

    struct LogInfo {
        strptr   file_name;
        strptr   function_name;
        u32      line;
        LogLevel level;
    };

    /// Log a message to the standard error stream.
    void log_msg(LogInfo info, strptr msg);

    /// Log a formatted message to the standard error stream.
    psh_attr_fmt(2) void log_fmt(LogInfo const& info, strptr fmt, ...) noexcept;
}  // namespace psh::impl::log

// -----------------------------------------------------------------------------
// Logging macro utilities intended for easier usage pattern.
// -----------------------------------------------------------------------------

#define psh_impl_make_log_info(log_level)            \
    psh::impl::log::LogInfo {                        \
        .file_name     = psh_source_file_name(),     \
        .function_name = psh_source_function_name(), \
        .line          = psh_source_line_number(),   \
        .level         = log_level,                  \
    }

#if !defined(PSH_DISABLE_LOGGING)
#    define psh_log_fatal(msg)            psh::impl::log::log_msg(psh_impl_make_log_info(psh::impl::log::LogLevel::LEVEL_FATAL), msg)
#    define psh_log_error(msg)            psh::impl::log::log_msg(psh_impl_make_log_info(psh::impl::log::LogLevel::LEVEL_ERROR), msg)
#    define psh_log_warning(msg)          psh::impl::log::log_msg(psh_impl_make_log_info(psh::impl::log::LogLevel::LEVEL_WARNING), msg)
#    define psh_log_info(msg)             psh::impl::log::log_msg(psh_impl_make_log_info(psh::impl::log::LogLevel::LEVEL_INFO), msg)
#    define psh_log_fatal_fmt(fmt, ...)   psh::impl::log::log_fmt(psh_impl_make_log_info(psh::impl::log::LogLevel::LEVEL_FATAL), fmt, __VA_ARGS__)
#    define psh_log_error_fmt(fmt, ...)   psh::impl::log::log_fmt(psh_impl_make_log_info(psh::impl::log::LogLevel::LEVEL_ERROR), fmt, __VA_ARGS__)
#    define psh_log_warning_fmt(fmt, ...) psh::impl::log::log_fmt(psh_impl_make_log_info(psh::impl::log::LogLevel::LEVEL_WARNING), fmt, __VA_ARGS__)
#    define psh_log_info_fmt(fmt, ...)    psh::impl::log::log_fmt(psh_impl_make_log_info(psh::impl::log::LogLevel::LEVEL_INFO), fmt, __VA_ARGS__)
#    if defined(PSH_DEBUG)
#        define psh_log_debug(msg)          psh::impl::log::log_msg(psh_impl_make_log_info(psh::impl::log::LogLevel::LEVEL_DEBUG), msg)
#        define psh_log_debug_fmt(fmt, ...) psh::impl::log::log_fmt(psh_impl_make_log_info(psh::impl::log::LogLevel::LEVEL_DEBUG), fmt, __VA_ARGS__)
#    else
#        define psh_log_debug(msg)          0
#        define psh_log_debug_fmt(fmt, ...) 0
#    endif  // PSH_DEBUG
#else
#    define psh_log_fatal(msg)            0
#    define psh_log_error(msg)            0
#    define psh_log_warning(msg)          0
#    define psh_log_info(msg)             0
#    define psh_log_debug(msg)            0
#    define psh_log_fatal_fmt(fmt, ...)   0
#    define psh_log_error_fmt(fmt, ...)   0
#    define psh_log_warning_fmt(fmt, ...) 0
#    define psh_log_info_fmt(fmt, ...)    0
#    define psh_log_debug_fmt(fmt, ...)   0
#endif

// -----------------------------------------------------------------------------
// Short names.
// -----------------------------------------------------------------------------

#if defined(PSH_DEFINE_SHORT_NAMES)
#    ifndef log_fatal
#        define log_fatal psh_log_fatal
#    endif
#    ifndef log_error
#        define log_error psh_log_error
#    endif
#    ifndef log_warning
#        define log_warning psh_log_warning
#    endif
#    ifndef log_info
#        define log_info psh_log_info
#    endif
#    ifndef log_debug
#        define log_debug psh_log_debug
#    endif
#    ifndef log_fatal_fmt
#        define log_fatal_fmt psh_log_fatal_fmt
#    endif
#    ifndef log_error_fmt
#        define log_error_fmt psh_log_error_fmt
#    endif
#    ifndef log_warning_fmt
#        define log_warning_fmt psh_log_warning_fmt
#    endif
#    ifndef log_info_fmt
#        define log_info_fmt psh_log_info_fmt
#    endif
#    ifndef log_debug_fmt
#        define log_debug_fmt psh_log_debug_fmt
#    endif
#endif
