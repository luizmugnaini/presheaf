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

// -------------------------------------------------------------------------------------------------
// Presheaf library compile-time flags.
//
// All of the below flags are disabled by default.
//
// - PSH_ENABLE_ASSERTIONS: Enable the use of asserts.
// - PSH_ENABLE_USAGE_VALIDATION: Insert checks to ensure that any given Presheaf function is given
//   valid arguments. In other words, this will assert that the implicit contract between caller
//   and callee is followed.
// - PSH_ENABLE_ASSERT_NOT_NULL: Assert that arguments that shouldn't be null, aren't.
// - PSH_ENABLE_ASSERT_NO_ALIAS: Assert that the no-aliasing rule is followed.
// - PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE: Check at compile time if template based functions have
//   arguments satisfying the procedure assumptions.
// - PSH_ENABLE_ASSERT_BOUNDS_CHECK: For every container-like struct, check if the accessing index stays
//   within the container memory region bounds.
// - PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP: Before calling memcpy, assert that the memory regions being
//   copied don't overlap.
// - PSH_ENABLE_CHECKED_POINTER_ARITHMETIC: Consider if a pointer is null before applying an offset.
// - PSH_ENABLE_ASSERT_NO_MEMORY_ERROR: When a memory acquisition function fails, abort the program.
// - PSH_ENABLE_LOGGING: Enable logging calls to execute.
// - PSH_ENABLE_DEBUG: Enables all of the above debug checks.
// - PSH_ENABLE_ANSI_COLOURS: When logging, use ANSI colour codes for pretty printing. This may not
//   be desired if you're printing to a log file, hence the option is disabled by default.
// - PSH_DISABLE_FORCED_INLINING: Disable the use of forced inlining hints via psh_inline.
// - PSH_DISABLE_NO_ALIAS: Disable the use of the no aliasing restriction hints in function
//   arguments that use psh_no_alias.
// -------------------------------------------------------------------------------------------------

// Enable all debug checks when compiled in debug mode. Otherwise, disable all.
#if defined(PSH_ENABLE_DEBUG) && PSH_ENABLE_DEBUG
#    if !defined(PSH_ENABLE_ASSERTIONS)
#        define PSH_ENABLE_ASSERTIONS 1
#    endif
#    if !defined(PSH_ENABLE_USAGE_VALIDATION)
#        define PSH_ENABLE_USAGE_VALIDATION 1
#    endif
#    if !defined(PSH_ENABLE_ASSERT_NOT_NULL)
#        define PSH_ENABLE_ASSERT_NOT_NULL 1
#    endif
#    if !defined(PSH_ENABLE_ASSERT_BOUNDS_CHECK)
#        define PSH_ENABLE_ASSERT_BOUNDS_CHECK 1
#    endif
#    if !defined(PSH_ENABLE_ASSERT_NO_ALIAS)
#        define PSH_ENABLE_ASSERT_NO_ALIAS 1
#    endif
#    if !defined(PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE)
#        define PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE 1
#    endif
#    if !defined(PSH_ENABLE_ASSERT_NO_MEMORY_ERROR)
#        define PSH_ENABLE_ASSERT_NO_MEMORY_ERROR 1
#    endif
#    if !defined(PSH_ENABLE_CHECKED_POINTER_ARITHMETIC)
#        define PSH_ENABLE_CHECKED_POINTER_ARITHMETIC 1
#    endif
#    if !defined(PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP)
#        define PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP 1
#    endif
#    if !defined(PSH_ENABLE_LOGGING)
#        define PSH_ENABLE_LOGGING 1
#    endif
#    if !defined(PSH_DISABLE_FORCED_INLINING)
#        define PSH_DISABLE_FORCED_INLINING 1
#    endif
#    if !defined(PSH_DISABLE_NO_ALIAS)
#        define PSH_DISABLE_NO_ALIAS 1
#    endif
#else
#    if !defined(PSH_ENABLE_ASSERTIONS)
#        define PSH_ENABLE_ASSERTIONS 0
#    endif
#    if !defined(PSH_ENABLE_USAGE_VALIDATION)
#        define PSH_ENABLE_USAGE_VALIDATION 0
#    endif
#    if !defined(PSH_ENABLE_ASSERT_NOT_NULL)
#        define PSH_ENABLE_ASSERT_NOT_NULL 0
#    endif
#    if !defined(PSH_ENABLE_ASSERT_BOUNDS_CHECK)
#        define PSH_ENABLE_ASSERT_BOUNDS_CHECK 0
#    endif
#    if !defined(PSH_ENABLE_ASSERT_NO_ALIAS)
#        define PSH_ENABLE_ASSERT_NO_ALIAS 0
#    endif
#    if !defined(PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE)
#        define PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE 0
#    endif
#    if !defined(PSH_ENABLE_ASSERT_NO_MEMORY_ERROR)
#        define PSH_ENABLE_ASSERT_NO_MEMORY_ERROR 0
#    endif
#    if !defined(PSH_ENABLE_CHECKED_POINTER_ARITHMETIC)
#        define PSH_ENABLE_CHECKED_POINTER_ARITHMETIC 0
#    endif
#    if !defined(PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP)
#        define PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP 0
#    endif
#    if !defined(PSH_ENABLE_LOGGING)
#        define PSH_ENABLE_LOGGING 0
#    endif
#    if !defined(PSH_DISABLE_FORCED_INLINING)
#        define PSH_DISABLE_FORCED_INLINING 0
#    endif
#    if !defined(PSH_DISABLE_NO_ALIAS)
#        define PSH_DISABLE_NO_ALIAS 0
#    endif
#endif

