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
/// Description: Implementation of the IO stream utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/log.h>

#include <assert.h>
#include <psh/core.h>
#include <stdarg.h>
#include <stdio.h>

namespace psh {
    // -----------------------------------------------------------------------------
    // - Internal implementation details -
    // -----------------------------------------------------------------------------

    namespace impl_log {
        constexpr strptr LOG_FMT = "%s [%s:%d] %s\n";

        strptr log_level_str(LogLevel level) {
            constexpr strptr LEVEL_STR[]{
            // clang-format off
#if !defined(PSH_DISABLE_ANSI_COLORS)
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
                // clang-format on
            };

            return LEVEL_STR[static_cast<u32>(level)];
        }
    }  // namespace impl_log

    // -----------------------------------------------------------------------------
    // - Implementation of the logging procedures -
    // -----------------------------------------------------------------------------

    void log(LogInfo info, strptr msg) {
        psh_discard(fprintf(
            stderr,
            impl_log::LOG_FMT,
            impl_log::log_level_str(info.lvl),
            info.file,
            info.line,
            msg));
    }

    void log_fmt(LogInfo const& info, strptr fmt, ...) noexcept {
        constexpr usize MAX_MSG_LEN = 8192;
        char            msg[MAX_MSG_LEN];

        va_list args;
        va_start(args, fmt);
        {
            // Format the message with the given arguments.
            i32 res_len = vsnprintf(msg, MAX_MSG_LEN, fmt, args);
            assert(res_len != -1 && "snptrintf unable to parse the format string and arguments");

            // Stamp the message with a null-terminator.
            usize ures_len = static_cast<usize>(res_len);
            usize msg_len  = ures_len < MAX_MSG_LEN ? ures_len : MAX_MSG_LEN;
            msg[msg_len]   = 0;
        }
        va_end(args);

        (void)fprintf(
            stderr,
            impl_log::LOG_FMT,
            impl_log::log_level_str(info.lvl),
            info.file,
            info.line,
            msg);
    }
}  // namespace psh
