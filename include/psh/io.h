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

#include <psh/types.h>

#include <cassert>
#include <cstdio>

namespace psh {
    /// Log level.
    ///
    /// The levels are set in an increasing level of vebosity, where `LogLevel::Fatal` is the lowest
    /// and `LogLevel::Debug` is the highest.
    enum class LogLevel {
        Fatal   = 0,  ///< Unrecoverable error.
        Error   = 1,  ///< Recoverable error.
        Warning = 2,  ///< Indicates that something non-optimal may have happened.
        Info    = 3,  ///< General message to state any useful information.
        Debug   = 4,  ///< Serves only for debugging purposes in development.
    };

    struct LogInfo {
        StrPtr const   file;  ///< Source file name.
        u32 const      line;  ///< Source current line.
        LogLevel const lvl;   ///< Severity level.

        consteval LogInfo(
            LogLevel _lvl,
            StrPtr   _file = __builtin_FILE(),
            u32      _line = __builtin_LINE())
            : file{_file}, line{_line}, lvl{_lvl} {}
    };

    StrPtr log_level_str(LogLevel level);

    [[noreturn]] void abort_program() noexcept;

    /// Log a message to the standard error stream.
    ///
    /// Should be used as follows:
    /// ```
    /// psh::log(psh::LogLevel::Error, "Got an error!");
    /// ```
    void log(LogInfo&& info, StrPtr msg);

    /// Log a formatted message to the standard error stream.
    ///
    /// Should be used as follows:
    /// ```
    /// psh::log_fmt(psh::LogLevel::Info, "The result is: %d", 5);
    /// ```
    template <typename... Arg>
    void log_fmt(LogInfo const& info, StringLiteral fmt, Arg const&... args) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_ENABLE_LOGGING)
        constexpr usize max_msg_len = 8192;

        // Format the string with the given arguments.
        char      msg[max_msg_len];
        i32 const res_len = std::snprintf(msg, max_msg_len, fmt.str, args...);
        assert(res_len != -1 && "std::snptrintf unable to parse the format string and arguments");

        // Stamp the message with a null-terminator.
        auto const  ures_len = static_cast<usize>(res_len);
        usize const msg_len  = ures_len < max_msg_len ? ures_len : max_msg_len;
        msg[msg_len]         = 0;

        (void)std::fprintf(
            stderr,
            "%s [%s:%d] %s\n",
            log_level_str(info.lvl),
            info.file,
            info.line,
            msg);
#endif
    }
}  // namespace psh