// By default, disable the use of ANSI colours for logging.
#if !defined(PSH_ENABLE_ANSI_COLOURS) || !PSH_ENABLE_ANSI_COLOURS
#    define PSH_ENABLE_ANSI_COLOURS 0
#endif

// -------------------------------------------------------------------------------------------------
// Macros for operating system and compiler detection.
// -------------------------------------------------------------------------------------------------

#if defined(_WIN32)
#    define PSH_OS_WINDOWS_32
#elif defined(_WIN64)
#    define PSH_OS_WINDOWS_64
#endif
#if defined(_WIN32) || defined(_WIN64)
#    define PSH_OS_WINDOWS
#endif
#if defined(__APPLE__) || defined(__MACH__)
#    define PSH_OS_APPLE
#endif
#if defined(__linux__)
#    define PSH_OS_LINUX
#endif
#if defined(__unix__)
#    define PSH_OS_UNIX
#endif
#if defined(_POSIX_VERSION)
#    define PSH_OS_POSIX
#endif

/// Windows-specific tweaks.
///
/// Disable most of the useless stuff that comes bundled with the Windows header files.
#if defined(PSH_OS_WINDOWS_32) || defined(PSH_OS_WINDOWS_64)
#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    ifndef NOATOM
#        define NOATOM
#    endif
#    ifndef NOGDI
#        define NOGDI
#    endif
#    ifndef NOKERNEL
#        define NOKERNEL
#    endif
#    ifndef NOUSER
#        define NOUSER
#    endif
#    ifndef NONLS
#        define NONLS
#    endif
#    ifndef NOMB
#        define NOMB
#    endif
#    ifndef NOMEMMGR
#        define NOMEMMGR
#    endif
#    ifndef NOMETAFILE
#        define NOMETAFILE
#    endif
#    ifndef NOOPENFILE
#        define NOOPENFILE
#    endif
#    ifndef NOSERVICE
#        define NOSERVICE
#    endif
#    ifndef NOSOUND
#        define NOSOUND
#    endif
#    ifndef NOWH
#        define NOWH
#    endif
#    ifndef NOCOMM
#        define NOCOMM
#    endif
#    ifndef NODEFERWINDOWPOS
#        define NODEFERWINDOWPOS
#    endif
#    ifndef NOMCX
#        define NOMCX
#    endif
#    ifndef NOIME
#        define NOIME
#    endif
#endif  // PSH_OS_WINDOWS

