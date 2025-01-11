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
// - PSH_ENABLE_ASSERTIONS: Enable the use of asserts.
// - PSH_ENABLE_USAGE_VALIDATION: Insert checks to ensure that any given Presheaf function is given
//   valid arguments. In other words, this will assert that the implicit contract between caller
//   and callee is followed.
// - PSH_ENABLE_PARANOID_USAGE_VALIDATION: Inserts even more validation checks (this option isn't
//   enabled via PSH_ENABLE_DEBUG, you have to set it manually).
// - PSH_ENABLE_ASSERT_NOT_NULL: Assert that arguments that shouldn't be null, aren't.
// - PSH_ENABLE_ASSERT_NO_ALIAS: Assert that the no-aliasing rule is followed.
// - PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE: Check at compile time if template based functions have
//   arguments satisfying the procedure assumptions.
// - PSH_ENABLE_ASSERT_BOUNDS_CHECK: For every container-like struct, check if the accessing index stays
//   within the container memory region bounds.
// - PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP: Before calling memcpy, assert that the memory regions being
//   copied don't overlap.
// - PSH_ENABLE_ASSERT_NO_MEMORY_ERROR: When a memory acquisition function fails, abort the program.
// - PSH_ENABLE_LOGGING: Enable logging calls to execute.
// - PSH_ENABLE_DEBUG: Enables all of the above debug checks.
// - PSH_ENABLE_ANSI_COLOURS: When logging, use ANSI colour codes for pretty printing. This may not
//   be desired if you're printing to a log file, hence the option is disabled by default.
// - PSH_ENABLE_FORCED_INLINING: Disable the use of forced inlining hints via psh_inline.
// - PSH_ENABLE_NO_ALIAS: Disable the use of the no aliasing restriction hints in function
//   arguments that use psh_no_alias.
//
// @TODO: update the flag description to account for the new ones.
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
#    if !defined(PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP)
#        define PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP 1
#    endif
#    if !defined(PSH_ENABLE_LOGGING)
#        define PSH_ENABLE_LOGGING 1
#    endif
#    if !defined(PSH_ENABLE_FORCED_INLINING)
#        define PSH_ENABLE_FORCED_INLINING 1
#    endif
#    if !defined(PSH_ENABLE_NO_ALIAS)
#        define PSH_ENABLE_NO_ALIAS 1
#    endif
#endif

//
// Set the default values for all flags:
//

#if !defined(PSH_ENABLE_USAGE_VALIDATION)
#    define PSH_ENABLE_USAGE_VALIDATION 0
#endif
#if !defined(PSH_ENABLE_PARANOID_USAGE_VALIDATION)
#    define PSH_ENABLE_PARANOID_USAGE_VALIDATION 0
#endif

#if !defined(PSH_ENABLE_ASSERTIONS)
#    define PSH_ENABLE_ASSERTIONS 0
#endif
#if !defined(PSH_ENABLE_ASSERT_NOT_NULL)
#    define PSH_ENABLE_ASSERT_NOT_NULL 0
#endif
#if !defined(PSH_ENABLE_ASSERT_BOUNDS_CHECK)
#    define PSH_ENABLE_ASSERT_BOUNDS_CHECK 0
#endif
#if !defined(PSH_ENABLE_ASSERT_NO_ALIAS)
#    define PSH_ENABLE_ASSERT_NO_ALIAS 0
#endif
#if !defined(PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE)
#    define PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE 0
#endif
#if !defined(PSH_ENABLE_ASSERT_NO_MEMORY_ERROR)
#    define PSH_ENABLE_ASSERT_NO_MEMORY_ERROR 0
#endif
#if !defined(PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP)
#    define PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP 0
#endif

#if !defined(PSH_ENABLE_LOGGING)
#    define PSH_ENABLE_LOGGING 0
#endif
#if !defined(PSH_ENABLE_ANSI_COLOURS)
#    define PSH_ENABLE_ANSI_COLOURS 0
#endif

// @TODO: these are the new ones:

#if !defined(PSH_ENABLE_USE_STB_SPRINTF)
#    define PSH_ENABLE_USE_STB_SPRINTF 1
#endif

#if !defined(PSH_ENABLE_BRANCH_HINTS)
#    define PSH_ENABLE_BRANCH_HINTS 1
#endif

#if !defined(PSH_ENABLE_STATIC_PROCEDURES)
#    define PSH_ENABLE_STATIC_PROCEDURES 0
#endif
#if !defined(PSH_ENABLE_NO_EXCEPT_PROCEDURES)
#    define PSH_ENABLE_NO_EXCEPT_PROCEDURES 1
#endif
#if !defined(PSH_ENABLE_FORCED_INLINING)
#    define PSH_ENABLE_FORCED_INLINING 1
#endif
#if !defined(PSH_ENABLE_NO_ALIAS)
#    define PSH_ENABLE_NO_ALIAS 1
#endif

