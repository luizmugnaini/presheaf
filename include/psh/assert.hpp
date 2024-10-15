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
/// Description: Code assertion utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/core.hpp>
#include <psh/log.hpp>

/// Assertion macros.
#if !defined(PSH_DISABLE_ASSERTS)
#    define psh_assert(expr)                                                             \
        do {                                                                             \
            if (!static_cast<bool>(expr)) {                                              \
                psh_log_fatal_fmt("Assertion failed: %s, msg: %s", #expr, "no message"); \
                psh_abort_program();                                                     \
            }                                                                            \
        } while (0)
#    define psh_assert_msg(expr, msg)                                             \
        do {                                                                      \
            if (!static_cast<bool>(expr)) {                                       \
                psh_log_fatal_fmt("Assertion failed: %s, msg: %s", #expr, (msg)); \
                psh_abort_program();                                              \
            }                                                                     \
        } while (0)
#    define psh_assert_msg_fmt(expr, fmt, ...)                                            \
        do {                                                                              \
            if (!static_cast<bool>(expr)) {                                               \
                psh_log_fatal_fmt("Assertion failed: %s, msg: " fmt, #expr, __VA_ARGS__); \
                psh_abort_program();                                                      \
            }                                                                             \
        } while (0)
#else
#    define psh_assert(expr)                   (void)(expr)
#    define psh_assert_msg(expr, msg)          (void)(expr)
#    define psh_assert_msg_fmt(expr, fmt, ...) (void)(expr)
#endif

#define psh_todo()                                       \
    do {                                                 \
        psh_log_fatal("TODO: code-path unimplemented!"); \
        psh_abort_program();                             \
    } while (0)

#define psh_todo_msg(msg)                                                 \
    do {                                                                  \
        psh_log_fatal_fmt("TODO: code-path unimplemented, msg: %s", msg); \
        psh_abort_program();                                              \
    } while (0)

// -----------------------------------------------------------------------------
// - Short names -
// -----------------------------------------------------------------------------

#if defined(PSH_DEFINE_SHORT_NAMES)
#    ifndef assert_msg
#        define assert_msg psh_assert_msg
#    endif
#    ifndef assert_msg_fmt
#        define assert_msg_fmt psh_assert_msg_fmt
#    endif
#    ifndef todo
#        define todo psh_todo
#    endif
#    ifndef todo_msg
#        define todo_msg psh_todo_msg
#    endif
#endif  // PSH_DEFINE_SHORT_NAMES
