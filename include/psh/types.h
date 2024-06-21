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
/// Description: Fundamental types used by the entire library.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
///
/// By default, these types will be exposed to the global namespace. However, if you want these
/// types to be put in the `psh` namespace, compile the library with the `PSH_NAMESPACED_TYPES`
/// flag.

#pragma once

#include <cstdint>

#if defined(PSH_NAMESPACED_TYPES)
namespace psh {
#endif

    /// Unsigned integer type.
    using u8    = std::uint8_t;
    using u16   = std::uint16_t;
    using u32   = std::uint32_t;
    using u64   = std::uint64_t;
    using usize = u64;

    /// Signed integer type.
    using i8    = std::int8_t;
    using i16   = std::int16_t;
    using i32   = std::int32_t;
    using i64   = std::int64_t;
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

#if defined(PSH_NAMESPACED_TYPES)
}
#endif
