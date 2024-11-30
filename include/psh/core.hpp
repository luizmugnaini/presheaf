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

// -----------------------------------------------------------------------------
// Presheaf library compile-time flags.
// -----------------------------------------------------------------------------

// Activate default debug checks.
#if defined(PSH_DEBUG)
#    ifndef PSH_CHECK_BOUNDS
#        define PSH_CHECK_BOUNDS
#    endif
#    ifndef PSH_CHECK_MEMCPY_OVERLAP
#        define PSH_CHECK_MEMCPY_OVERLAP
#    endif
#endif

// -----------------------------------------------------------------------------
// Macros for operating system and compiler detection.
// -----------------------------------------------------------------------------

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
#endif  // PSH_OS_WINDOWS_32 || PSH_OS_WINDOWS_64

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
#endif

// -----------------------------------------------------------------------------
// DLL support - importing/exporting function declarations
// -----------------------------------------------------------------------------

#if defined(PSH_DLL) && defined(PSH_BUILD_DLL)
#    error "The user of the DLL version of the library should only define PSH_DLL." \
           "The macro PSH_BUILD_DLL should only be defined in the internal compilation stage of the library as a DLL."
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

// -----------------------------------------------------------------------------
// Architecture information.
// -----------------------------------------------------------------------------

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
#endif  // PSH_ARCH_X64

/// SIMD availability in ARM processors.
#if defined(PSH_ARCH_ARM) && defined(__ARM_NEON)
#    define PSH_ARCH_SIMD_NEON
#endif  // PSH_ARCH_ARM

// -----------------------------------------------------------------------------
// Compiler hints.
// -----------------------------------------------------------------------------

/// Hint for disabling exception handling in the generated code for a given procedure.
#define psh_noexcept noexcept

/// Hint for forced function inlining.
#if defined(PSH_COMPILER_MSVC)
#    define psh_inline __forceinline
#elif defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define psh_inline inline __attribute__((always_inline))
#endif

/// Hints that the current switch branch should fallthrough the next.
#if __cplusplus >= 202002L  // Technically Presheaf only supports C++20.
#    define PSH_FALLTHROUGH [[fallthrough]]
#else
#    define PSH_FALLTHROUGH
#endif

/// Macro for aborting a program at runtime.
#if defined(PSH_COMPILER_MSVC)
#    define psh_abort_program() __debugbreak()
#elif defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define psh_abort_program() __builtin_trap()
#else
#    include <assert.h>
#    define psh_abort_program() assert(false)
#endif

/// Code-path should be unreachable.
#if defined(PSH_COMPILER_MSVC)
#    define psh_unreachable() __assume(false)
#elif defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define psh_unreachable() __builtin_unreachable()
#else
#    define psh_unreachable() psh_abort()
#endif

/// Linkage hits.
#define psh_internal static
#define psh_global   static

/// Hints for pointer aliasing rules.
#if defined(PSH_COMPILER_MSVC)
#    define psh_no_alias __restrict
#elif defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define psh_no_alias __restrict__
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
///     * fmt_pos: The position of the argument containing the formatting string (the first argument
///                of a function has position 1).
#if defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define psh_attr_fmt(fmt_pos) __attribute__((__format__(__printf__, fmt_pos, fmt_pos + 1)))
#else
#    define psh_attr_fmt(fmt_pos)
#endif

/// Discard the value of a given expression.
#define psh_discard_value(x) (void)(x)

// -----------------------------------------------------------------------------
// Fundamental types.
// -----------------------------------------------------------------------------

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

/// Immutable zero-terminated string type
///
/// A pointer to a contiguous array of constant character values.
using strptr = char const*;

namespace psh {
    /// Status of an operation.
    ///
    /// This gives a better semantic meaning of the return of a failable function, while still
    /// preserving the use of booleans for simple use.
    enum Status : bool {
        STATUS_FAILED = false,
        STATUS_OK     = true,
    };
}  // namespace psh

// -----------------------------------------------------------------------------
// Common operations.
// -----------------------------------------------------------------------------

/// Swap the values of two given variables.
#define psh_swap_values(lhs_var, rhs_var)       \
    do {                                        \
        decltype(lhs_var) psh_tmp_ = (lhs_var); \
        lhs_var                    = rhs_var;   \
        rhs_var                    = psh_tmp_;  \
    } while (0)

// -----------------------------------------------------------------------------
// Pointer operations.
// -----------------------------------------------------------------------------

/// Add or subtract an offset from a pointer if and only if the pointer is not null.
#define psh_ptr_add(ptr, offset) (((ptr) == nullptr) ? nullptr : ((ptr) + static_cast<uptr>(offset)))
#define psh_ptr_sub(ptr, offset) (((ptr) == nullptr) ? nullptr : ((ptr) - static_cast<iptr>(offset)))

