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

#include <psh/core.hh>

namespace psh {
    /// Log level.
    ///
    /// The levels are set in an increasing level of vebosity, where `LogLevel::Fatal` is the lowest
    /// and `LogLevel::Debug` is the highest.
    enum struct LogLevel {
        LEVEL_FATAL   = 0,  ///< Unrecoverable error.
        LEVEL_ERROR   = 1,  ///< Recoverable error.
        LEVEL_WARNING = 2,  ///< Indicates that something non-optimal may have happened.
        LEVEL_INFO    = 3,  ///< General message to state any useful information.
        LEVEL_DEBUG   = 4,  ///< Serves only for debugging purposes in development.
    };

    struct LogInfo {
        strptr   file;  ///< Source file name.
        u32      line;  ///< Source current line.
        LogLevel lvl;   ///< Severity level.

        consteval LogInfo(
            LogLevel _lvl,
            strptr   _file = __builtin_FILE(),
            u32      _line = __builtin_LINE())
            : file{_file}, line{_line}, lvl{_lvl} {}
    };

    /// Log a message to the standard error stream.
    void log(LogInfo info, strptr msg);

    /// Log a formatted message to the standard error stream.
    psh_attr_fmt(2) void log_fmt(LogInfo const& info, strptr fmt, ...) noexcept;
}  // namespace psh

// -----------------------------------------------------------------------------
// - Logging macro utilities intended for easier usage pattern  -
// -----------------------------------------------------------------------------

#if !defined(PSH_DISABLE_LOGGING)
#    define psh_fatal(msg)   psh::log(psh::LogInfo{psh::LogLevel::LEVEL_FATAL}, msg)
#    define psh_error(msg)   psh::log(psh::LogInfo{psh::LogLevel::LEVEL_ERROR}, msg)
#    define psh_warning(msg) psh::log(psh::LogInfo{psh::LogLevel::LEVEL_WARNING}, msg)
#    define psh_info(msg)    psh::log(psh::LogInfo{psh::LogLevel::LEVEL_INFO}, msg)
#    define psh_fatal_fmt(fmt, ...) \
        psh::log_fmt(psh::LogInfo{psh::LogLevel::LEVEL_FATAL}, fmt, __VA_ARGS__)
#    define psh_error_fmt(fmt, ...) \
        psh::log_fmt(psh::LogInfo{psh::LogLevel::LEVEL_ERROR}, fmt, __VA_ARGS__)
#    define psh_warning_fmt(fmt, ...) \
        psh::log_fmt(psh::LogInfo{psh::LogLevel::LEVEL_WARNING}, fmt, __VA_ARGS__)
#    define psh_info_fmt(fmt, ...) \
        psh::log_fmt(psh::LogInfo{psh::LogLevel::LEVEL_INFO}, fmt, __VA_ARGS__)
#    if defined(PSH_DEBUG)
#        define psh_debug(msg) psh::log(psh::LogInfo{psh::LogLevel::LEVEL_DEBUG}, msg)
#        define psh_debug_fmt(fmt, ...) \
            psh::log_fmt(psh::LogInfo{psh::LogLevel::LEVEL_DEBUG}, fmt, __VA_ARGS__)
#    else
#        define psh_debug(msg)          0
#        define psh_debug_fmt(fmt, ...) 0
#    endif  // PSH_DEBUG
#else
#    define psh_fatal(msg)            0
#    define psh_error(msg)            0
#    define psh_warning(msg)          0
#    define psh_info(msg)             0
#    define psh_debug(msg)            0
#    define psh_fatal_fmt(fmt, ...)   0
#    define psh_error_fmt(fmt, ...)   0
#    define psh_warning_fmt(fmt, ...) 0
#    define psh_info_fmt(fmt, ...)    0
#    define psh_debug_fmt(fmt, ...)   0
#endif

// -----------------------------------------------------------------------------
// - Short names -
// -----------------------------------------------------------------------------

#if defined(PSH_DEFINE_SHORT_NAMES)
#    ifndef fatal
#        define fatal psh_fatal
#    endif
#    ifndef error
#        define error psh_error
#    endif
#    ifndef warning
#        define warning psh_warning
#    endif
#    ifndef info
#        define info psh_info
#    endif
#    ifndef debug
#        define debug psh_debug
#    endif
#    ifndef fatal_fmt
#        define fatal_fmt psh_fatal_fmt
#    endif
#    ifndef error_fmt
#        define error_fmt psh_error_fmt
#    endif
#    ifndef warning_fmt
#        define warning_fmt psh_warning_fmt
#    endif
#    ifndef info_fmt
#        define info_fmt psh_info_fmt
#    endif
#    ifndef debug_fmt
#        define debug_fmt psh_debug_fmt
#    endif
#endif