// -------------------------------------------------------------------------------------------------
// Macros for operating system and compiler detection.
// -------------------------------------------------------------------------------------------------

/// All supported operating systems.
#define PSH_OS_WINDOWS_32 0
#define PSH_OS_WINDOWS_64 0
#define PSH_OS_APPLE      0
#define PSH_OS_LINUX      0
#define PSH_OS_UNIX       0
#define PSH_OS_POSIX      0

#if defined(_WIN32)
#    undef PSH_OS_WINDOWS_32
#    define PSH_OS_WINDOWS_32 1
#elif defined(_WIN64)
#    undef PSH_OS_WINDOWS_64
#    define PSH_OS_WINDOWS_64 1
#endif

#if PSH_OS_WINDOWS_32 || PSH_OS_WINDOWS_64
#    undef PSH_OS_WINDOWS
#    define PSH_OS_WINDOWS 1
#endif

#if defined(__APPLE__) || defined(__MACH__)
#    undef PSH_OS_APPLE
#    define PSH_OS_APPLE 1
#endif

#if defined(__linux__)
#    undef PSH_OS_LINUX
#    define PSH_OS_LINUX 1
#endif

#if defined(__unix__)
#    undef PSH_OS_UNIX
#    define PSH_OS_UNIX 1
#endif

#if defined(_POSIX_VERSION)
#    undef PSH_OS_POSIX
#    define PSH_OS_POSIX 1
#endif

// -------------------------------------------------------------------------------------------------
// Windows debloating.
// -------------------------------------------------------------------------------------------------

/// Windows-specific tweaks.
///
/// Disable most of the useless stuff that comes bundled with the Windows header files.
#if PSH_OS_WINDOWS
#    if !defined(WIN32_LEAN_AND_MEAN)
#        define WIN32_LEAN_AND_MEAN
#    endif
#    if !defined(NOMINMAX)
#        define NOMINMAX
#    endif
#    if !defined(NOATOM)
#        define NOATOM
#    endif
#    if !defined(NOGDI)
#        define NOGDI
#    endif
#    if !defined(NOKERNEL)
#        define NOKERNEL
#    endif
#    if !defined(NOUSER)
#        define NOUSER
#    endif
#    if !defined(NONLS)
#        define NONLS
#    endif
#    if !defined(NOMB)
#        define NOMB
#    endif
#    if !defined(NOMEMMGR)
#        define NOMEMMGR
#    endif
#    if !defined(NOMETAFILE)
#        define NOMETAFILE
#    endif
#    if !defined(NOOPENFILE)
#        define NOOPENFILE
#    endif
#    if !defined(NOSERVICE)
#        define NOSERVICE
#    endif
#    if !defined(NOSOUND)
#        define NOSOUND
#    endif
#    if !defined(NOWH)
#        define NOWH
#    endif
#    if !defined(NOCOMM)
#        define NOCOMM
#    endif
#    if !defined(NODEFERWINDOWPOS)
#        define NODEFERWINDOWPOS
#    endif
#    if !defined(NOMCX)
#        define NOMCX
#    endif
#    if !defined(NOIME)
#        define NOIME
#    endif
#endif  // PSH_OS_WINDOWS

// -------------------------------------------------------------------------------------------------
// Compiler detection.
// -------------------------------------------------------------------------------------------------

/// All supported compilers.
#define PSH_COMPILER_MSVC      0
#define PSH_COMPILER_MSVC_YEAR 0
#define PSH_COMPILER_CLANG     0
#define PSH_COMPILER_CLANG_CL  0
#define PSH_COMPILER_GCC       0

#if defined(_MSC_VER)
#    undef PSH_COMPILER_MSVC
#    define PSH_COMPILER_MSVC 1
#    undef PSH_COMPILER_MSVC_YEAR

#    if _MSC_VER >= 1920
#        define PSH_COMPILER_MSVC_YEAR 2019
#    elif _MSC_VER >= 1910
#        define PSH_COMPILER_MSVC_YEAR 2017
#    elif _MSC_VER >= 1900
#        define PSH_COMPILER_MSVC_YEAR 2015
#    else
#        error "Only MSVC 2015 or later is supported."
#    endif

