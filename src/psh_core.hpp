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
/// Description: Library intrinsics and defaults.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "psh_config.hpp"

// -------------------------------------------------------------------------------------------------
// Fundamental types.
// -------------------------------------------------------------------------------------------------

/// Unsigned integer type.
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

/// Signed integer type.
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

/// Memory-address types.
using uptr  = uintptr_t;
using iptr  = ptrdiff_t;
using usize = size_t;
using isize = ptrdiff_t;

/// Floating-point types.
using f32 = float;
using f64 = double;

/// Boolean types.
using b8  = i8;
using b32 = i32;

/// Immutable zero-terminated string type
///
/// A pointer to a contiguous array of constant character values.
using cstring = char const*;

// Namespaced fundamental Presheaf types.
namespace psh {
    /// Status of an operation.
    ///
    /// This gives a better semantic meaning of the return of a failable function, while still
    /// preserving the use of booleans for simple use.
    using Status                   = b32;
    constexpr Status STATUS_FAILED = false;
    constexpr Status STATUS_OK     = true;
};  // namespace psh

// -------------------------------------------------------------------------------------------------
// Compiler hints.
// -------------------------------------------------------------------------------------------------

/// Linkage attributes.
#define psh_internal static
#define psh_global   static

// @TODO: Fix this to allow the use of psh_proc as static
// #define psh_proc
// #if PSH_ENABLE_STATIC_PROCEDURES
// #    define psh_proc [[maybe_unused]] static
// #else
#define psh_proc
// #endif

/// Hint for disabling exception handling in the generated code for a given procedure.
#if PSH_ENABLE_NO_EXCEPT_PROCEDURES
#    define psh_no_except noexcept
#else
#    define psh_no_except
#endif

/// Hint for forced function inlining.
#if !PSH_ENABLE_FORCED_INLINING
#    if defined(_MSC_VER)
#        define psh_inline __forceinline
#    elif defined(__clang__) || defined(__GNUC__)
#        define psh_inline inline __attribute__((always_inline))
#    endif
#endif
#if !defined(psh_inline)
#    define psh_inline inline
#endif

#define psh_no_return [[noreturn]]

/// Hints that the current switch branch should fallthrough the next.
#if __cplusplus >= 202002L
#    define PSH_FALLTHROUGH [[fallthrough]]
#else
#    define PSH_FALLTHROUGH
#endif

/// Code-path should be unreachable.
#if defined(_MSC_VER)
#    define psh_unreachable() __assume(false)
#elif defined(__clang__) || defined(__GNUC__)
#    define psh_unreachable() __builtin_unreachable()
#else
#    define psh_unreachable() 0
#endif

/// Hints for pointer aliasing rules.
#if !PSH_ENABLE_NO_ALIAS
#    if defined(_MSC_VER)
#        define psh_no_alias __restrict
#    elif defined(__clang__) || defined(__GNUC__)
#        define psh_no_alias __restrict__
#    endif
#endif
#if !defined(psh_no_alias)
#    define psh_no_alias
#endif

/// Compiler hints for branching patterns.
#if PSH_ENABLE_BRANCH_HINTS
#    if defined(__clang__) || defined(__GNUC__)
#        define psh_likely(expr)   __builtin_expect(!!(static_cast<long>(static_cast<bool>(expr))), 1)
#        define psh_unlikely(expr) __builtin_expect(!!(static_cast<long>(static_cast<bool>(expr))), 0)
#    endif
#endif
#if !defined(psh_likely)
#    define psh_likely
#endif
#if !defined(psh_unlikely)
#    define psh_unlikely
#endif

// -------------------------------------------------------------------------------------------------
// Common operations.
// -------------------------------------------------------------------------------------------------

namespace psh {
    /// Add an offset in bytes to a pointer if and only if the pointer is not null.
    template <typename T>
    psh_proc psh_inline T* pointer_add_bytes(T* ptr, usize offset_bytes) psh_no_except {
        return (ptr != nullptr) ? reinterpret_cast<T*>(reinterpret_cast<u8*>(ptr) + offset_bytes) : nullptr;
    }
    template <typename T>
    psh_proc psh_inline T const* pointer_const_add_bytes(T const* ptr, usize offset_bytes) psh_no_except {
        return (ptr != nullptr) ? reinterpret_cast<T const*>(reinterpret_cast<u8 const*>(ptr) + offset_bytes) : nullptr;
    }

