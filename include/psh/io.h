///                          Presheaf Library
///    Copyright (C) 2024 Luiz Gustavo Mugnaini Anselmo
///
///    This program is free software; you can redistribute it and/or modify
///    it under the terms of the GNU General Public License as published by
///    the Free Software Foundation; either version 2 of the License, or
///    (at your option) any later version.
///
///    This program is distributed in the hope that it will be useful,
///    but WITHOUT ANY WARRANTY; without even the implied warranty of
///    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///    GNU General Public License for more details.
///
///    You should have received a copy of the GNU General Public License along
///    with this program; if not, write to the Free Software Foundation, Inc.,
///    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
///
/// Description: IO stream utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/intrinsics.h>
#include <psh/types.h>

namespace psh {
    /// Log level.
    ///
    /// The levels are set in an increasing level of vebosity, where `LogLevel::Fatal` is the lowest
    /// and `LogLevel::Debug` is the highest.
    enum struct LogLevel {
        Fatal   = 0,  ///< Unrecoverable error.
        Error   = 1,  ///< Recoverable error.
        Warning = 2,  ///< Indicates that something non-optimal may have happened.
        Info    = 3,  ///< General message to state any useful information.
        Debug   = 4,  ///< Serves only for debugging purposes in development.
    };

    struct LogInfo {
        strptr const   file;  ///< Source file name.
        u32 const      line;  ///< Source current line.
        LogLevel const lvl;   ///< Severity level.

        consteval LogInfo(
            LogLevel _lvl,
            strptr   _file = __builtin_FILE(),
            u32      _line = __builtin_LINE())
            : file{_file}, line{_line}, lvl{_lvl} {}
    };

    [[noreturn]] void abort_program() noexcept;

    /// Log a message to the standard error stream.
    ///
    /// Should be used as follows:
    /// ```
    /// psh::log(psh::LogLevel::Error, "Got an error!");
    /// ```
    void log(LogInfo info, strptr msg);

    /// Log a formatted message to the standard error stream.
    ///
    /// Should be used as follows:
    /// ```
    /// psh::log_fmt(psh::LogLevel::Info, "The result is: %d", 5);
    /// ```
    psh_attr_fmt(2, 3) void log_fmt(LogInfo const& info, strptr fmt, ...) noexcept;
}  // namespace psh

#if defined(PSH_DEBUG) || defined(PSH_ENABLE_LOGGING)
#    define psh_fatal(msg)            psh::log(psh::LogLevel::Fatal, msg)
#    define psh_error(msg)            psh::log(psh::LogLevel::Error, msg)
#    define psh_warning(msg)          psh::log(psh::LogLevel::Warning, msg)
#    define psh_info(msg)             psh::log(psh::LogLevel::Info, msg)
#    define psh_fatal_fmt(fmt, ...)   psh::log_fmt(psh::LogLevel::Fatal, fmt, __VA_ARGS__)
#    define psh_error_fmt(fmt, ...)   psh::log_fmt(psh::LogLevel::Error, fmt, __VA_ARGS__)
#    define psh_warning_fmt(fmt, ...) psh::log_fmt(psh::LogLevel::Warning, fmt, __VA_ARGS__)
#    define psh_info_fmt(fmt, ...)    psh::log_fmt(psh::LogLevel::Info, fmt, __VA_ARGS__)
#else
#    define psh_fatal(msg)            0
#    define psh_error(msg)            0
#    define psh_warning(msg)          0
#    define psh_info(msg)             0
#    define psh_fatal_fmt(fmt, ...)   0
#    define psh_error_fmt(fmt, ...)   0
#    define psh_warning_fmt(fmt, ...) 0
#    define psh_info_fmt(fmt, ...)    0
#endif

#if defined(PSH_DEBUG) && defined(PSH_ENABLE_LOGGING)
#    define psh_debug(msg)          psh::log(psh::LogLevel::Debug, msg)
#    define psh_debug_fmt(fmt, ...) psh::log_fmt(psh::LogLevel::Debug, fmt, __VA_ARGS__)
#else
#    define psh_debug(msg)          0
#    define psh_debug_fmt(fmt, ...) 0
#endif