/// Macros for compiler detection.
#if defined(_MSC_VER)
#    define PSH_COMPILER_MSVC
#    if _MSC_VER >= 1920
#        define PSH_COMPILER_MSVC_YEAR 2019
#    elif _MSC_VER >= 1910
#        define PSH_COMPILER_MSVC_YEAR 2017
#    elif _MSC_VER >= 1900
#        define PSH_COMPILER_MSVC_YEAR 2015
#    elif _MSC_VER >= 1800
#        define PSH_COMPILER_MSVC_YEAR 2013
#    elif _MSC_VER >= 1700
#        define PSH_COMPILER_MSVC_YEAR 2012
#    elif _MSC_VER >= 1600
#        define PSH_COMPILER_MSVC_YEAR 2010
#    elif _MSC_VER >= 1500
#        define PSH_COMPILER_MSVC_YEAR 2008
#    elif _MSC_VER >= 1400
#        define PSH_COMPILER_MSVC_YEAR 2005
#    else
#        define PSH_COMPILER_MSVC_YEAR 0
#    endif
#    if defined(__clang_major__)
#        define PSH_COMPILER_CLANG_CL
#        define PSH_COMPILER_CLANG
#    endif
#elif defined(__clang__)
#    define PSH_COMPILER_CLANG
#elif defined(__GNUC__)
#    define PSH_COMPILER_GCC
#else
#    error "Compiler not supported. Please use MSVC, Clang or GCC."
#endif

// -------------------------------------------------------------------------------------------------
// DLL support - importing/exporting function declarations
// -------------------------------------------------------------------------------------------------

#if defined(PSH_DLL) && defined(PSH_BUILD_DLL)
#    error "The user of the DLL version of the library should only define PSH_DLL." \
           "The macro PSH_BUILD_DLL has only internal usage and should not be defined by the user."
#endif

#if defined(PSH_DLL)
#    if defined(PSH_OS_WINDOWS)
#        define psh_api __declspec(dllimport)
#    endif
#elif defined(PSH_BUILD_DLL)
#    if defined(PSH_OS_WINDOWS)
#        define psh_api __declspec(dllexport)
#    else
#        define psh_api __attribute__((visibility("default"))
#    endif
#else
#    define psh_api
#endif

// -------------------------------------------------------------------------------------------------
// Architecture information.
// -------------------------------------------------------------------------------------------------

/// Processor architecture.
#if defined(__x86_64__) || defined(_M_X64) || defined(__amd64__)
#    define PSH_ARCH_X64
#elif defined(__arm__) || defined(_ARM_) || defined(_ARM_ARCH)
#    define PSH_ARCH_ARM
#endif

/// SIMD availability in x64 processors.
#if defined(PSH_ARCH_X64)
#    if defined(PSH_COMPILER_MSVC)
#        if defined(_M_AMD64)
#            define PSH_ARCH_SIMD_SSE
#            define PSH_ARCH_SIMD_SSE2
#        endif
#        if defined(__AVX2__)
#            define PSH_ARCH_SIMD_AVX
#            define PSH_ARCH_SIMD_AVX2
#        elif defined(__AVX__)
#            define PSH_ARCH_SIMD_AVX
#        endif
#    elif defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#        if defined(__SSE__)
#            define PSH_ARCH_SIMD_SSE
#        endif
#        if defined(__SSE2__)
#            define PSH_ARCH_SIMD_SSE2
#        endif
#        if defined(__AVX__)
#            define PSH_ARCH_SIMD_AVX
#        endif
#        if defined(__AVX2__)
#            define PSH_ARCH_SIMD_AVX2
#        endif
#    endif
#endif

/// SIMD availability in ARM processors.
#if defined(PSH_ARCH_ARM) && defined(__ARM_NEON)
#    define PSH_ARCH_SIMD_NEON
#endif

// -------------------------------------------------------------------------------------------------
// Compiler hints.
// -------------------------------------------------------------------------------------------------

/// Hint for disabling exception handling in the generated code for a given procedure.
#define psh_no_except noexcept

