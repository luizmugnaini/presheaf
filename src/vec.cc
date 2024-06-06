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
/// Description: Implementation of vector types.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#include <psh/vec.h>

#include <cmath>
#include "psh/assert.h"

namespace psh {
    // -----------------------------------------------------------------------------
    // - 2-dimensional floating-point vector -
    // -----------------------------------------------------------------------------

    // TODO: impl

    // -----------------------------------------------------------------------------
    // - 3-dimensional floating-point vector -
    // -----------------------------------------------------------------------------

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

    bool Vec3::is_zero() const noexcept {
        return (x > F32_IS_ZERO_RANGE) && (y > F32_IS_ZERO_RANGE) && (z > F32_IS_ZERO_RANGE);
    }

    Vec3 Vec3::normalized() const noexcept {
        f32 len = std::sqrt(x * x + y * y + z * z);
        if (psh_unlikely(len < F32_IS_ZERO_RANGE)) {
            return Vec3{};
        }
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

    // -----------------------------------------------------------------------------
    // - 3-dimensional integer vector -
    // -----------------------------------------------------------------------------

    IVec3& IVec3::operator+=(IVec3 const& other) noexcept {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    IVec3& IVec3::operator-=(IVec3 const& other) noexcept {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    IVec3& IVec3::operator*=(IVec3 const& other) noexcept {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    IVec3& IVec3::operator*=(i32 scalar) noexcept {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    IVec3 IVec3::operator+(IVec3 const& other) const noexcept {
        return IVec3{x + other.x, y + other.y, z + other.z};
    }

    IVec3 IVec3::operator-(IVec3 const& other) const noexcept {
        return IVec3{x - other.x, y - other.y, z - other.z};
    }

    IVec3 IVec3::operator-() const noexcept {
        return IVec3{-x, -y, -z};
    }

    IVec3 IVec3::operator*(IVec3 const& other) const noexcept {
        return IVec3{x * other.x, y * other.y, z * other.z};
    }

    IVec3 IVec3::operator*(i32 scalar) const noexcept {
        return IVec3{x * scalar, y * scalar, z * scalar};
    }

    Vec3 IVec3::operator*(f32 scalar) const noexcept {
        return Vec3{
            static_cast<f32>(x) * scalar,
            static_cast<f32>(y) * scalar,
            static_cast<f32>(z) * scalar};
    }

    bool IVec3::is_zero() const noexcept {
        return (x == 0) && (y == 0) && (z == 0);
    }

    Vec3 IVec3::normalized() const noexcept {
        f32 len = std::sqrt(static_cast<f32>(x * x + y * y + z * z));
        return Vec3{
            static_cast<f32>(x) / len,
            static_cast<f32>(y) / len,
            static_cast<f32>(z) / len,
        };
    }

    i32 IVec3::dot(IVec3 const& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }

    IVec3 IVec3::cross(IVec3 const& other) const noexcept {
        return IVec3{
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x,
        };
    }

    // -----------------------------------------------------------------------------
    // - 4-dimensional floating-point vector -
    // -----------------------------------------------------------------------------

    // TODO: impl

    // -----------------------------------------------------------------------------
    // - 4-dimensional square matrix in floating-point space -
    // -----------------------------------------------------------------------------

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

    Mat4 Mat4::translation(Vec3 dx_dy_dz) noexcept {
        return Mat4::translation(dx_dy_dz.x, dx_dy_dz.y, dx_dy_dz.z);
    }
}  // namespace psh
