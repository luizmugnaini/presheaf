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
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/io.h>

namespace psh {
    constexpr strptr ASSERT_FMT = "Assertion failed: %s, msg: %s";
}

// Assertion macros.
#if defined(PSH_DEBUG) || defined(PSH_ENABLE_ASSERTS)
#    define psh_assert(expr)                                         \
        do {                                                         \
            if (!static_cast<bool>(expr)) {                          \
                psh_fatal_fmt(psh::ASSERT_FMT, #expr, "no message"); \
                psh_abort();                                         \
            }                                                        \
        } while (0)
#    define psh_assert_msg(expr, msg)                         \
        do {                                                  \
            if (!static_cast<bool>(expr)) {                   \
                psh_fatal_fmt(psh::ASSERT_FMT, #expr, (msg)); \
                psh_abort();                                  \
            }                                                 \
        } while (0)
#else
#    define psh_assert(expr) (void)(expr)
#    define psh_assert_msg(expr, msg) \
        do {                          \
            (void)(expr);             \
            (void)(msg);              \
        } while (0)
#endif  // PSH_DEBUG || PSH_ENABLE_ASSERTS

#define psh_unreachable()                             \
    do {                                              \
        psh_fatal("Codepath should be unreachable!"); \
        psh_abort();                                  \
    } while (0)

#define psh_todo()                                   \
    do {                                             \
        psh_fatal("TODO: code-path unimplemented!"); \
        psh_abort();                                 \
    } while (0)

#define psh_todo_msg(msg)                                             \
    do {                                                              \
        psh_fatal_fmt("TODO: code-path unimplemented, msg: %s", msg); \
        psh_abort();                                                  \
    } while (0)
