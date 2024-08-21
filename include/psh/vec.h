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
/// Description: Vector types (yes, a matrix is also vector). Just like in linear algebra, they have
///              a fixed size.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/core.h>
#include <psh/math.h>

namespace psh {
    // -----------------------------------------------------------------------------
    // - Floating point vectors -
    // -----------------------------------------------------------------------------

    /// 2-dimensional vector in floating-point space.
    struct Vec2 {
        f32 x = 0.0f;
        f32 y = 0.0f;

        /// Check if the components of the vector are inside the floating point zero range.
        bool is_zero(f32 zero_range = F32_IS_ZERO_RANGE) const noexcept;

        /// Get the normalized vector.
        ///
        /// Note: The vector is assumed to be non-zero, otherwise this will result in UB.
        Vec2 normalized() const noexcept;

        /// Euclidean inner product.
        f32 dot(Vec2 const& other) const noexcept;

        Vec2& operator+=(Vec2 const& other) noexcept;
        Vec2& operator-=(Vec2 const& other) noexcept;
        Vec2& operator*=(Vec2 const& other) noexcept;
        Vec2& operator*=(f32 scalar) noexcept;
    };
    Vec2 operator+(Vec2 lhs, Vec2 rhs) noexcept;
    Vec2 operator-(Vec2 lhs, Vec2 rhs) noexcept;
    Vec2 operator*(Vec2 lhs, Vec2 rhs) noexcept;
    Vec2 operator*(Vec2 v, f32 scalar) noexcept;
    Vec2 operator-(Vec2 v) noexcept;

    /// 3-dimensional vector in floating-point space.
    struct Vec3 {
        f32 x = 0.0f;
        f32 y = 0.0f;
        f32 z = 0.0f;

        /// Check if the components of the vector are inside the floating point zero range.
        bool is_zero(f32 zero_range = F32_IS_ZERO_RANGE) const noexcept;

        /// Get the normalized vector.
        ///
        /// Note: The vector is assumed to be non-zero, otherwise this will result in UB.
        Vec3 normalized() const noexcept;

        /// Euclidean inner product.
        f32 dot(Vec3 const& other) const noexcept;

        /// Cross product.
        Vec3 cross(Vec3 const& other) const noexcept;

        Vec3& operator+=(Vec3 const& other) noexcept;
        Vec3& operator-=(Vec3 const& other) noexcept;
        Vec3& operator*=(Vec3 const& other) noexcept;
        Vec3& operator*=(f32 scalar) noexcept;
    };
    Vec3 operator+(Vec3 lhs, Vec3 rhs) noexcept;
    Vec3 operator-(Vec3 lhs, Vec3 rhs) noexcept;
    Vec3 operator*(Vec3 lhs, Vec3 rhs) noexcept;
    Vec3 operator*(Vec3 v, f32 scalar) noexcept;
    Vec3 operator-(Vec3 v) noexcept;

    /// 4-dimensional vector in floating-point space.
    struct Vec4 {
        f32 x = 0.0f;
        f32 y = 0.0f;
        f32 z = 0.0f;
        f32 w = 0.0f;
    };

    // -----------------------------------------------------------------------------
    // - Integer vectors -
    // -----------------------------------------------------------------------------

    /// 2-dimensional vector in integer space.
    struct IVec2 {
        i32 x = 0;
        i32 y = 0;

        bool is_zero() const noexcept;
        Vec2 normalized() const noexcept;
        i32  dot(IVec2 const& other) const noexcept;

        IVec2& operator+=(IVec2 const& other) noexcept;
        IVec2& operator-=(IVec2 const& other) noexcept;
        IVec2& operator*=(IVec2 const& other) noexcept;
        IVec2& operator*=(i32 scalar) noexcept;
    };
    IVec2 operator+(IVec2 lhs, IVec2 rhs) noexcept;
    IVec2 operator-(IVec2 lhs, IVec2 rhs) noexcept;
    IVec2 operator*(IVec2 lhs, IVec2 rhs) noexcept;
    IVec2 operator*(IVec2 v, i32 scalar) noexcept;
    IVec2 operator-(IVec2 v) noexcept;
    bool  operator==(IVec2 lhs, IVec2 rhs) noexcept;

    /// 3-dimensional vector in integer space.
    struct IVec3 {
        i32 x = 0;
        i32 y = 0;
        i32 z = 0;

        /// Check if the vector is zero-valued components.
        bool is_zero() const noexcept;

        /// Get the normalized vector in floating point space coordinates.
        Vec3 normalized() const noexcept;

        /// Euclidean inner product.
        i32 dot(IVec3 const& other) const noexcept;

        /// Cross product.
        IVec3 cross(IVec3 const& other) const noexcept;

