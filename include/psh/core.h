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

#include <cstdint>

// -----------------------------------------------------------------------------
// - Macros for operating system and compiler detection -
// -----------------------------------------------------------------------------

/// Macros for OS detection.
#if defined(_WIN32)
#    define PSH_OS_WINDOWS_32
#elif defined(_WIN64)
#    define PSH_OS_WINDOWS_64
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
// Stop MSVC from complaining about fopen and such.
#    ifndef _CRT_SECURE_NO_WARNINGS
#        define _CRT_SECURE_NO_WARNINGS
#    endif
// Macros disabling many of the annoying Windows headers macros.
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
// - Fundamental types -
// -----------------------------------------------------------------------------

/// Unsigned integer type.
using u8    = uint8_t;
using u16   = uint16_t;
using u32   = uint32_t;
using u64   = uint64_t;
using usize = u64;

/// Signed integer type.
using i8    = int8_t;
using i16   = int16_t;
using i32   = int32_t;
using i64   = int64_t;
using isize = i64;

/// Memory-address types.
using uptr = u64;
using iptr = i64;

/// Floating-point types.
using f32 = float;
using f64 = double;

/// Immutable zero-terminated string type
///
/// A pointer to a contiguous array of constant character values.
using strptr = char const*;

// -----------------------------------------------------------------------------
// - Compiler hints -
// -----------------------------------------------------------------------------

#if __cplusplus >= 202002L  // Technically Presheaf only supports C++20.
#    define PSH_FALLTHROUGH [[fallthrough]]
#else
#    define PSH_FALLTHROUGH
#endif

/// Macro for aborting a program at runtime.
#if defined(PSH_COMPILER_MSVC)
#    define psh_abort() __debugbreak()
#elif defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define psh_abort() __builtin_trap()
#else
#    include <signal.h>
#    define psh_abort() raise(SIGTRAP)
#endif

#if defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define psh_unreachable() (__builtin_unreachable())
#elif defined(PSH_COMPILER_MSVC)
#    define psh_unreachable() (__assume(false))
#else
#    define psh_unreachable() psh_abort()
#endif

/// Signals internal linkage.
#define psh_internal static

/// Signals that a variable is available in the global scope.
#define psh_global static

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
#define psh_discard(x) (void)(x)

// -----------------------------------------------------------------------------
// - Overriding allocation/deallocation -
// -----------------------------------------------------------------------------

#if defined(psh_malloc) && defined(psh_free)
// Fine, all defined.
#elif !defined(psh_malloc) && !defined(psh_free)
// Fine, none defined.
#else
#    error "You must define all or none of the macros psh_malloc and psh_free."
#endif

// Define the default allocation procedures.
#if !defined(psh_malloc)
#    define psh_malloc malloc
#endif
#if !defined(psh_free)
#    define psh_free free
#endif

// -----------------------------------------------------------------------------
// - Pointer operations -
// -----------------------------------------------------------------------------

/// Add or subtract an offset from a pointer if and only if the pointer is not null.
#define psh_ptr_add(ptr, offset) (((ptr) == nullptr) ? nullptr : ((ptr) + static_cast<uptr>(offset)))
#define psh_ptr_sub(ptr, offset) (((ptr) == nullptr) ? nullptr : ((ptr) - static_cast<iptr>(offset)))

#define psh_ptr_offset_bytes(end, start) (reinterpret_cast<iptr>(reinterpret_cast<u8 const*>(end)) - reinterpret_cast<iptr>(reinterpret_cast<u8 const*>(start)))

// -----------------------------------------------------------------------------
// - Mathematical operations -
// -----------------------------------------------------------------------------

#define psh_in_closed_range(val, min, max) (((min) <= (val)) && ((val) <= (max)))
#define psh_in_open_range(val, min, max)   (((min) < (val)) && ((val) < (max)))

/// Minimum/maximum functions.
#define psh_min(lhs, rhs) (((lhs) < (rhs)) ? (lhs) : (rhs))
#define psh_max(lhs, rhs) (((lhs) > (rhs)) ? (lhs) : (rhs))

/// Check if a value is a power of two.
#define psh_is_pow_of_two(n) (((n) > 0) && !((n) & ((n) - 1)))

/// Clamp a value to an interval.
#define psh_clamp(x, min, max) (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x)))

/// Add values and clamp to a lower bound.
#define psh_lb_add(lhs, rhs, lb) (((lhs) + (rhs)) < (lb) ? (lb) : ((lhs) + (rhs)))

/// Add values and clamp to an upper bound.
#define psh_ub_add(lhs, rhs, ub) (((lhs) + (rhs)) > (ub) ? (ub) : ((lhs) + (rhs)))

// Get the sign of a number.
#define psh_sign(x) ((static_cast<f64>(x) > 0.0) ? 1 : ((static_cast<f64>(x) != 0.0) ? -1 : 0))

// Get the absolute value of a number.
#define psh_abs(x) ((static_cast<f64>(x) > 0.0) ? (x) : -(x))

// -----------------------------------------------------------------------------
// - Common memory sizes -
// -----------------------------------------------------------------------------

#define psh_kibibytes(n) ((n) * (1 << 10))
#define psh_mebibytes(n) ((n) * (1 << 20))
#define psh_gibibytes(n) ((n) * (1 << 30))

// -----------------------------------------------------------------------------
// - Misc. utilities -
// -----------------------------------------------------------------------------

/// Generate a string containing the given expression.
#define psh_stringify(x) #x

#if defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
#    define PSH_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#elif defined(PSH_COMPILER_MSVC)
#    define PSH_FUNCTION_SIGNATURE __FUNCSIG__
#else
#    define PSH_FUNCTION_SIGNATURE "<unknown>"
#endif

// -----------------------------------------------------------------------------
// - Short names -
//
// NOTE: Unfortunately, some of the above macros would cause name collisions
//       with the majority of compilers, and for this reason they don't get
//       assigned a short name. (Examples: min, max, clamp, abs, ...)
// -----------------------------------------------------------------------------

#if defined(PSH_DEFINE_SHORT_NAMES)
#    ifndef FALLTHROUGH
#        define FALLTHROUGH PSH_FALLTHROUGH
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
#    ifndef likely
#        define likely psh_likely
#    endif
#    ifndef unlikely
#        define unlikely psh_unlikely
#    endif
#    ifndef attr_fmt
#        define attr_fmt psh_attr_fmt
#    endif
#    ifndef discard
#        define discard psh_discard
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
#    ifndef in_closed_range
#        define in_closed_range psh_in_closed_range
#    endif
#    ifndef in_open_range
#        define in_open_range psh_in_open_range
#    endif
#    ifndef lb_add
#        define lb_add psh_lb_add
#    endif
#    ifndef ub_add
#        define ub_add psh_ub_add
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
#    ifndef stringify
#        define stringify psh_stringify
#    endif
#endif  // PSH_DEFINE_SHORT_NAMES
