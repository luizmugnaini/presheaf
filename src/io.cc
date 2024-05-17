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
/// Description: Implementation of the IO stream utilities.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#include <psh/io.h>

#include <psh/assert.h>
#include <psh/intrinsics.h>
#include <psh/types.h>

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace psh {
    namespace {
        [[maybe_unused]] constexpr strptr LOG_FMT = "%s [%s:%d] %s\n";

        strptr log_level_str(LogLevel level) {
            strptr s;
            switch (level) {
                case LogLevel::Fatal:   s = "\x1b[1;41m[FATAL]\x1b[0m"; break;
                case LogLevel::Error:   s = "\x1b[1;31m[ERROR]\x1b[0m"; break;
                case LogLevel::Warning: s = "\x1b[1;33m[WARNING]\x1b[0m"; break;
                case LogLevel::Info:    s = "\x1b[1;32m[INFO]\x1b[0m"; break;
                case LogLevel::Debug:   s = "\x1b[1;34m[DEBUG]\x1b[0m"; break;
            }
            return s;
        }
    }  // namespace

    void abort_program() noexcept {
        psh_discard(std::fprintf(stderr, "Aborting program...\n"));
        std::abort();
    }

    void log(LogInfo info, strptr msg) {
#if defined(PSH_DEBUG) || defined(PSH_ENABLE_LOGGING)
        psh_discard(
            std::fprintf(stderr, LOG_FMT, log_level_str(info.lvl), info.file, info.line, msg));
#else
        psh_discard(info);
        psh_discard(msg);
#endif
    }

    void log_fmt(LogInfo const& info, strptr fmt, ...) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_ENABLE_LOGGING)
        constexpr usize MAX_MSG_LEN = 8192;
        char            msg[MAX_MSG_LEN];

        va_list args;
        va_start(args, fmt);
        {
            // Format the message with the given arguments.
            i32 const res_len = std::vsnprintf(msg, MAX_MSG_LEN, fmt, args);
            assert(
                res_len != -1 && "std::snptrintf unable to parse the format string and arguments");

            // Stamp the message with a null-terminator.
            auto const  ures_len = static_cast<usize>(res_len);
            usize const msg_len  = ures_len < MAX_MSG_LEN ? ures_len : MAX_MSG_LEN;
            msg[msg_len]         = 0;
        }
        va_end(args);

        (void)std::fprintf(
            stderr,
            "%s [%s:%d] %s\n",
            log_level_str(info.lvl),
            info.file,
            info.line,
            msg);
#else
        psh_discard(info);
        psh_discard(msg);
#endif
    }
}  // namespace psh
