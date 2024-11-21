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

#include <math.h>
#include <psh/core.hpp>
#include <psh/math.hpp>

namespace psh {
    // @TODO: I should write all of the code below with SIMD support...

    // -----------------------------------------------------------------------------
    // Floating point vectors.
    // -----------------------------------------------------------------------------

    /// 2-dimensional vector in floating-point space.
    struct psh_api Vec2 {
        f32 x = 0.0f;
        f32 y = 0.0f;

        /// Check if the components of the vector are inside the floating point zero range.
        psh_inline bool is_zero(f32 zero_range) const noexcept {
            return approx_equal(x, 0.0f, zero_range) && approx_equal(y, 0.0f, zero_range);
        }

        /// Get the normalized vector.
        ///
        /// Note: The vector is assumed to be non-zero, otherwise this will result in UB.
        psh_inline Vec2 normalized() const noexcept {
            f32 len = sqrtf(x * x + y * y);

            if (psh_unlikely(len < F32_IS_ZERO_RANGE)) {
                return Vec2{};
            }

            return Vec2{x / len, y / len};
        }

        /// Euclidean inner product.
        psh_inline f32 dot(Vec2 other) const noexcept {
            return x * other.x + y * other.y;
        }

        psh_inline bool is_to_the_left_of(Vec2 other) const noexcept {
            return ((other.x * y - other.y * x) >= 0.0f);
        }

        psh_inline Vec2& operator+=(Vec2 other) noexcept {
            x += other.x;
            y += other.y;
            return *this;
        }
        psh_inline Vec2& operator-=(Vec2 other) noexcept {
            x -= other.x;
            y -= other.y;
            return *this;
        }
        psh_inline Vec2& operator*=(Vec2 other) noexcept {
            x *= other.x;
            y *= other.y;
            return *this;
        }
        psh_inline Vec2& operator*=(f32 scalar) noexcept {
            x *= scalar;
            y *= scalar;
            return *this;
        }
        psh_inline Vec2 operator+(Vec2 other) const noexcept { return Vec2{x + other.x, y + other.y}; }
        psh_inline Vec2 operator-(Vec2 other) const noexcept { return Vec2{x - other.x, y - other.y}; }
        psh_inline Vec2 operator*(Vec2 other) const noexcept { return Vec2{x * other.x, y * other.y}; }
        psh_inline Vec2 operator*(f32 scalar) const noexcept { return Vec2{x * scalar, y * scalar}; }
        psh_inline Vec2 operator-() const noexcept { return Vec2{-x, -y}; }
    };

    /// 3-dimensional vector in floating-point space.
    struct psh_api Vec3 {
        f32 x = 0.0f;
        f32 y = 0.0f;
        f32 z = 0.0f;

        /// Check if the components of the vector are inside the floating point zero range.
        psh_inline bool is_zero(f32 zero_range) const noexcept {
            return approx_equal(x, 0.0f, zero_range) &&
                   approx_equal(y, 0.0f, zero_range) &&
                   approx_equal(z, 0.0f, zero_range);
        }

        /// Get the normalized vector.
        psh_inline Vec3 normalized() const noexcept {
            f32 len = sqrtf(x * x + y * y + z * z);

            if (psh_unlikely(approx_equal(len, 0.0f))) {
                return Vec3{};
            }

            return Vec3{x / len, y / len, z / len};
        }

        /// Euclidean inner product.
        psh_inline f32 dot(Vec3 other) const noexcept {
            return x * other.x + y * other.y + z * other.z;
        }

        /// Cross product.
        psh_inline Vec3 cross(Vec3 other) const noexcept {
            return Vec3{
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x,
            };
        }

        psh_inline Vec3& operator+=(Vec3 other) noexcept {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }
        psh_inline Vec3& operator-=(Vec3 other) noexcept {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }
        psh_inline Vec3& operator*=(Vec3 other) noexcept {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }
        psh_inline Vec3& operator*=(f32 scalar) noexcept {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }
        psh_inline Vec3 operator+(Vec3 other) const noexcept { return Vec3{x + other.x, y + other.y, z + other.z}; }
        psh_inline Vec3 operator-(Vec3 other) const noexcept { return Vec3{x - other.x, y - other.y, z - other.z}; }
        psh_inline Vec3 operator*(Vec3 other) const noexcept { return Vec3{x * other.x, y * other.y, z * other.z}; }
        psh_inline Vec3 operator*(f32 scalar) const noexcept { return Vec3{x * scalar, y * scalar, z * scalar}; }
        psh_inline Vec3 operator-() const noexcept { return Vec3{-x, -y, -z}; }
    };

