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
/// Description: Code assertion utilities.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/io.h>

namespace psh {
    [[maybe_unused]] constexpr strptr ASSERT_FMT = "Assertion failed: %s, msg: %s";
}

#if defined(PSH_DEBUG) || defined(PSH_ENABLE_ASSERTS)
#    define psh_assert(expr)                                                         \
        do {                                                                         \
            if (!(expr)) {                                                           \
                psh::log_fmt(LogLevel::Fatal, psh::ASSERT_FMT, #expr, "no message"); \
                psh::abort_program();                                                \
            }                                                                        \
        } while (0)
#    define psh_assert_msg(expr, msg)                                         \
        do {                                                                  \
            if (!(expr)) {                                                    \
                psh::log_fmt(LogLevel::Fatal, psh::ASSERT_FMT, #expr, (msg)); \
                psh::abort_program();                                         \
            }                                                                 \
        } while (0)
#else
#    define psh_assert(expr) (void)(expr)
#    define psh_assert_msg(expr, msg) \
        do {                          \
            (void)(expr);             \
            (void)(msg);              \
        } while (0)
#endif

/// Macro used to mark code-paths as unreachable.
#define psh_unreachable()                                                  \
    do {                                                                   \
        psh::log(psh::LogLevel::Fatal, "Codepath should be unreachable!"); \
        psh::abort_program();                                              \
    } while (0)

/// Macro used to mark code-paths as unimplemented.
#define psh_todo()                                                        \
    do {                                                                  \
        psh::log(psh::LogLevel::Fatal, "TODO: code-path unimplemented!"); \
        psh::abort_program();                                             \
    } while (0)
