#include <psh/vec.h>
#include <cmath>
#include "psh/assert.h"

namespace psh {
    ///////////////////////////////////////////////////////////////////////////
    // 2-dimensional real vector
    ///////////////////////////////////////////////////////////////////////////

    // TODO: impl

    ///////////////////////////////////////////////////////////////////////////
    // 3-dimensional real vector
    ///////////////////////////////////////////////////////////////////////////

    Vec3& Vec3::operator+=(Vec3 const& other) noexcept {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3& Vec3::operator-=(Vec3 const& other) noexcept {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vec3& Vec3::operator*=(Vec3 const& other) noexcept {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    Vec3& Vec3::operator*=(f32 scalar) noexcept {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vec3 Vec3::operator+(Vec3 const& other) const noexcept {
        return Vec3{x + other.x, y + other.y, z + other.z};
    }

    Vec3 Vec3::operator-(Vec3 const& other) const noexcept {
        return Vec3{x - other.x, y - other.y, z - other.z};
    }

    Vec3 Vec3::operator-() const noexcept {
        return Vec3{-x, -y, -z};
    }

    Vec3 Vec3::operator*(Vec3 const& other) const noexcept {
        return Vec3{x * other.x, y * other.y, z * other.z};
    }

    Vec3 Vec3::operator*(f32 scalar) const noexcept {
        return Vec3{x * scalar, y * scalar, z * scalar};
    }

    Vec3 Vec3::normalized() const noexcept {
        f32 len = std::sqrt(x * x + y * y + z * z);
        psh_assert_msg(len > 0.0f, "Cannot normalize vector whose length is zero.");
        return Vec3{x / len, y / len, z / len};
    }

    f32 Vec3::dot(Vec3 const& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 Vec3::cross(Vec3 const& other) const noexcept {
        return Vec3{
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x,
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    // 4-dimensional real vector
    ///////////////////////////////////////////////////////////////////////////

    // TODO: impl

    ///////////////////////////////////////////////////////////////////////////
    // 4-dimensional square matrix
    ///////////////////////////////////////////////////////////////////////////

    f32& Mat4::at(u32 r, u32 c) noexcept {
        return data[r * 4 + c];
    }

    Mat4 Mat4::id() noexcept {
        // clang-format off
        return Mat4{{
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f,
            }};
        // clang-format on
    }

    Mat4 Mat4::translation(f32 dx, f32 dy, f32 dz) noexcept {
        // clang-format off
        return Mat4{{
                0.0f, 0.0f, 0.0f,   dx,
                0.0f, 0.0f, 0.0f,   dy,
                0.0f, 0.0f, 0.0f,   dz,
                0.0f, 0.0f, 0.0f, 0.0f,
            }};
        // clang-format on
    }
}  // namespace psh
