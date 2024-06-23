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
/// Description: Library intrinsics and defaults.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

/// Macro for aborting a program at runtime.
#if defined(_MSC_VER)
#    define psh_abort() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#    define psh_abort() __builtin_trap()
#else
#    include <signal.h>
#    define psh_abort() raise(SIGTRAP)
#endif

/// Signals internal linkage.
#define psh_internal static
/// Signals that a variable is available in the global scope.
#define psh_global   static

/// Compiler hints for branching patterns.
#define psh_likely(expr)   __builtin_expect(!!(static_cast<long>(static_cast<bool>(expr))), 1)
#define psh_unlikely(expr) __builtin_expect(!!(static_cast<long>(static_cast<bool>(expr))), 0)

/// printf-like function attribute.
///
/// Parameters:
///     * fmt_pos: The position of the argument containing the formatting string (the first argument
///                of a function has position 1).
///     * args_pos: The position of the first argument to be used when formatting the output string.
#define psh_attr_fmt(fmt_pos, args_pos) __attribute__((__format__(__printf__, fmt_pos, args_pos)))

/// Generate a string containing the given expression.
#define psh_stringify(x) #x

/// Discard the value of a given expression.
#define psh_discard(x) (void)(x)

/// Common Memory sizes
#define psh_kibibytes(n) ((n) * (1 << 10))
#define psh_mebibytes(n) ((n) * (1 << 20))
#define psh_gibibytes(n) ((n) * (1 << 30))

/// Minimum/maximum mathematical functions.
#define psh_min(lhs, rhs) (((lhs) < (rhs)) ? (lhs) : (rhs))
#define psh_max(lhs, rhs) (((lhs) > (rhs)) ? (lhs) : (rhs))

/// Check if a value is a power of two.
#define psh_is_pow_of_two(n) (((n) > 0) && !((n) & ((n)-1)))

/// Clamp a value to an interval.
#define psh_clamp(x, min, max) (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x)))

/// Add values and clamp to a lower bound.
#define psh_lb_add(lhs, rhs, lb) (((lhs) + (rhs)) < (lb) ? (lb) : ((lhs) + (rhs)))

/// Add values and clamp to an upper bound.
#define psh_ub_add(lhs, rhs, ub) (((lhs) + (rhs)) > (ub) ? (ub) : ((lhs) + (rhs)))