/// Hint for forced function inlining.
#if !PSH_DISABLE_FORCED_INLINING
#    if defined(PSH_COMPILER_MSVC)
#        define psh_inline __forceinline
#    elif defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#        define psh_inline inline __attribute__((always_inline))
#    else
#        define psh_inline inline
#    endif
#else
#    define psh_inline inline
#endif

/// Hints that the current switch branch should fallthrough the next.
#if __cplusplus >= 202002L
#    define PSH_FALLTHROUGH [[fallthrough]]
#else
#    define PSH_FALLTHROUGH
#endif

#define psh_no_return [[noreturn]]

/// Code-path should be unreachable.
#if defined(PSH_COMPILER_MSVC)
#    define psh_unreachable() __assume(false)
#elif defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define psh_unreachable() __builtin_unreachable()
#else
#    define psh_unreachable() 0
#endif

/// Linkage hits.
#define psh_internal static
#define psh_global   static

/// Hints for pointer aliasing rules.
#if !PSH_DISABLE_NO_ALIAS
#    if defined(PSH_COMPILER_MSVC)
#        define psh_no_alias __restrict
#    elif defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#        define psh_no_alias __restrict__
#    else
#        define psh_no_alias
#    endif
#else
#    define psh_no_alias
#endif

/// Compiler hints for branching patterns.
#if defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define psh_likely(expr)   __builtin_expect(!!(static_cast<long>(static_cast<bool>(expr))), 1)
#    define psh_unlikely(expr) __builtin_expect(!!(static_cast<long>(static_cast<bool>(expr))), 0)
#else
#    define psh_likely(expr)   (expr)
#    define psh_unlikely(expr) (expr)
#endif

/// printf-like function attribute.
///
/// Parameters:
///     - fmt_pos: The position of the argument containing the formatting string (the first argument
///                of a function has position 1).
#if defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define psh_attr_fmt(fmt_pos) __attribute__((__format__(__printf__, fmt_pos, fmt_pos + 1)))
#else
#    define psh_attr_fmt(fmt_pos)
#endif

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
    using Status                   = bool;
    constexpr Status STATUS_FAILED = false;
    constexpr Status STATUS_OK     = true;
};  // namespace psh

// -------------------------------------------------------------------------------------------------
// Common operations.
// -------------------------------------------------------------------------------------------------

namespace psh {
    /// Add an offset in bytes to a pointer if and only if the pointer is not null.
    template <typename T>
    psh_inline T* pointer_add(T* ptr, usize offset_bytes) psh_no_except {
#if PSH_ENABLE_CHECKED_POINTER_ARITHMETIC
        if (ptr == nullptr) {
            return nullptr;
        }
#endif
        return reinterpret_cast<T*>(reinterpret_cast<u8*>(ptr) + offset_bytes);
    }
    template <typename T>
    psh_inline T const* pointer_const_add(T const* ptr, usize offset_bytes) psh_no_except {
#if PSH_ENABLE_CHECKED_POINTER_ARITHMETIC
        if (ptr == nullptr) {
            return nullptr;
        }
#endif
        return reinterpret_cast<T const*>(reinterpret_cast<u8 const*>(ptr) + offset_bytes);
    }

    /// Subtract an offset in bytes to a pointer if and only if the pointer is not null.
    template <typename T>
    psh_inline void* pointer_sub(T* ptr, isize offset_bytes) psh_no_except {
#if PSH_ENABLE_CHECKED_POINTER_ARITHMETIC
        if (ptr == nullptr) {
            return nullptr;
        }
#endif
        return reinterpret_cast<T*>(reinterpret_cast<u8*>(ptr) - offset_bytes);
    }
    template <typename T>
    psh_inline void const* pointer_const_sub(T const* ptr, isize offset_bytes) psh_no_except {
#if PSH_ENABLE_CHECKED_POINTER_ARITHMETIC
        if (ptr == nullptr) {
            return nullptr;
        }
#endif
        return reinterpret_cast<T const*>(reinterpret_cast<u8 const*>(ptr) - offset_bytes);
    }

