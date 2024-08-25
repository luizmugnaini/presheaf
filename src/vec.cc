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
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/vec.h>

#include <psh/assert.h>
#include <cmath>

namespace psh {
    // -----------------------------------------------------------------------------
    // - 2-dimensional floating-point vector -
    // -----------------------------------------------------------------------------

    Vec2& Vec2::operator+=(Vec2 const& other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& Vec2::operator-=(Vec2 const& other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& Vec2::operator*=(Vec2 const& other) noexcept {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    Vec2& Vec2::operator*=(f32 scalar) noexcept {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    bool Vec2::is_zero(f32 zero_range) const noexcept {
        return f32_approx_equal(x, 0.0f, zero_range) && f32_approx_equal(y, 0.0f, zero_range);
    }

    Vec2 Vec2::normalized() const noexcept {
        f32 len = std::sqrt(x * x + y * y);

        if (psh_unlikely(len < F32_IS_ZERO_RANGE)) {
            return Vec2{};
        }

        return Vec2{x / len, y / len};
    }

    f32 Vec2::dot(Vec2 const& other) const noexcept {
        return x * other.x + y * other.y;
    }

    bool Vec2::is_to_the_left_of(Vec2 const& other) const noexcept {
        return ((other.x * y - other.y * x) >= 0.0f);
    }

    Vec2 operator+(Vec2 lhs, Vec2 rhs) noexcept {
        return Vec2{lhs.x + rhs.x, lhs.y + rhs.y};
    }

    Vec2 operator-(Vec2 lhs, Vec2 rhs) noexcept {
        return Vec2{lhs.x - rhs.x, lhs.y - rhs.y};
    }

    Vec2 operator*(Vec2 lhs, Vec2 rhs) noexcept {
        return Vec2{lhs.x * rhs.x, lhs.y * rhs.y};
    }

    Vec2 operator*(Vec2 v, f32 scalar) noexcept {
        return Vec2{v.x * scalar, v.y * scalar};
    }

    Vec2 operator-(Vec2 v) noexcept {
        return Vec2{-v.x, -v.y};
    }

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

    bool Vec3::is_zero(f32 zero_range) const noexcept {
        return f32_approx_equal(x, 0.0f, zero_range) &&
               f32_approx_equal(y, 0.0f, zero_range) &&
               f32_approx_equal(z, 0.0f, zero_range);
    }

    Vec3 Vec3::normalized() const noexcept {
        f32 len = std::sqrt(x * x + y * y + z * z);

        if (psh_unlikely(f32_approx_equal(len, 0.0f))) {
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

    Vec3 operator+(Vec3 lhs, Vec3 rhs) noexcept {
        return Vec3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
    }

    Vec3 operator-(Vec3 lhs, Vec3 rhs) noexcept {
        return Vec3{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
    }

    Vec3 operator*(Vec3 lhs, Vec3 rhs) noexcept {
        return Vec3{lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
    }

    Vec3 operator*(Vec3 v, f32 scalar) noexcept {
        return Vec3{v.x * scalar, v.y * scalar, v.z * scalar};
    }

    Vec3 operator-(Vec3 v) noexcept {
        return Vec3{-v.x, -v.y, -v.z};
    }

    // -----------------------------------------------------------------------------
    // - 2-dimensional integer vector -
    // -----------------------------------------------------------------------------

    IVec2& IVec2::operator+=(IVec2 const& other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }

    IVec2& IVec2::operator-=(IVec2 const& other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    IVec2& IVec2::operator*=(IVec2 const& other) noexcept {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    IVec2& IVec2::operator*=(i32 scalar) noexcept {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    bool IVec2::is_zero() const noexcept {
        return (x == 0) && (y == 0);
    }

    Vec2 IVec2::normalized() const noexcept {
        f32 len = std::sqrt(static_cast<f32>(x * x + y * y));

        if (psh_unlikely(len < F32_IS_ZERO_RANGE)) {
            return Vec2{};
        }

        return Vec2{static_cast<f32>(x) / len, static_cast<f32>(y) / len};
    }

    i32 IVec2::dot(IVec2 const& other) const noexcept {
        return x * other.x + y * other.y;
    }

    IVec2 operator+(IVec2 lhs, IVec2 rhs) noexcept {
        return IVec2{lhs.x + rhs.x, lhs.y + rhs.y};
    }

    IVec2 operator-(IVec2 lhs, IVec2 rhs) noexcept {
        return IVec2{lhs.x - rhs.x, lhs.y - rhs.y};
    }

    IVec2 operator-(IVec2 v) noexcept {
        return IVec2{-v.x, -v.y};
    }

    IVec2 operator*(IVec2 lhs, IVec2 rhs) noexcept {
        return IVec2{lhs.x * rhs.x, lhs.y * rhs.y};
    }

    IVec2 operator*(IVec2 v, i32 scalar) noexcept {
        return IVec2{v.x * scalar, v.y * scalar};
    }

    bool operator==(IVec2 lhs, IVec2 rhs) noexcept {
        return (lhs.x == rhs.x) && (lhs.y == rhs.y);
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

    bool IVec3::is_zero() const noexcept {
        return (x == 0) && (y == 0) && (z == 0);
    }

    Vec3 IVec3::normalized() const noexcept {
        f32 len = std::sqrt(static_cast<f32>(x * x + y * y + z * z));

        if (psh_unlikely(len < F32_IS_ZERO_RANGE)) {
            return Vec3{};
        }

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

    IVec3 operator+(IVec3 lhs, IVec3 rhs) noexcept {
        return IVec3{lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
    }

    IVec3 operator-(IVec3 lhs, IVec3 rhs) noexcept {
        return IVec3{lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
    }

    IVec3 operator-(IVec3 v) noexcept {
        return IVec3{-v.x, -v.y, -v.z};
    }

    IVec3 operator*(IVec3 lhs, IVec3 rhs) noexcept {
        return IVec3{lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z};
    }

    IVec3 operator*(IVec3 v, i32 scalar) noexcept {
        return IVec3{v.x * scalar, v.y * scalar, v.z * scalar};
    }

    bool operator==(IVec3 lhs, IVec3 rhs) noexcept {
        return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
    }

    // -----------------------------------------------------------------------------
    // - Row-major 3-dimensional square matrix in floating-point space -
    // -----------------------------------------------------------------------------

    f32& Mat3::at(u32 r, u32 c) noexcept {
        psh_assert_msg(r <= 3, "Mat3::at called with row outside range");
        psh_assert_msg(c <= 3, "Mat3::at called with column outside range");

        return buf[r + c * 3];
    }

    Mat3 Mat3::id() noexcept {
        // clang-format off
        return Mat3{
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
        };
        // clang-format on
    }

    Mat3 Mat3::change_of_basis(Vec3 v1, Vec3 v2, Vec3 v3) noexcept {
        // TODO: should we check if v1, v2, v3 form an orthogonal triple?

        // clang-format off
        return Mat3{
            v1.x, v2.x, v3.x,
            v1.y, v2.y, v3.y,
            v1.z, v2.z, v3.z,
        };
        // clang-format on
    }

    Mat3 Mat3::rotation_tb(f32 rot_x, f32 rot_y, f32 rot_z) noexcept {
        f32 xsin = std::sin(rot_x);
        f32 ysin = std::sin(rot_y);
        f32 zsin = std::sin(rot_z);
        f32 xcos = std::cos(rot_x);
        f32 ycos = std::cos(rot_y);
        f32 zcos = std::cos(rot_z);

        // clang-format off
        return psh::Mat3{
            (zcos * ycos), (zcos * ysin * xsin - zsin * xcos), (xcos * ysin * xcos + zsin * xsin),
            (zsin * ycos), (zsin * ysin * xsin + zcos * xcos), (zsin * ysin * xcos - zcos * xsin),
               (-ysin),                  (ycos * xsin),                      (ycos * xcos),
        };
        // clang-format on
    }

    // -----------------------------------------------------------------------------
    // - Column-major 3-dimensional square matrix in floating-point space -
    // -----------------------------------------------------------------------------

    f32& ColMat3::at(u32 r, u32 c) noexcept {
        psh_assert_msg(r <= 3, "ColMat3::at called with row outside range");
        psh_assert_msg(c <= 3, "ColMat3::at called with column outside range");

        return buf[r * 3 + c];
    }

    ColMat3 ColMat3::id() noexcept {
        // clang-format off
        return ColMat3{
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
        };
        // clang-format on
    }

    // -----------------------------------------------------------------------------
    // - Column-major 4-dimensional square matrix in floating-point space -
    // -----------------------------------------------------------------------------

    f32& ColMat4::at(u32 r, u32 c) noexcept {
        psh_assert_msg(r <= 4, "ColMat4::at called with row outside range");
        psh_assert_msg(c <= 4, "ColMat4::at called with column outside range");

        return buf[r * 4 + c];
    }

    ColMat4 ColMat4::id() noexcept {
        // clang-format off
        return ColMat4{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
        // clang-format on
    }

    ColMat4 ColMat4::scale(Vec3 scaling) noexcept {
        // clang-format off
        return ColMat4{
            scaling.x,    0.0f,      0.0f,   0.0f,
               0.0f,   scaling.y,    0.0f,   0.0f,
               0.0f,      0.0f,   scaling.z, 0.0f,
               0.0f,      0.0f,      0.0f,   1.0f,
        };
        // clang-format on
    }

    ColMat4 ColMat4::translation(Vec3 dx_dy_dz) noexcept {
        // clang-format off
        return ColMat4{
               1.0f,       0.0f,       0.0f,    0.0f,
               0.0f,       1.0f,       0.0f,    0.0f,
               0.0f,       0.0f,       1.0f,    0.0f,
            dx_dy_dz.x, dx_dy_dz.y, dx_dy_dz.z, 1.0f,
        };
        // clang-format on
    }

    ColMat4 ColMat4::view_direction_rh(Vec3 eye, Vec3 view_direction, Vec3 view_up) noexcept {
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

    ColMat4 ColMat4::look_at_rh(Vec3 eye, Vec3 target, Vec3 view_up) noexcept {
        return ColMat4::view_direction_rh(eye, target - eye, view_up);
    }

    ColMat4 ColMat4::perspective_projection_rhzo(
        f32 fovy,
        f32 aspect,
        f32 near_plane,
        f32 far_plane) noexcept {
        f32 tan_hfovy = std::tan(fovy * 0.5f);

        // clang-format off
        return ColMat4{
            1 / (aspect * tan_hfovy),      0.0f,                               0.0f,                             0.0f,
                     0.0f,            -(1 / tan_hfovy),                        0.0f,                             0.0f,
                     0.0f,                 0.0f,            -(far_plane / (far_plane - near_plane)),            -1.0f,
                     0.0f,                 0.0f,        -((far_plane * near_plane) / (far_plane - near_plane)),  0.0f,
        };
        // clang-format on
    }

    ColMat4 ColMat4::orthographic_projection_rhzo(
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

    // -----------------------------------------------------------------------------
    // - Implementation of the matrix multiplication operations -
    // -----------------------------------------------------------------------------

    Vec2 mat_mul(Mat2 m, Vec2 v) noexcept {
        return Vec2{
            (m.buf[0] * v.x) + (m.buf[1] * v.y),
            (m.buf[2] * v.x) + (m.buf[3] * v.y),
        };
    }

    Vec3 mat_mul(Mat3 m, Vec3 v) noexcept {
        return Vec3{
            (m.buf[0] * v.x) + (m.buf[1] * v.y) + (m.buf[2] * v.z),
            (m.buf[3] * v.x) + (m.buf[4] * v.y) + (m.buf[5] * v.z),
            (m.buf[6] * v.x) + (m.buf[7] * v.y) + (m.buf[8] * v.z),
        };
    }

    Mat3 mat_mul(Mat3 lhs, Mat3 rhs) noexcept {
        return Mat3{
            // Row 1.
            (lhs.buf[0] * rhs.buf[0]) + (lhs.buf[1] * rhs.buf[3]) + (lhs.buf[2] * rhs.buf[6]),
            (lhs.buf[0] * rhs.buf[1]) + (lhs.buf[1] * rhs.buf[4]) + (lhs.buf[2] * rhs.buf[7]),
            (lhs.buf[0] * rhs.buf[2]) + (lhs.buf[1] * rhs.buf[5]) + (lhs.buf[2] * rhs.buf[8]),
            // Row 2.
            (lhs.buf[3] * rhs.buf[0]) + (lhs.buf[4] * rhs.buf[3]) + (lhs.buf[5] * rhs.buf[6]),
            (lhs.buf[3] * rhs.buf[1]) + (lhs.buf[4] * rhs.buf[4]) + (lhs.buf[5] * rhs.buf[7]),
            (lhs.buf[3] * rhs.buf[2]) + (lhs.buf[4] * rhs.buf[5]) + (lhs.buf[5] * rhs.buf[8]),
            // Row 3.
            (lhs.buf[6] * rhs.buf[0]) + (lhs.buf[7] * rhs.buf[3]) + (lhs.buf[8] * rhs.buf[6]),
            (lhs.buf[6] * rhs.buf[1]) + (lhs.buf[7] * rhs.buf[4]) + (lhs.buf[8] * rhs.buf[7]),
            (lhs.buf[6] * rhs.buf[2]) + (lhs.buf[7] * rhs.buf[5]) + (lhs.buf[8] * rhs.buf[8]),
        };
    }

    Vec4 mat_mul(ColMat4 m, Vec4 v) noexcept {
        return Vec4{
            (m.buf[0] * v.x) + (m.buf[4] * v.y) + (m.buf[8] * v.z) + (m.buf[12] * v.w),
            (m.buf[1] * v.x) + (m.buf[5] * v.y) + (m.buf[9] * v.z) + (m.buf[13] * v.w),
            (m.buf[2] * v.x) + (m.buf[6] * v.y) + (m.buf[10] * v.z) + (m.buf[14] * v.w),
            (m.buf[3] * v.x) + (m.buf[7] * v.y) + (m.buf[11] * v.z) + (m.buf[15] * v.w),
        };
    }

    ColMat4 mat_mul(ColMat4 lhs, ColMat4 rhs) noexcept {
        // clang-format off
        return ColMat4{
            // Row 1.
            (lhs.buf[0] * rhs.buf[0]) + (lhs.buf[1] * rhs.buf[4]) + (lhs.buf[2] * rhs.buf[8])  + (lhs.buf[3] * rhs.buf[12]),
            (lhs.buf[0] * rhs.buf[1]) + (lhs.buf[1] * rhs.buf[5]) + (lhs.buf[2] * rhs.buf[9])  + (lhs.buf[3] * rhs.buf[13]),
            (lhs.buf[0] * rhs.buf[2]) + (lhs.buf[1] * rhs.buf[6]) + (lhs.buf[2] * rhs.buf[10]) + (lhs.buf[3] * rhs.buf[14]),
            (lhs.buf[0] * rhs.buf[3]) + (lhs.buf[1] * rhs.buf[7]) + (lhs.buf[2] * rhs.buf[11]) + (lhs.buf[3] * rhs.buf[15]),
            // Row 2.
            (lhs.buf[4] * rhs.buf[0]) + (lhs.buf[5] * rhs.buf[4]) + (lhs.buf[6] * rhs.buf[8])  + (lhs.buf[7] * rhs.buf[12]),
            (lhs.buf[4] * rhs.buf[1]) + (lhs.buf[5] * rhs.buf[5]) + (lhs.buf[6] * rhs.buf[9])  + (lhs.buf[7] * rhs.buf[13]),
            (lhs.buf[4] * rhs.buf[2]) + (lhs.buf[5] * rhs.buf[6]) + (lhs.buf[6] * rhs.buf[10]) + (lhs.buf[7] * rhs.buf[14]),
            (lhs.buf[4] * rhs.buf[3]) + (lhs.buf[5] * rhs.buf[7]) + (lhs.buf[6] * rhs.buf[11]) + (lhs.buf[7] * rhs.buf[15]),
            // Row 3.
            (lhs.buf[8] * rhs.buf[0]) + (lhs.buf[9] * rhs.buf[4]) + (lhs.buf[10] * rhs.buf[8])  + (lhs.buf[11] * rhs.buf[12]),
            (lhs.buf[8] * rhs.buf[1]) + (lhs.buf[9] * rhs.buf[5]) + (lhs.buf[10] * rhs.buf[9])  + (lhs.buf[11] * rhs.buf[13]),
            (lhs.buf[8] * rhs.buf[2]) + (lhs.buf[9] * rhs.buf[6]) + (lhs.buf[10] * rhs.buf[10]) + (lhs.buf[11] * rhs.buf[14]),
            (lhs.buf[8] * rhs.buf[3]) + (lhs.buf[9] * rhs.buf[7]) + (lhs.buf[10] * rhs.buf[11]) + (lhs.buf[11] * rhs.buf[15]),
            // Row 4.
            (lhs.buf[12] * rhs.buf[0]) + (lhs.buf[13] * rhs.buf[4]) + (lhs.buf[14] * rhs.buf[8])  + (lhs.buf[15] * rhs.buf[12]),
            (lhs.buf[12] * rhs.buf[1]) + (lhs.buf[13] * rhs.buf[5]) + (lhs.buf[14] * rhs.buf[9])  + (lhs.buf[15] * rhs.buf[13]),
            (lhs.buf[12] * rhs.buf[2]) + (lhs.buf[13] * rhs.buf[6]) + (lhs.buf[14] * rhs.buf[10]) + (lhs.buf[15] * rhs.buf[14]),
            (lhs.buf[12] * rhs.buf[3]) + (lhs.buf[13] * rhs.buf[7]) + (lhs.buf[14] * rhs.buf[11]) + (lhs.buf[15] * rhs.buf[15]),
        };
        // clang-format on
    }
}  // namespace psh