        IVec3& operator+=(IVec3 const& other) noexcept;
        IVec3& operator-=(IVec3 const& other) noexcept;
        IVec3& operator*=(IVec3 const& other) noexcept;
        IVec3& operator*=(i32 scalar) noexcept;
    };
    IVec3 operator+(IVec3 lhs, IVec3 rhs) noexcept;
    IVec3 operator-(IVec3 lhs, IVec3 rhs) noexcept;
    IVec3 operator*(IVec3 lhs, IVec3 rhs) noexcept;
    IVec3 operator*(IVec3 v, i32 scalar) noexcept;
    IVec3 operator-(IVec3 v) noexcept;
    bool  operator==(IVec3 lhs, IVec3 rhs) noexcept;

    // -----------------------------------------------------------------------------
    // - Floating point matrices -
    // -----------------------------------------------------------------------------

    /// Row-major 3-dimensional square matrix in floating-point space.
    struct Mat3 {
        f32 buf[9] = {0.0f};

        /// Get a reference to the matrix component whose row is `r` and column is `c`.
        f32& at(u32 r, u32 c) noexcept;

        /// Create an identity matrix.
        static Mat3 id() noexcept;

        /// Create the change of basis transformation for a given triple of basis vectors.
        static Mat3 change_of_basis(Vec3 v1, Vec3 v2, Vec3 v3) noexcept;

        /// Create the general Tait-Bryan intrinsic rotation matrix.
        ///
        /// Parameters: All angle parameters are assumed to be in radians.
        ///     * rot_x: The angle to rotate about the x axis (aka roll angle).
        ///     * rot_y: The angle to rotate about the y axis (aka pitch angle).
        ///     * rot_z: The angle to rotate about the z axis (aka yaw angle).
        static Mat3 rotation_tb(f32 rot_x, f32 rot_y, f32 rot_z) noexcept;

        // TODO: rotation with Euler angles.
    };

    /// Column-major 3-dimensional square matrix in floating-point space.
    struct ColMat3 {
        f32 buf[9] = {0.0f};

        /// Get a reference to the matrix component whose row is `r` and column is `c`.
        f32& at(u32 r, u32 c) noexcept;

        /// Create an identity matrix.
        static ColMat3 id() noexcept;
    };

    /// Column-major 4-dimensional square matrix in floating-point space.
    struct ColMat4 {
        f32 buf[16] = {0.0f};

        /// Get the matrix component whose row is `r` and column is `c`.
        f32& at(u32 r, u32 c) noexcept;

        /// Create an identity matrix.
        static ColMat4 id() noexcept;

        /// Create a scaling matrix for 3D space.
        static ColMat4 scale(Vec3 scaling) noexcept;

        /// Create the translation matrix for a given displacement in 3D space.
        static ColMat4 translation(Vec3 dx_dy_dz) noexcept;

        /// Create the right-handed world-to-view coordinate transformation with respect to a
        /// viewing directional vector.
        ///
        /// Parameters:
        ///     * eye: The position of the camera's projection center.
        ///     * target: The direction to look at.
        ///     * view_up: Orientational vector with the up direction of the camera coordinate
        ///                system.
        static ColMat4 view_direction_rh(Vec3 eye, Vec3 view_direction, Vec3 view_up) noexcept;

        /// Create the right-handed world-to-view coordinate transformation with respect to a
        /// target.
        ///
        /// Parameters:
        ///     * eye: The position of the camera's projection center.
        ///     * target: The target that the camera should look at.
        ///     * view_up: Orientational vector with the up direction of the camera coordinate
        ///                system.
        static ColMat4 look_at_rh(Vec3 eye, Vec3 target, Vec3 view_up) noexcept;

        /// Create the right-handed perspective projection transformation.
        ///
        /// This transformation assumes the following constraints:
        ///     * The x coordinates go from -1 to 1 (left to right direction).
        ///     * The y coordinates go from -1 to 1 (up to down direction, distinct from OpenGL
        ///       convention).
        ///     * The z coordinates go from 0 to 1 (forward to back direction) distinct from OpenGL
        ///       convention.
        /// This transformation is suited for programs working with Vulkan, which assumes the exact
        /// constraints seen above.
        ///
        /// Parameters:
        ///     * fovy: The vertical field of view in radians.
        ///     * aspect: The aspect ratio (width / height) of the view.
        ///     * near: The distance to the near clip plane.
        ///     * far: The distance to the far clip plane.
        static ColMat4
        perspective_projection_rhzo(f32 fovy, f32 aspect, f32 near, f32 far) noexcept;

        // TODO: perspective_projection_rhno following OpenGL conventions.
    };

    /// Left-multiply a 3D vector by a 3-dimensional square matrix.
    Vec3 mat_mul(Mat3 m, Vec3 v) noexcept;

    /// Multiply a pair of 3-dimensional square matrices.
    Mat3 mat_mul(Mat3 lhs, Mat3 rhs) noexcept;

    /// Multiply a pair of 4-dimensional square column-major matrices.
    ColMat4 mat_mul(ColMat4 lhs, ColMat4 rhs) noexcept;
}  // namespace psh