#    if defined(__clang_major__)
#        undef PSH_COMPILER_CLANG_CL
#        define PSH_COMPILER_CLANG_CL 1
#        undef PSH_COMPILER_CLANG
#        define PSH_COMPILER_CLANG 1
#    endif
#elif defined(__clang__)
#    undef PSH_COMPILER_CLANG
#    define PSH_COMPILER_CLANG 1
#elif defined(__GNUC__)
#    undef PSH_COMPILER_GCC
#    define PSH_COMPILER_GCC 1
#else
#    error "Compiler not supported. Please use MSVC, Clang or GCC."
#endif

// -------------------------------------------------------------------------------------------------
// Compiler capabilities.
// -------------------------------------------------------------------------------------------------

/// All compiler capabilities to be detected.
#define PSH_COMPILER_CAPABILITY_HAS_FEATURE 0
#define PSH_COMPILER_CAPABILITY_HAS_INCLUDE 0

#if defined(__has_feature)
#    undef PSH_COMPILER_CAPABILITY_HAS_FEATURE
#    define PSH_COMPILER_CAPABILITY_HAS_FEATURE 1
#endif

#if defined(__has_include)
#    undef PSH_COMPILER_CAPABILITY_HAS_INCLUDE
#    define PSH_COMPILER_CAPABILITY_HAS_INCLUDE 1
#endif

// -------------------------------------------------------------------------------------------------
// Address sanitizer.
// -------------------------------------------------------------------------------------------------

/// All address sanitizer related macros.
#define PSH_ADDRESS_SANITIZER_ENABLED     0
#define PSH_RUNTIME_HAS_ADDRESS_SANITIZER 0
#define psh_attribute_disable_asan

// Function attribute for locally disabling the address sanitizer.
#if PSH_COMPILER_MSVC
#    if defined(__SANITIZE_ADDRESS__) && __SANITIZE_ADDRESS__
#        undef PSH_ADDRESS_SANITIZER_ENABLED
#        define PSH_ADDRESS_SANITIZER_ENABLED 1
#        undef psh_attribute_disable_asan
#        define psh_attribute_disable_asan __declspec(no_sanitize_address)
#    endif
#elif PSH_COMPILER_CLANG
#    if PSH_COMPILER_CAPABILITY_HAS_FEATURE
#        if __has_feature(address_sanitizer)
#            undef PSH_ADDRESS_SANITIZER_ENABLED
#            define PSH_ADDRESS_SANITIZER_ENABLED 1
#            undef psh_attribute_disable_asan
#            define psh_attribute_disable_asan __attribute__((__no_sanitize__("address")))
#        endif
#    endif
#elif PSH_COMPILER_GCC
#    if defined(__SANITIZE_ADDRESS__) && __SANITIZE_ADDRESS__
#        undef PSH_ADDRESS_SANITIZER_ENABLED
#        define PSH_ADDRESS_SANITIZER_ENABLED 1
#        undef psh_attribute_disable_asan
#        define psh_attribute_disable_asan __attribute__((__no_sanitize_address__))
#    endif
#endif

// Check if we can use the address sanitizer runtime interface.
#if PSH_COMPILER_CAPABILITY_HAS_INCLUDE
#    if __has_include(<sanitizer/asan_interface.h>)
#        undef PSH_RUNTIME_HAS_ADDRESS_SANITIZER
#        define PSH_RUNTIME_HAS_ADDRESS_SANITIZER 1
#    endif
#endif

// -------------------------------------------------------------------------------------------------
// Architecture information.
// -------------------------------------------------------------------------------------------------

/// All processor architecture information.
#define PSH_ARCH_X64       0
#define PSH_ARCH_ARM       0
#define PSH_ARCH_SIMD_SSE  0
#define PSH_ARCH_SIMD_SSE2 0
#define PSH_ARCH_SIMD_AVX  0
#define PSH_ARCH_SIMD_AVX2 0
#define PSH_ARCH_SIMD_NEON 0

// Detect processor architecture.
#if defined(__x86_64__) || defined(_M_X64) || defined(__amd64__)
#    undef PSH_ARCH_X64
#    define PSH_ARCH_X64 1
#elif defined(__arm__) || defined(_ARM_) || defined(_ARM_ARCH)
#    undef PSH_ARCH_ARM
#    define PSH_ARCH_ARM 1
#endif