    /// 4-dimensional vector in floating-point space.
    struct psh_api Vec4 {
        f32 x = 0.0f;
        f32 y = 0.0f;
        f32 z = 0.0f;
        f32 w = 0.0f;

        // @TODO Write associated methods.
    };

    // -----------------------------------------------------------------------------
    // Integer vectors.
    // -----------------------------------------------------------------------------

    /// 2-dimensional vector in integer space.
    struct psh_api IVec2 {
        i32 x = 0;
        i32 y = 0;

        /// Check if all components of the vector are zero.
        psh_inline bool is_zero() const noexcept {
            return (x == 0) && (y == 0);
        }

        /// Get the normalized vector in floating point coordinates.
        psh_inline Vec2 normalized() const noexcept {
            f32 len = sqrtf(static_cast<f32>(x * x + y * y));

            if (psh_unlikely(len < F32_IS_ZERO_RANGE)) {
                return Vec2{};
            }

            return Vec2{static_cast<f32>(x) / len, static_cast<f32>(y) / len};
        }

        /// Euclidean dot product.
        psh_inline i32 dot(IVec2 other) const noexcept {
            return x * other.x + y * other.y;
        }

        psh_inline IVec2& operator+=(IVec2 other) noexcept {
            x += other.x;
            y += other.y;
            return *this;
        }
        psh_inline IVec2& operator-=(IVec2 other) noexcept {
            x -= other.x;
            y -= other.y;
            return *this;
        }
        psh_inline IVec2& operator*=(IVec2 other) noexcept {
            x *= other.x;
            y *= other.y;
            return *this;
        }
        psh_inline IVec2& operator*=(i32 scalar) noexcept {
            x *= scalar;
            y *= scalar;
            return *this;
        }
        psh_inline IVec2 operator+(IVec2 other) const noexcept { return IVec2{x + other.x, y + other.y}; }
        psh_inline IVec2 operator-(IVec2 other) const noexcept { return IVec2{x - other.x, y - other.y}; }
        psh_inline IVec2 operator*(IVec2 other) const noexcept { return IVec2{x * other.x, y * other.y}; }
        psh_inline IVec2 operator*(i32 scalar) const noexcept { return IVec2{x * scalar, y * scalar}; }
        psh_inline IVec2 operator-() const noexcept { return IVec2{-x, -y}; }
        psh_inline bool  operator==(IVec2 other) const noexcept { return (x == other.x) && (y == other.y); }
    };

    /// 3-dimensional vector in integer space.
    struct psh_api IVec3 {
        i32 x = 0;
        i32 y = 0;
        i32 z = 0;

        /// Check if all components of the vector are zero.
        psh_inline bool is_zero() const noexcept {
            return (x == 0) && (y == 0) && (z == 0);
        }

        /// Get the normalized vector in floating point coordinates.
        psh_inline Vec3 normalized() const noexcept {
            f32 len = sqrtf(static_cast<f32>(x * x + y * y + z * z));

            if (psh_unlikely(len < F32_IS_ZERO_RANGE)) {
                return Vec3{};
            }

            return Vec3{
                static_cast<f32>(x) / len,
                static_cast<f32>(y) / len,
                static_cast<f32>(z) / len,
            };
        }

        /// Euclidean inner product.
        psh_inline i32 dot(IVec3 const& other) const noexcept {
            return x * other.x + y * other.y + z * other.z;
        }

        /// Cross product.
        psh_inline IVec3 cross(IVec3 const& other) const noexcept {
            return IVec3{
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x,
            };
        }

        psh_inline IVec3& operator+=(IVec3 const& other) noexcept {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }
        psh_inline IVec3& operator-=(IVec3 const& other) noexcept {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }
        psh_inline IVec3& operator*=(IVec3 const& other) noexcept {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }
        psh_inline IVec3& operator*=(i32 scalar) noexcept {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }
        psh_inline IVec3 operator+(IVec3 other) const noexcept { return IVec3{x + other.x, y + other.y, z + other.z}; }
        psh_inline IVec3 operator-(IVec3 other) const noexcept { return IVec3{x - other.x, y - other.y, z - other.z}; }
        psh_inline IVec3 operator*(IVec3 other) const noexcept { return IVec3{x * other.x, y * other.y, z * other.z}; }
        psh_inline IVec3 operator*(i32 scalar) const noexcept { return IVec3{x * scalar, y * scalar, z * scalar}; }
        psh_inline IVec3 operator-() const noexcept { return IVec3{-x, -y, -z}; }
        psh_inline bool  operator==(IVec3 other) const noexcept { return (x == other.x) && (y == other.y) && (z == other.z); }
    };

