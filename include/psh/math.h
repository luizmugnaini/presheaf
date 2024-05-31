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
/// Description: Mathematical utilities.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/type_utils.h>
#include <psh/types.h>

#include <limits>

namespace psh {
    [[maybe_unused]] constexpr f32 PI = 3.14159265359f;

    /// Add two values wrapping the result to the corresponding maximal numeric limit.
    template <typename T>
        requires Addable<T> && IsSigned<T> && TriviallyCopyable<T>
    constexpr T wrap_add(T a, T b) noexcept {
        T const c = a + b;
        return (c >= a) ? c : std::numeric_limits<T>::max();
    }

    /// Subtract two values wrapping the result to the corresponding minimal numeric limit.
    template <typename T>
        requires Addable<T> && IsUnsigned<T> && TriviallyCopyable<T>
    constexpr T wrap_sub(T a, T b) noexcept {
        T const c = a - b;
        return (c <= a) ? c : std::numeric_limits<T>::min();
    }
}  // namespace psh
