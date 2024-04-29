/// Vector types.
///
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#pragma once

namespace psh {
    template <typename T>
    struct Vec2 {
        T x = 0;
        T y = 0;

        constexpr Vec2& operator+=(Vec2 const& other) noexcept {
            x += other.x;
            y += other.y;
            return *this;
        }
    };

    template <typename T>
    struct Vec3 {
        T x = 0;
        T y = 0;
        T z = 0;

        constexpr Vec3& operator+=(Vec3 const& other) noexcept {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }
    };
}  // namespace psh
