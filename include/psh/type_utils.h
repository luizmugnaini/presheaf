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
/// Description: Utility concepts for template trait-based requirements.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

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