// Detect SIMD availability in x64 processors.
#if PSH_ARCH_X64
#    if PSH_COMPILER_MSVC
#        if defined(_M_AMD64)
#            undef PSH_ARCH_SIMD_SSE
#            define PSH_ARCH_SIMD_SSE 1
#            undef PSH_ARCH_SIMD_SSE2
#            define PSH_ARCH_SIMD_SSE2 1
#        endif
#        if defined(__AVX2__)
#            undef PSH_ARCH_SIMD_AVX
#            define PSH_ARCH_SIMD_AVX 1
#            undef PSH_ARCH_SIMD_AVX2
#            define PSH_ARCH_SIMD_AVX2 1
#        elif defined(__AVX__)
#            undef PSH_ARCH_SIMD_AVX
#            define PSH_ARCH_SIMD_AVX 1
#        endif
#    elif PSH_COMPILER_CLANG || PSH_COMPILER_GCC
#        if defined(__SSE__)
#            undef PSH_ARCH_SIMD_SSE
#            define PSH_ARCH_SIMD_SSE 1
#        endif
#        if defined(__SSE2__)
#            undef PSH_ARCH_SIMD_SSE2
#            define PSH_ARCH_SIMD_SSE2 1
#        endif
#        if defined(__AVX__)
#            undef PSH_ARCH_SIMD_AVX
#            define PSH_ARCH_SIMD_AVX 1
#        endif
#        if defined(__AVX2__)
#            undef PSH_ARCH_SIMD_AVX2
#            define PSH_ARCH_SIMD_AVX2 1
#        endif
#    endif
#endif

// Detect SIMD availability in ARM processors.
#if PSH_ARCH_ARM && defined(__ARM_NEON)
#    undef PSH_ARCH_SIMD_NEON
#    define PSH_ARCH_SIMD_NEON 1
#endif

// -------------------------------------------------------------------------------------------------
// Compiler hints.
// -------------------------------------------------------------------------------------------------

/// Linkage attributes.
#define psh_internal static
#define psh_global   static
#if PSH_ENABLE_STATIC_PROCEDURES
#    define psh_proc static
#else
#    define psh_proc
#endif

/// Hint for disabling exception handling in the generated code for a given procedure.
#if PSH_ENABLE_NO_EXCEPT_PROCEDURES
#    define psh_no_except noexcept
#else
#    define psh_no_except
#endif

/// Hint for forced function inlining.
#if !PSH_ENABLE_FORCED_INLINING
#    if PSH_COMPILER_MSVC
#        define psh_inline __forceinline
#    elif PSH_COMPILER_CLANG || PSH_COMPILER_GCC
#        define psh_inline inline __attribute__((always_inline))
#    endif
#endif
#if !defined(psh_inline)
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
#if PSH_COMPILER_MSVC
#    define psh_unreachable() __assume(false)
#elif PSH_COMPILER_CLANG || PSH_COMPILER_GCC
#    define psh_unreachable() __builtin_unreachable()
#else
#    define psh_unreachable() 0
#endif

/// Hints for pointer aliasing rules.
#if !PSH_ENABLE_NO_ALIAS
#    if PSH_COMPILER_MSVC
#        define psh_no_alias __restrict
#    elif PSH_COMPILER_CLANG || PSH_COMPILER_GCC
#        define psh_no_alias __restrict__
#    endif
#endif
#if !defined(psh_no_alias)
#    define psh_no_alias
#endif

/// Compiler hints for branching patterns.
#if PSH_ENABLE_BRANCH_HINTS
#    if PSH_COMPILER_CLANG || PSH_COMPILER_GCC
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

/// printf-like function attribute.
///
/// Parameters:
///     - fmt_pos: The position of the argument containing the formatting string (the first argument
///                of a function has position 1).
#if PSH_COMPILER_CLANG || PSH_COMPILER_GCC
#    define psh_attribute_fmt(fmt_pos) __attribute__((__format__(__printf__, fmt_pos, fmt_pos + 1)))
#else
#    define psh_attribute_fmt(fmt_pos)
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
    using Status                   = b32;
    constexpr Status STATUS_FAILED = false;
    constexpr Status STATUS_OK     = true;
};  // namespace psh

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

// -------------------------------------------------------------------------------------------------
// Source introspection information.
// -------------------------------------------------------------------------------------------------

/// Query the string representing the signature of the current function.
#if PSH_COMPILER_CLANG || PSH_COMPILER_GCC
#    define psh_source_function_signature() __PRETTY_FUNCTION__
#elif PSH_COMPILER_MSVC
#    define psh_source_function_signature() __FUNCSIG__
#else
#    define psh_source_function_signature() "<unknown signature>"
#endif

/// Query the string representing the unadorned name of the current function.
#if PSH_COMPILER_CLANG || PSH_COMPILER_GCC
#    define psh_source_function_name() __func__
#elif PSH_COMPILER_MSVC
#    define psh_source_function_name() __FUNCTION__
#else
#    define psh_source_function_name() "<unknown name>"
#endif

#if PSH_COMPILER_CLANG || PSH_COMPILER_GCC || PSH_COMPILER_MSVC
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
