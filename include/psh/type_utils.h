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
/// Description: Utility concepts for template trait-based requirements.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <type_traits>

namespace psh {
    // -----------------------------------------------------------------------------
    // - Concepts -
    // -----------------------------------------------------------------------------

    template <typename T>
    concept NotLValueRef = !std::is_lvalue_reference_v<T>;

    template <typename T>
    concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

    template <typename T>
    concept NotTriviallyCopyable = !std::is_trivially_copyable_v<T>;

    template <typename T>
    concept IsObject = std::is_object_v<T>;

    template <typename T>
    concept IsPointer = std::is_pointer_v<T>;

    template <typename T>
    concept NotPointer = !std::is_pointer_v<T>;

    template <typename T>
    concept Integral = std::is_integral_v<T>;

    template <typename T>
    concept Numeric = std::is_integral_v<T> || std::is_floating_point_v<T>;

    template <typename T>
    concept IsSigned = std::is_signed_v<T>;

    template <typename T>
    concept IsUnsigned = std::is_unsigned_v<T>;

    template <typename T>
    concept Addable = requires(T x) { x + x; };

    template <typename T>
    concept PartiallyOrdered = requires(T x, T y) {
        x <= y;
        x == y;
    };

    template <typename T>
    concept Reflexive = requires(T x, T y) { x == y; };

    // -----------------------------------------------------------------------------
    // - Type trickery -
    // -----------------------------------------------------------------------------

    template <typename T>
    struct RemoveRef {
        using Type = T;
    };
    template <typename T>
    struct RemoveRef<T&> {
        using Type = T;
    };
    template <typename T>
    struct RemoveRef<T&&> {
        using Type = T;
    };

    template <typename T>
        requires NotLValueRef<T>
    T&& cast_forward(typename RemoveRef<T>::Type x) {
        return static_cast<T&&>(x);
    }
}  // namespace psh
