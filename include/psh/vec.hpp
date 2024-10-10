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
/// Description: Vector types (yes, a matrix is also vector). Just like in linear algebra, they have
///              a fixed size.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/core.hpp>
#include <psh/math.hpp>

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

        bool is_to_the_left_of(Vec2 const& other) const noexcept;

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

    // TODO: implement Mat2 methods.
    struct Mat2 {
        f32 buf[4] = {0.0f};
    };

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
        ///     * near_plane: The distance to the near clip plane.
        ///     * far_plane: The distance to the far clip plane.
        static ColMat4
        perspective_projection_rhzo(f32 fovy, f32 aspect, f32 near_plane, f32 far_plane) noexcept;

        // TODO: perspective_projection_rhno following OpenGL conventions.

        static ColMat4 orthographic_projection_rhzo(
            f32 left,
            f32 right,
            f32 bottom,
            f32 top,
            f32 near_plane,
            f32 far_plane) noexcept;
    };

    /// Left-multiply a 2D vector by a 2D square matrix.
    Vec2 mat_mul(Mat2 m, Vec2 v) noexcept;

    /// Left-multiply a 3D vector by a 3D square matrix.
    Vec3 mat_mul(Mat3 m, Vec3 v) noexcept;

    /// Multiply a pair of 3D square matrices.
    Mat3 mat_mul(Mat3 lhs, Mat3 rhs) noexcept;

    /// Left-multiply a 4D vector by a 4D square column-major matrix.
    Vec4 mat_mul(ColMat4 m, Vec4 v) noexcept;

    /// Multiply a pair of 4D square column-major matrices.
    ColMat4 mat_mul(ColMat4 lhs, ColMat4 rhs) noexcept;
}  // namespace psh