/// Check if two pointers refer to the same address in memory.
#define psh_ptr_same_addr(lhs_ptr, rhs_ptr) (reinterpret_cast<u8*>(lhs_ptr) == reinterpret_cast<u8*>(rhs_ptr))

/// Compute the offset, in bytes, between two pointers.
#define psh_ptr_offset_bytes(end_ptr, start_ptr)                                           \
    (psh_ptr_same_addr(end_ptr, start_ptr) ? 0                                             \
                                           : reinterpret_cast<iptr>(                       \
                                                 psh_ptr_sub(                              \
                                                     reinterpret_cast<u8 const*>(end_ptr), \
                                                     reinterpret_cast<iptr>(start_ptr))))

// -----------------------------------------------------------------------------
// Mathematical operations.
// -----------------------------------------------------------------------------

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
#define psh_sign_value(x) ((static_cast<f64>(x) > 0.0) ? 1 : ((static_cast<f64>(x) != 0.0) ? -1 : 0))

// Get the absolute value of a number.
#define psh_abs_value(x) ((static_cast<f64>(x) > 0.0) ? (x) : -(x))

/// Add values and clamp to a lower bound.
#define psh_lower_bound_add(lhs, rhs, lb) (((lhs) + (rhs)) < (lb) ? (lb) : ((lhs) + (rhs)))

/// Add values and clamp to an upper bound.
#define psh_upper_bound_add(lhs, rhs, ub) (((lhs) + (rhs)) > (ub) ? (ub) : ((lhs) + (rhs)))

/// Decrement an unsigned value without wrapping - the lower bound will always be zero.
#define psh_nowrap_unsigned_dec(x) (((x) > 0) ? (((x)) - 1) : 0)

/// Check if a value is a power of two.
#define psh_is_pow_of_two(n) (((n) > 0) && !((n) & (((n)) - 1)))

// -----------------------------------------------------------------------------
// Stringification and tokenization utilities.
// -----------------------------------------------------------------------------

/// Generate a string containing the given expression.
#define psh_stringify(x) #x

#define psh_impl_token_concat(x, y)      x##y
#define psh_token_concat(prefix, suffix) psh_impl_token_concat(prefix, suffix)

// -----------------------------------------------------------------------------
// Common memory sizes.
// -----------------------------------------------------------------------------

#define psh_kibibytes(n) ((n) * (1 << 10))
#define psh_mebibytes(n) ((n) * (1 << 20))
#define psh_gibibytes(n) ((n) * (1 << 30))

// -----------------------------------------------------------------------------
// Source introspection information.
// -----------------------------------------------------------------------------

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
#    define psh_source_line_number() (0)
#endif

// -----------------------------------------------------------------------------
// Short names.
// -----------------------------------------------------------------------------

