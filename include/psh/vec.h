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
/// Description: Vector types. Just like in linear algebra, they have a fixed size.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/types.h>
#include <psh/math.h>

namespace psh {
    /// 2-dimensional vector in floating-point space.
    struct Vec2 {
        f32 x = 0.0f;
        f32 y = 0.0f;

        Vec2& operator+=(Vec2 const& other) noexcept;
    };

    /// 3-dimensional vector in floating-point space.
    struct Vec3 {
        f32 x = 0.0f;
        f32 y = 0.0f;
        f32 z = 0.0f;

        Vec3& operator+=(Vec3 const& other) noexcept;
        Vec3& operator-=(Vec3 const& other) noexcept;
        Vec3& operator*=(Vec3 const& other) noexcept;
        Vec3& operator*=(f32 scalar) noexcept;
        Vec3  operator+(Vec3 const& other) const noexcept;
        Vec3  operator-(Vec3 const& other) const noexcept;
        Vec3  operator-() const noexcept;
        Vec3  operator*(Vec3 const& other) const noexcept;
        Vec3  operator*(f32 scalar) const noexcept;

        /// Check if the components of the vector are inside the floating point zero range defined
        /// by `F32_IS_ZERO_RANGE`.
        bool is_zero() const noexcept;

        /// Get the normalized vector.
        ///
        /// Note: The vector is assumed to be non-zero, otherwise this will result in UB.
        Vec3 normalized() const noexcept;

        /// Euclidean inner product.
        f32 dot(Vec3 const& other) const noexcept;

        /// Cross product.
        Vec3 cross(Vec3 const& other) const noexcept;
    };

    /// 3-dimensional vector in integer space.
    struct IVec3 {
        i32 x = 0;
        i32 y = 0;
        i32 z = 0;

        IVec3& operator+=(IVec3 const& other) noexcept;
        IVec3& operator-=(IVec3 const& other) noexcept;
        IVec3& operator*=(IVec3 const& other) noexcept;
        IVec3& operator*=(i32 scalar) noexcept;
        IVec3  operator+(IVec3 const& other) const noexcept;
        IVec3  operator-(IVec3 const& other) const noexcept;
        IVec3  operator-() const noexcept;
        IVec3  operator*(IVec3 const& other) const noexcept;
        IVec3  operator*(i32 scalar) const noexcept;
        Vec3   operator*(f32 scalar) const noexcept;

        /// Check if the vector is zero-valued components.
        bool is_zero() const noexcept;

        /// Get the normalized vector in floating point space coordinates.
        ///
        /// Note: The vector is assumed to be non-zero, otherwise this will result in UB.
        Vec3 normalized() const noexcept;

        /// Euclidean inner product.
        i32 dot(IVec3 const& other) const noexcept;

        /// Cross product.
        IVec3 cross(IVec3 const& other) const noexcept;
    };

    /// 4-dimensional vector in floating-point space.
    struct Vec4 {
        f32 x = 0.0f;
        f32 y = 0.0f;
        f32 z = 0.0f;
        f32 w = 0.0f;
    };

    /// 4-dimensional square matrix in floating-point space.
    ///
    /// Note: The matrix is laid in memory in a column-major form. Although horrible, this is the
    ///       standard way that GLSL reads matrices, so we'll have to compromise on that.
    struct Mat4 {
        f32 buf[16] = {0.0f};

        /// Get the matrix component whose row is `r` and column is `c`.
        f32& at(u32 r, u32 c) noexcept;

        /// Create an identity matrix.
        static Mat4 id() noexcept;

        /// Create the translation matrix for a given displacement in 3D space.
        static Mat4 translation(f32 dx, f32 dy, f32 dz) noexcept;
        static Mat4 translation(Vec3 dx_dy_dz) noexcept;
    };
}  // namespace psh
