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

namespace psh {
    namespace impl::assertion {
        /// Detect if two types are the same.
        template <typename T, typename U>
        struct IsSameType {
            static constexpr bool value = false;
        };
        template <typename T>
        struct IsSameType<T, T> {
            static constexpr bool value = true;
        };
    }  // namespace impl::assertion

    using AbortFunction = void(void* arg);

    psh_api void set_abort_function(AbortFunction* func, void* abort_context = nullptr) psh_no_except;

    psh_api void abort_program() psh_no_except;
}  // namespace psh

#if PSH_ENABLE_USAGE_VALIDATION
#    define psh_validate_usage(validation_code) \
        do {                                    \
            validation_code;                    \
        } while (0)
#else
#    define psh_validate_usage(validation_code) 0
#endif

#if PSH_ENABLE_PARANOID_USAGE_VALIDATION
#    define psh_paranoid_validate_usage(validation_code) \
        do {                                             \
            validation_code;                             \
        } while (0)
#else
#    define psh_paranoid_validate_usage(validation_code) 0
#endif

/// Assertion macros.
#if PSH_ENABLE_ASSERTIONS
#    define psh_assert(expr)                                                              \
        do {                                                                              \
            if (!static_cast<bool>(expr)) {                                               \
                psh_log_fatal_fmt("Assertion failed: %s, msg: %s", #expr, "no message."); \
                psh::abort_program();                                                     \
            }                                                                             \
        } while (0)
#    define psh_assert_msg(expr, msg)                                             \
        do {                                                                      \
            if (!static_cast<bool>(expr)) {                                       \
                psh_log_fatal_fmt("Assertion failed: %s, msg: %s", #expr, (msg)); \
                psh::abort_program();                                             \
            }                                                                     \
        } while (0)
#    define psh_assert_fmt(expr, fmt, ...)                                                \
        do {                                                                              \
            if (!static_cast<bool>(expr)) {                                               \
                psh_log_fatal_fmt("Assertion failed: %s, msg: " fmt, #expr, __VA_ARGS__); \
                psh::abort_program();                                                     \
            }                                                                             \
        } while (0)
#    define psh_assert_constexpr(expr)                                                    \
        do {                                                                              \
            if constexpr (!static_cast<bool>(expr)) {                                     \
                psh_log_fatal_fmt("Assertion failed: %s, msg: %s", #expr, "no message."); \
                psh::abort_program();                                                     \
            }                                                                             \
        } while (0)
#else
#    define psh_assert(expr)               psh_discard_value(expr)
#    define psh_assert_msg(expr, msg)      psh_discard_value(expr)
#    define psh_assert_fmt(expr, fmt, ...) psh_discard_value(expr)
#    define psh_assert_constexpr(expr)     psh_discard_value(expr)
#endif

#if PSH_ENABLE_ASSERT_NOT_NULL
#    define psh_assert_not_null(ptr) psh_assert_msg((ptr) != nullptr, "Invalid pointer.")
#else
#    define psh_assert_not_null(ptr) psh_discard_value(ptr)
#endif

#if PSH_ENABLE_ASSERT_NO_ALIAS
#    define psh_assert_no_alias(rhs_ptr, lhs_ptr) psh_assert_msg(!pointers_have_same_address(rhs_ptr, lhs_ptr), "Pointers are expected not to alias, but assumption does not hold.")
#else
#    define psh_assert_no_alias(rhs_ptr, lhs_ptr) 0
#endif

#if PSH_ENABLE_ASSERT_BOUNDS_CHECK
#    define psh_assert_bounds_check(idx, element_count) psh_assert_fmt((idx) < (element_count), "Index %zu out of bounds for container with element count %zu.", static_cast<usize>(idx), static_cast<usize>(element_count))
#else
#    define psh_assert_bounds_check(idx, element_count) 0
#endif

/// Ensure that a certain container function argument satisfies all of the properties expected from
/// a proper Presheaf container.
#if PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE
#    define psh_static_assert_valid_mutable_container_type(ContainerType, container)                                         \
        do {                                                                                                                 \
            static_assert(                                                                                                   \
                psh::impl::assertion::IsSameType<decltype(&(container)->buf[0]), typename ContainerType::ValueType*>::value, \
                "A valid mutable container must have a 'buf' member variable of type 'ValueType*' or 'ValueType const*'.");  \
            static_assert(                                                                                                   \
                psh::impl::assertion::IsSameType<decltype((container)->count), usize const>::value                           \
                    || psh::impl::assertion::IsSameType<decltype((container)->count), usize>::value,                         \
                "A valid mutable container must have a 'count' member variable of type 'usize' or 'usize const'.");          \
        } while (0)
#    define psh_static_assert_valid_const_container_type(ContainerType, container)                                                        \
        do {                                                                                                                              \
            static_assert(                                                                                                                \
                psh::impl::assertion::IsSameType<decltype(&(container)->buf[0]), typename ContainerType::ValueType*>::value               \
                    || psh::impl::assertion::IsSameType<decltype(&(container)->buf[0]), typename ContainerType::ValueType const*>::value, \
                "A valid constant container must have a 'buf' member variable of type 'ValueType*' or 'ValueType const*'.");              \
            static_assert(                                                                                                                \
                psh::impl::assertion::IsSameType<decltype((container)->count), usize>::value                                              \
                    || psh::impl::assertion::IsSameType<decltype((container)->count), usize const>::value,                                \
                "A valid constant container must have a 'count' member variable of type 'usize' or 'usize const'.");                      \
        } while (0)
#else
#    define psh_static_assert_valid_mutable_container_type(ContainerType, container) 0
#    define psh_static_assert_valid_const_container_type(ContainerType, container)   0
#endif

#define psh_todo()                                        \
    do {                                                  \
        psh_log_fatal("@TODO: code-path unimplemented."); \
        psh::abort_program();                             \
    } while (0)

#define psh_todo_msg(msg)                                                  \
    do {                                                                   \
        psh_log_fatal_fmt("@TODO: code-path unimplemented, msg: %s", msg); \
        psh::abort_program();                                              \
    } while (0)
#define psh_todo_fmt(fmt, ...)                                                       \
    do {                                                                             \
        psh_log_fatal_fmt("@TODO: code-path unimplemented, msg: " fmt, __VA_ARGS__); \
        psh::abort_program();                                                        \
    } while (0)
