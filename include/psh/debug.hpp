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
/// Description: Utilities for program debugging via assertions and logging.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/core.hpp>

// -------------------------------------------------------------------------------------------------
// Implementation details.
// -------------------------------------------------------------------------------------------------

namespace psh::impl {
    template <typename T, typename U>
    struct IsSameType {
        static constexpr bool value = false;
    };
    template <typename T>
    struct IsSameType<T, T> {
        static constexpr bool value = true;
    };

    enum LogLevel : u32 {
        LOG_LEVEL_FATAL = 0,
        LOG_LEVEL_ERROR,
        LOG_LEVEL_WARNING,
        LOG_LEVEL_INFO,
        LOG_LEVEL_DEBUG,
        LOG_LEVEL_COUNT,
    };

    struct LogInfo {
        cstring  file_name;
        cstring  function_name;
        u32      line;
        LogLevel level;
    };

    /// Log a message to the standard error stream.
    psh_internal void log_msg(LogInfo info, cstring msg) psh_no_except;

    /// Log a formatted message to the standard error stream.
    psh_internal psh_attribute_fmt(2) void log_fmt(LogInfo const& info, cstring fmt, ...) psh_no_except;
}  // namespace psh::impl

// -------------------------------------------------------------------------------------------------
// Procedures for program abortion.
// -------------------------------------------------------------------------------------------------

namespace psh {
    using AbortFunction = void(void* arg);

    psh_proc void set_abort_function(AbortFunction* func, void* abort_context = nullptr) psh_no_except;

    psh_proc void abort_program() psh_no_except;
}  // namespace psh

// -------------------------------------------------------------------------------------------------
// Logging macros.
// -------------------------------------------------------------------------------------------------

#define psh_impl_make_log_info(log_level)            \
    psh::impl::LogInfo {                             \
        .file_name     = psh_source_file_name(),     \
        .function_name = psh_source_function_name(), \
        .line          = psh_source_line_number(),   \
        .level         = log_level,                  \
    }

#if PSH_ENABLE_LOGGING
#    define psh_log_fatal(msg)            psh::impl::log_msg(psh_impl_make_log_info(psh::impl::LOG_LEVEL_FATAL), msg)
#    define psh_log_error(msg)            psh::impl::log_msg(psh_impl_make_log_info(psh::impl::LOG_LEVEL_ERROR), msg)
#    define psh_log_warning(msg)          psh::impl::log_msg(psh_impl_make_log_info(psh::impl::LOG_LEVEL_WARNING), msg)
#    define psh_log_info(msg)             psh::impl::log_msg(psh_impl_make_log_info(psh::impl::LOG_LEVEL_INFO), msg)
#    define psh_log_fatal_fmt(fmt, ...)   psh::impl::log_fmt(psh_impl_make_log_info(psh::impl::LOG_LEVEL_FATAL), fmt, __VA_ARGS__)
#    define psh_log_error_fmt(fmt, ...)   psh::impl::log_fmt(psh_impl_make_log_info(psh::impl::LOG_LEVEL_ERROR), fmt, __VA_ARGS__)
#    define psh_log_warning_fmt(fmt, ...) psh::impl::log_fmt(psh_impl_make_log_info(psh::impl::LOG_LEVEL_WARNING), fmt, __VA_ARGS__)
#    define psh_log_info_fmt(fmt, ...)    psh::impl::log_fmt(psh_impl_make_log_info(psh::impl::LOG_LEVEL_INFO), fmt, __VA_ARGS__)
#    if PSH_ENABLE_DEBUG
#        define psh_log_debug(msg)          psh::impl::log_msg(psh_impl_make_log_info(psh::impl::LOG_LEVEL_DEBUG), msg)
#        define psh_log_debug_fmt(fmt, ...) psh::impl::log_fmt(psh_impl_make_log_info(psh::impl::LOG_LEVEL_DEBUG), fmt, __VA_ARGS__)
#    else
#        define psh_log_debug(msg)          0
#        define psh_log_debug_fmt(fmt, ...) 0
#    endif  // PSH_ENABLE_DEBUG
#else
#    define psh_log_fatal(msg)            0
#    define psh_log_error(msg)            0
#    define psh_log_warning(msg)          0
#    define psh_log_info(msg)             0
#    define psh_log_debug(msg)            0
#    define psh_log_fatal_fmt(fmt, ...)   0
#    define psh_log_error_fmt(fmt, ...)   0
#    define psh_log_warning_fmt(fmt, ...) 0
#    define psh_log_info_fmt(fmt, ...)    0
#    define psh_log_debug_fmt(fmt, ...)   0
#endif

// -------------------------------------------------------------------------------------------------
// Assertion macros.
// -------------------------------------------------------------------------------------------------

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
#    define psh_static_assert_valid_mutable_container_type(ContainerType, container)                                        \
        do {                                                                                                                \
            static_assert(                                                                                                  \
                psh::impl::IsSameType<decltype(&(container)->buf[0]), typename ContainerType::ValueType*>::value,           \
                "A valid mutable container must have a 'buf' member variable of type 'ValueType*' or 'ValueType const*'."); \
            static_assert(                                                                                                  \
                psh::impl::IsSameType<decltype((container)->count), usize const>::value                                     \
                    || psh::impl::IsSameType<decltype((container)->count), usize>::value,                                   \
                "A valid mutable container must have a 'count' member variable of type 'usize' or 'usize const'.");         \
        } while (0)
#    define psh_static_assert_valid_const_container_type(ContainerType, container)                                             \
        do {                                                                                                                   \
            static_assert(                                                                                                     \
                psh::impl::IsSameType<decltype(&(container)->buf[0]), typename ContainerType::ValueType*>::value               \
                    || psh::impl::IsSameType<decltype(&(container)->buf[0]), typename ContainerType::ValueType const*>::value, \
                "A valid constant container must have a 'buf' member variable of type 'ValueType*' or 'ValueType const*'.");   \
            static_assert(                                                                                                     \
                psh::impl::IsSameType<decltype((container)->count), usize>::value                                              \
                    || psh::impl::IsSameType<decltype((container)->count), usize const>::value,                                \
                "A valid constant container must have a 'count' member variable of type 'usize' or 'usize const'.");           \
        } while (0)
#else
#    define psh_static_assert_valid_mutable_container_type(ContainerType, container) 0
#    define psh_static_assert_valid_const_container_type(ContainerType, container)   0
#endif

// -------------------------------------------------------------------------------------------------
// Todo abortion macros.
// -------------------------------------------------------------------------------------------------

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