    /// Subtract an offset in bytes to a pointer if and only if the pointer is not null.
    template <typename T>
    psh_proc psh_inline T* pointer_subtract_bytes(T* ptr, isize offset_bytes) psh_no_except {
        return (ptr != nullptr) ? reinterpret_cast<T*>(reinterpret_cast<u8*>(ptr) - offset_bytes) : nullptr;
    }
    template <typename T>
    psh_proc psh_inline T const* pointer_const_subtract_bytes(T const* ptr, isize offset_bytes) psh_no_except {
        return (ptr != nullptr) ? reinterpret_cast<T const*>(reinterpret_cast<u8 const*>(ptr) - offset_bytes) : nullptr;
    }

    /// Check if two pointers refer to the same address in memory.
    psh_proc psh_inline bool pointers_have_same_address(void const* lhs, void const* rhs) psh_no_except {
        return (reinterpret_cast<u8 const*>(lhs) == reinterpret_cast<u8 const*>(rhs));
    }

    /// Compute the offset in bytes, between two pointers.
    psh_proc psh_inline isize pointer_offset(void const* start, void const* end) psh_no_except {
        return reinterpret_cast<isize>(end) - reinterpret_cast<isize>(start);
    }

    /// Calculate, at compile-time, the element count of a literal array.
    template <typename T, usize COUNT>
    psh_proc psh_inline constexpr usize count_of(T (&)[COUNT]) psh_no_except {
        return COUNT;
    }
    template <typename T, usize COUNT>
    psh_proc psh_inline constexpr usize count_of(T[COUNT]) psh_no_except {
        return COUNT;
    }

    /// Swap the values of two given variables.
    template <typename T>
    psh_proc psh_inline void swap_values(T& lhs, T& rhs) psh_no_except {
        T tmp = lhs;
        lhs   = rhs;
        rhs   = tmp;
    }
}  // namespace psh

/// Compute, at compile time, the size of a given type, signed or unsigned for convenience.
#define psh_isize_of(T) static_cast<isize>(sizeof(T))
#define psh_usize_of(T) static_cast<usize>(sizeof(T))

/// Evaluate then discard the value of a given expression.
#define psh_discard_value(x) (void)(x)

// -------------------------------------------------------------------------------------------------
// Mathematical operations.
// -------------------------------------------------------------------------------------------------

/// Checks if a value is in the closed interval [min, max].
#define psh_value_in_range(value, min, max) (((min) <= (value)) && ((value) <= (max)))

/// Checks if a value is in the open intervalue (min, max).
#define psh_value_within_range(value, min, max) (((min) < (value)) && ((value) < (max)))

/// Minimum/maximum functions.
#define psh_min_value(lhs, rhs) (((lhs) < (rhs)) ? (lhs) : (rhs))
#define psh_max_value(lhs, rhs) (((lhs) > (rhs)) ? (lhs) : (rhs))

/// Clamp a value to an interval.
#define psh_clamp_value(x, min, max) (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x)))

// Get the sign of a number.
#define psh_value_sign(x) ((static_cast<f64>(x) > 0.0) ? 1 : ((static_cast<f64>(x) != 0.0) ? -1 : 0))

// Get the absolute value of a number.
#define psh_abs_value(x) ((static_cast<f64>(x) > 0.0) ? (x) : -(x))

/// Add values and clamp to a lower bound.
#define psh_lower_bound_add(lhs, rhs, lb) (((lhs) + (rhs)) < (lb) ? (lb) : ((lhs) + (rhs)))

/// Add values and clamp to an upper bound.
#define psh_upper_bound_add(lhs, rhs, ub) (((lhs) + (rhs)) > (ub) ? (ub) : ((lhs) + (rhs)))

/// Decrement an unsigned value without wrapping - the lower bound will always be zero.
#define psh_nowrap_unsigned_dec(x) (((x) > 0u) ? (((x)) - 1u) : 0u)

/// Check if a value is a power of two.
#define psh_is_pow_of_two(n) (((n) > 0) && !((n) & (((n)) - 1)))

// -------------------------------------------------------------------------------------------------
// Stringification and tokenization utilities.
// -------------------------------------------------------------------------------------------------

/// Generate a string containing the given expression.
#define psh_stringify(x) #x

#define psh_impl_token_concat_3(x, y)    x##y
#define psh_impl_token_concat_2(x, y)    psh_impl_token_concat_3(x, y)
#define psh_impl_token_concat_1(x, y)    psh_impl_token_concat_2(x, y)
#define psh_token_concat(prefix, suffix) psh_impl_token_concat_1(prefix, suffix)

// -------------------------------------------------------------------------------------------------
// Common memory sizes.
// -------------------------------------------------------------------------------------------------

#define psh_kibibytes(n) ((n) * (1 << 10))
#define psh_mebibytes(n) ((n) * (1 << 20))
#define psh_gibibytes(n) ((n) * (1 << 30))