    // -----------------------------------------------------------------------------
    // Floating point matrices.
    // -----------------------------------------------------------------------------

    // @TODO: implement Mat2 methods.
    struct psh_api Mat2 {
        f32 buf[4] = {0.0f};
    };

    /// Row-major 3-dimensional square matrix in floating-point space.
    struct psh_api Mat3 {
        f32 buf[9] = {0.0f};

        /// Get a reference to the matrix component whose row is `r` and column is `c`.
        psh_inline f32& at(u32 r, u32 c) noexcept {
            psh_assert_msg(r <= 3, "Row outside range.");
            psh_assert_msg(c <= 3, "Column outside range.");

            return buf[r + c * 3];
        }

        /// Create an identity matrix.
        static psh_inline Mat3 id() noexcept {
            // clang-format off
            return Mat3{
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f,
            };
            // clang-format on
        }

        /// Create the change of basis transformation for a given triple of basis vectors.
        static psh_inline Mat3 change_of_basis(Vec3 v1, Vec3 v2, Vec3 v3) noexcept {
            // @TODO: should we check if v1, v2, v3 form an orthogonal triple?
            // clang-format off
            return Mat3{
                v1.x, v2.x, v3.x,
                v1.y, v2.y, v3.y,
                v1.z, v2.z, v3.z,
            };
            // clang-format on
        }

        /// Create the general Tait-Bryan intrinsic rotation matrix.
        ///
        /// Parameters: All angle parameters are assumed to be in radians.
        ///     * rot_x: The angle to rotate about the x axis (aka roll angle).
        ///     * rot_y: The angle to rotate about the y axis (aka pitch angle).
        ///     * rot_z: The angle to rotate about the z axis (aka yaw angle).
        static Mat3 rotation_tb(f32 rot_x, f32 rot_y, f32 rot_z) noexcept;

        // @TODO: rotation with Euler angles.
    };

    /// Column-major 3-dimensional square matrix in floating-point space.
    struct psh_api ColMat3 {
        f32 buf[9] = {0.0f};

        /// Get a reference to the matrix component whose row is `r` and column is `c`.
        psh_inline f32& at(u32 r, u32 c) noexcept {
            psh_assert_msg(r <= 3, "Row outside range.");
            psh_assert_msg(c <= 3, "Column outside range.");

            return buf[r * 3 + c];
        }

        /// Create an identity matrix.
        static psh_inline ColMat3 id() noexcept {
            // clang-format off
            return ColMat3{
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f,
            };
            // clang-format on
        }
    };

    /// Column-major 4-dimensional square matrix in floating-point space.
    struct psh_api ColMat4 {
        f32 buf[16] = {0.0f};

        /// Get the matrix component whose row is `r` and column is `c`.
        f32& at(u32 r, u32 c) noexcept {
            psh_assert_msg(r <= 4, "Row outside range.");
            psh_assert_msg(c <= 4, "Column outside range.");

            return buf[r * 4 + c];
        }

        /// Create an identity matrix.
        static psh_inline ColMat4 id() noexcept {
            // clang-format off
            return ColMat4{
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f,
            };
            // clang-format on
        }

        /// Create a scaling matrix for 3D space.
        static psh_inline ColMat4 scale(Vec3 scaling) noexcept {
            // clang-format off
            return ColMat4{
                scaling.x,    0.0f,      0.0f,   0.0f,
                   0.0f,   scaling.y,    0.0f,   0.0f,
                   0.0f,      0.0f,   scaling.z, 0.0f,
                   0.0f,      0.0f,      0.0f,   1.0f,
            };
            // clang-format on
        }

        /// Create the translation matrix for a given displacement in 3D space.
        static psh_inline ColMat4 translation(Vec3 dx_dy_dz) noexcept {
            // clang-format off
            return ColMat4{
                   1.0f,       0.0f,       0.0f,    0.0f,
                   0.0f,       1.0f,       0.0f,    0.0f,
                   0.0f,       0.0f,       1.0f,    0.0f,
                dx_dy_dz.x, dx_dy_dz.y, dx_dy_dz.z, 1.0f,
            };
            // clang-format on
        }

