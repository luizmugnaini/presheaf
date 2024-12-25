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
    /// The levels are set in an increasing level of vebosity, where LogLevel::Fatal is the lowest
    /// and LogLevel::Debug is the highest.
    enum LogLevel : u32 {
        LOG_LEVEL_FATAL = 0,
        LOG_LEVEL_ERROR,
        LOG_LEVEL_WARNING,
        LOG_LEVEL_INFO,
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_COUNT,
    };

    struct psh_api LogInfo {
        strptr   file_name;
        strptr   function_name;
        u32      line;
        LogLevel level;
    };

    /// Log a message to the standard error stream.
    psh_api void log_msg(LogInfo info, strptr msg) psh_no_except;

    /// Log a formatted message to the standard error stream.
    psh_api psh_attr_fmt(2) void log_fmt(LogInfo const& info, strptr fmt, ...) psh_no_except;
}  // namespace psh::impl::log

// -------------------------------------------------------------------------------------------------
// Logging macro utilities intended for easier usage pattern.
// -------------------------------------------------------------------------------------------------

#define psh_impl_make_log_info(log_level)            \
    psh::impl::log::LogInfo {                        \
        .file_name     = psh_source_file_name(),     \
        .function_name = psh_source_function_name(), \
        .line          = psh_source_line_number(),   \
        .level         = log_level,                  \
    }

#if !defined(PSH_DISABLE_LOGGING)
#    define psh_log_fatal(msg)            psh::impl::log::log_msg(psh_impl_make_log_info(psh::impl::log::LOG_LEVEL_FATAL), msg)
#    define psh_log_error(msg)            psh::impl::log::log_msg(psh_impl_make_log_info(psh::impl::log::LOG_LEVEL_ERROR), msg)
#    define psh_log_warning(msg)          psh::impl::log::log_msg(psh_impl_make_log_info(psh::impl::log::LOG_LEVEL_WARNING), msg)
#    define psh_log_info(msg)             psh::impl::log::log_msg(psh_impl_make_log_info(psh::impl::log::LOG_LEVEL_INFO), msg)
#    define psh_log_fatal_fmt(fmt, ...)   psh::impl::log::log_fmt(psh_impl_make_log_info(psh::impl::log::LOG_LEVEL_FATAL), fmt, __VA_ARGS__)
#    define psh_log_error_fmt(fmt, ...)   psh::impl::log::log_fmt(psh_impl_make_log_info(psh::impl::log::LOG_LEVEL_ERROR), fmt, __VA_ARGS__)
#    define psh_log_warning_fmt(fmt, ...) psh::impl::log::log_fmt(psh_impl_make_log_info(psh::impl::log::LOG_LEVEL_WARNING), fmt, __VA_ARGS__)
#    define psh_log_info_fmt(fmt, ...)    psh::impl::log::log_fmt(psh_impl_make_log_info(psh::impl::log::LOG_LEVEL_INFO), fmt, __VA_ARGS__)
#    if defined(PSH_DEBUG)
#        define psh_log_debug(msg)          psh::impl::log::log_msg(psh_impl_make_log_info(psh::impl::log::LOG_LEVEL_DEBUG), msg)
#        define psh_log_debug_fmt(fmt, ...) psh::impl::log::log_fmt(psh_impl_make_log_info(psh::impl::log::LOG_LEVEL_DEBUG), fmt, __VA_ARGS__)
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
