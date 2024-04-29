#pragma once

#include <type_traits>

namespace psh {
    template <typename T>
    concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

    template <typename T>
    concept NotTriviallyCopyable = !std::is_trivially_copyable_v<T>;

    template <typename T>
    concept Integral = std::is_integral_v<T>;

    template <typename T>
    concept Numeric = std::is_integral_v<T> || std::is_floating_point_v<T>;

    template <typename T>
    concept IsObject = std::is_object_v<T>;

    template <typename T>
    concept Pointer = std::is_pointer_v<T>;

    template <typename T>
    concept NotPointer = !std::is_pointer_v<T>;

    template <typename T>
    concept Addable = requires(T x) { x + x; };

    template <typename T>
    concept PartiallyOrdered = requires(T x, T y) {
        x <= y;
        x == y;
    };

    template <typename T>
    concept Reflexive = requires(T x, T y) { x == y; };
}  // namespace psh