        /// Create the right-handed world-to-view coordinate transformation with respect to a
        /// viewing directional vector.
        ///
        /// Parameters:
        ///     * eye: The position of the camera's projection center.
        ///     * target: The direction to look at.
        ///     * view_up: Orientational vector with the up direction of the camera coordinate
        ///                system.
        static psh_inline ColMat4 view_direction_rh(Vec3 eye, Vec3 view_direction, Vec3 view_up) noexcept {
            Vec3 forward = view_direction.normalized();
            Vec3 right   = forward.cross(view_up).normalized();
            Vec3 up      = right.cross(forward);

            // clang-format off
            return ColMat4{
                     right.x,         up.x,       -forward.x,     0.0f,
                     right.y,         up.y,       -forward.y,     0.0f,
                     right.z,         up.z,       -forward.z,     0.0f,
                 -right.dot(eye), -up.dot(eye), forward.dot(eye), 1.0f,
            };
            // clang-format on
        }

        /// Create the right-handed world-to-view coordinate transformation with respect to a
        /// target.
        ///
        /// Parameters:
        ///     * eye: The position of the camera's projection center.
        ///     * target: The target that the camera should look at.
        ///     * view_up: Orientational vector with the up direction of the camera coordinate
        ///                system.
        static psh_inline ColMat4 look_at_rh(Vec3 eye, Vec3 target, Vec3 view_up) noexcept {
            return ColMat4::view_direction_rh(eye, target - eye, view_up);
        }

        /// Create the right-handed perspective projection transformation.
        ///
        /// This transformation assumes the following constraints:
        ///     * The x coordinates go from -1 to 1 (left to right direction).
        ///     * The y coordinates go from -1 to 1 (up to down direction, distinct from OpenGL convention).
        ///     * The z coordinates go from 0 to 1 (forward to back direction) distinct from OpenGL convention.
        /// This transformation is suited for programs working with Vulkan, which assumes the exact
        /// constraints seen above.
        static psh_inline ColMat4
        perspective_projection_rhzo(f32 fovy, f32 aspect, f32 near_plane, f32 far_plane) noexcept {
            f32 tan_hfovy = tanf(fovy * 0.5f);

            // clang-format off
            return ColMat4{
                1 / (aspect * tan_hfovy),      0.0f,                               0.0f,                             0.0f,
                         0.0f,            -(1 / tan_hfovy),                        0.0f,                             0.0f,
                         0.0f,                 0.0f,            -(far_plane / (far_plane - near_plane)),            -1.0f,
                         0.0f,                 0.0f,        -((far_plane * near_plane) / (far_plane - near_plane)),  0.0f,
            };
            // clang-format on
        }

        /// Create the right-handed orthogonal projection transformation.
        ///
        /// This transformation assumes the following constraints:
        ///     * The x coordinates go from -1 to 1 (left to right direction).
        ///     * The y coordinates go from -1 to 1 (up to down direction, distinct from OpenGL convention).
        ///     * The z coordinates go from 0 to 1 (forward to back direction) distinct from OpenGL convention.
        /// This transformation is suited for programs working with Vulkan, which assumes the exact
        /// constraints seen above.
        static ColMat4 orthographic_projection_rhzo(
            f32 left,
            f32 right,
            f32 bottom,
            f32 top,
            f32 near_plane,
            f32 far_plane) noexcept {
            // clang-format off
            return ColMat4{
                     2.0f / (right - left),                  0.0f,                            0.0f,                      0.0f,
                              0.0f,                  2.0f / (top - bottom),                   0.0f,                      0.0f,
                              0.0f,                          0.0f,                  1.0f / (near_plane - far_plane),     0.0f,
                (left + right) / (left - right), (bottom + top) / (bottom - top), near_plane / (near_plane - far_plane), 1.0f,
            };
            // clang-format on
        }

        // @TODO:
        // - perspective_projection_rhno following OpenGL conventions.
        // - orthographic_projection_rhno following OpenGL conventions.
    };

    /// Left-multiply a 2D vector by a 2D square matrix.
    psh_api Vec2 mat_mul(Mat2 m, Vec2 v) noexcept;

    /// Left-multiply a 3D vector by a 3D square matrix.
    psh_api Vec3 mat_mul(Mat3 m, Vec3 v) noexcept;

    /// Multiply a pair of 3D square matrices.
    psh_api Mat3 mat_mul(Mat3 lhs, Mat3 rhs) noexcept;

    /// Left-multiply a 4D vector by a 4D square column-major matrix.
    psh_api Vec4 mat_mul(ColMat4 m, Vec4 v) noexcept;

    /// Multiply a pair of 4D square column-major matrices.
    psh_api ColMat4 mat_mul(ColMat4 lhs, ColMat4 rhs) noexcept;
}  // namespace psh