#if defined(PSH_DEFINE_SHORT_NAMES)
// OS detection macros.
#    if defined(PSH_OS_WINDOWS_32) && !defined(OS_WINDOWS_32)
#        define OS_WINDOWS_32 PSH_OS_WINDOWS_32
#    endif
#    if defined(PSH_OS_WINDOWS_64) && !defined(OS_WINDOWS_64)
#        define OS_WINDOWS_64 PSH_OS_WINDOWS_64
#    endif
#    if defined(PSH_OS_WINDOWS) && !defined(OS_WINDOWS)
#        define OS_WINDOWS PSH_OS_WINDOWS
#    endif
#    if defined(PSH_OS_APPLE) && !defined(OS_APPLE)
#        define OS_APPLE PSH_OS_APPLE
#    endif
#    if defined(PSH_OS_LINUX) && !defined(OS_LINUX)
#        define OS_LINUX PSH_OS_LINUX
#    endif
#    if defined(PSH_OS_UNIX) && !defined(OS_UNIX)
#        define OS_UNIX PSH_OS_UNIX
#    endif
#    if defined(PSH_OS_POSIX) && !defined(OS_POSIX)
#        define OS_POSIX PSH_OS_POSIX
#    endif
// Compiler detection macros.
#    if defined(PSH_COMPILER_MSVC) && !defined(COMPILER_MSVC)
#        define COMPILER_MSVC PSH_COMPILER_MSVC
#    endif
#    if defined(PSH_COMPILER_MSVC_YEAR) && !defined(COMPILER_MSVC_YEAR)
#        define COMPILER_MSVC_YEAR PSH_COMPILER_MSVC_YEAR
#    endif
#    if defined(PSH_COMPILER_CLANG_CL) && !defined(COMPILER_CLANG_CL)
#        define COMPILER_CLANG_CL PSH_COMPILER_CLANG_CL
#    endif
#    if defined(PSH_COMPILER_CLANG) && !defined(COMPILER_CLANG)
#        define COMPILER_CLANG PSH_COMPILER_CLANG
#    endif
#    if defined(PSH_COMPILER_GCC) && !defined(COMPILER_GCC)
#        define COMPILER_GCC PSH_COMPILER_GCC
#    endif
// Architecture detection
#    if defined(PSH_ARCH_X64) && !defined(ARCH_X64)
#        define ARCH_X64 PSH_ARCH_X64
#    endif
#    if defined(PSH_ARCH_ARM) && !defined(ARCH_ARM)
#        define ARCH_ARM PSH_ARCH_ARM
#    endif
#    if defined(PSH_ARCH_SIMD_SSE) && !defined(ARCH_SIMD_SSE)
#        define ARCH_SIMD_SSE PSH_ARCH_SIMD_SSE
#    endif
#    if defined(PSH_ARCH_SIMD_SSE2) && !defined(ARCH_SIMD_SSE2)
#        define ARCH_SIMD_SSE2 PSH_ARCH_SIMD_SSE2
#    endif
#    if defined(PSH_ARCH_SIMD_AVX) && !defined(ARCH_SIMD_AVX)
#        define ARCH_SIMD_AVX PSH_ARCH_SIMD_AVX
#    endif
#    if defined(PSH_ARCH_SIMD_AVX2) && !defined(ARCH_SIMD_AVX2)
#        define ARCH_SIMD_AVX2 PSH_ARCH_SIMD_AVX2
#    endif
#    if defined(PSH_ARCH_SIMD_NEON) && !defined(ARCH_SIMD_NEON)
#        define ARCH_SIMD_NEON PSH_ARCH_SIMD_NEON
#    endif
// Miscelaneous macros.
#    ifndef FALLTHROUGH
#        define FALLTHROUGH PSH_FALLTHROUGH
#    endif
#    ifndef abort_program
#        define abort_program psh_abort_program
#    endif
#    ifndef unreachable
#        define unreachable psh_unreachable
#    endif
#    ifndef internal
#        define internal psh_internal
#    endif
#    ifndef global
#        define global psh_global
#    endif
#    ifndef no_alias
#        define no_alias psh_no_alias
#    endif
#    ifndef likely
#        define likely psh_likely
#    endif
#    ifndef unlikely
#        define unlikely psh_unlikely
#    endif
#    ifndef attr_fmt
#        define attr_fmt psh_attr_fmt
#    endif
#    ifndef discard_value
#        define discard_value psh_discard_value
#    endif
#    ifndef stringify
#        define stringify psh_stringify
#    endif
#    ifndef token_concat
#        define token_concat psh_token_concat
#    endif
#    ifndef ptr_add
#        define ptr_add psh_ptr_add
#    endif
#    ifndef ptr_sub
#        define ptr_sub psh_ptr_sub
#    endif
#    ifndef ptr_offset_bytes
#        define ptr_offset_bytes psh_ptr_offset_bytes
#    endif
#    ifndef min_value
#        define min_value psh_min_value
#    endif
#    ifndef max_value
#        define max_value psh_max_value
#    endif
#    ifndef clamp_value
#        define clamp_value psh_clamp_value
#    endif
#    ifndef sign_value
#        define sign_value psh_sign_value
#    endif
#    ifndef abs_value
#        define abs_value psh_abs_value
#    endif
#    ifndef lower_bound_add
#        define lower_bound_add psh_lower_bound_add
#    endif
#    ifndef upper_bound_add
#        define upper_bound_add psh_upper_bound_add
#    endif
#    ifndef nowrap_unsigned_dec
#        define nowrap_unsigned_dec psh_nowrap_unsigned_dec
#    endif
#    ifndef is_pow_of_two
#        define is_pow_of_two psh_is_pow_of_two
#    endif
#    ifndef swap_values
#        define swap_values psh_swap_values
#    endif
#    ifndef kibibytes
#        define kibibytes psh_kibibytes
#    endif
#    ifndef mebibytes
#        define mebibytes psh_mebibytes
#    endif
#    ifndef gibibytes
#        define gibibytes psh_gibibytes
#    endif
#    ifndef source_function_signature
#        define source_function_signature psh_source_function_signature
#    endif
#    ifndef source_function_name
#        define source_function_name psh_source_function_name
#    endif
#    ifndef source_file_name
#        define source_file_name psh_source_file_name
#    endif
#    ifndef source_line_number
#        define source_line_number psh_source_line_number
#    endif
#endif  // PSH_DEFINE_SHORT_NAMES