    /// Check if two pointers refer to the same address in memory.
    psh_inline bool pointers_have_same_address(void const* lhs, void const* rhs) psh_no_except {
        return (reinterpret_cast<u8 const*>(lhs) == reinterpret_cast<u8 const*>(rhs));
    }

    /// Compute the offset in bytes, between two pointers.
    psh_inline isize pointer_offset(void const* start, void const* end) psh_no_except {
        return static_cast<isize>(reinterpret_cast<u8 const*>(end) - reinterpret_cast<u8 const*>(start));
    }

    /// Calculate, at compile-time, the element count of a literal array.
    template <typename T, usize COUNT>
    psh_inline constexpr usize count_of(T (&)[COUNT]) psh_no_except {
        return COUNT;
    }
    template <typename T, usize COUNT>
    psh_inline constexpr usize count_of(T[COUNT]) psh_no_except {
        return COUNT;
    }

    /// Swap the values of two given variables.
    template <typename T>
    psh_inline void swap_values(T& lhs, T& rhs) psh_no_except {
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

// -------------------------------------------------------------------------------------------------
// Source introspection information.
// -------------------------------------------------------------------------------------------------

/// Query the string representing the signature of the current function.
#if defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define psh_source_function_signature() __PRETTY_FUNCTION__
#elif defined(PSH_COMPILER_MSVC)
#    define psh_source_function_signature() __FUNCSIG__
#else
#    define psh_source_function_signature() "<unknown signature>"
#endif

/// Query the string representing the unadorned name of the current function.
#if defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define psh_source_function_name() __func__
#elif defined(PSH_COMPILER_MSVC)
#    define psh_source_function_name() __FUNCTION__
#else
#    define psh_source_function_name() "<unknown name>"
#endif

#if defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC) || defined(PSH_COMPILER_MSVC)
#    define psh_source_file_name()   __builtin_FILE()
#    define psh_source_line_number() __builtin_LINE()
#else
#    define psh_source_file_name()   "<unknown file>"
#    define psh_source_line_number() 0
#endif

// -------------------------------------------------------------------------------------------------
// Utility macros for dealing with any of the Presheaf containers.
// -------------------------------------------------------------------------------------------------

/// Generate code for inlined index and iterator based access, as well as the necessary type
/// information.
#define psh_impl_generate_container_boilerplate(InnerType, this_buf, this_count)      \
    using ValueType = InnerType;                                                      \
    psh_inline InnerType& operator[](usize idx) psh_no_except {                       \
        psh_assert_bounds_check(idx, this_count);                                     \
        return this_buf[idx];                                                         \
    }                                                                                 \
    psh_inline InnerType const& operator[](usize idx) const psh_no_except {           \
        psh_assert_bounds_check(idx, this_count);                                     \
        return this_buf[idx];                                                         \
    }                                                                                 \
    psh_inline InnerType*       begin() psh_no_except { return this_buf; }            \
    psh_inline InnerType*       end() psh_no_except { return this_buf + this_count; } \
    psh_inline InnerType const* begin() const psh_no_except { return this_buf; }      \
    psh_inline InnerType const* end() const psh_no_except { return this_buf + this_count; }
#define psh_impl_generate_constexpr_container_boilerplate(InnerType, this_buf, this_count)      \
    using ValueType = InnerType;                                                                \
    psh_inline constexpr InnerType& operator[](usize idx) psh_no_except {                       \
        psh_assert_bounds_check(idx, this_count);                                               \
        return this_buf[idx];                                                                   \
    }                                                                                           \
    psh_inline constexpr InnerType const& operator[](usize idx) const psh_no_except {           \
        psh_assert_bounds_check(idx, this_count);                                               \
        return this_buf[idx];                                                                   \
    }                                                                                           \
    psh_inline constexpr InnerType*       begin() psh_no_except { return this_buf; }            \
    psh_inline constexpr InnerType*       end() psh_no_except { return this_buf + this_count; } \
    psh_inline constexpr InnerType const* begin() const psh_no_except { return this_buf; }      \
    psh_inline constexpr InnerType const* end() const psh_no_except { return this_buf + this_count; }
