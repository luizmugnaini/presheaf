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

#include <psh/core.h>

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
