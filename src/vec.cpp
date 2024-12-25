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
/// Description: Implementation of vector types.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/vec.hpp>

#include <math.h>

namespace psh {
    // -------------------------------------------------------------------------------------------------
    // Row-major 3-dimensional square matrix in floating-point space.
    // -------------------------------------------------------------------------------------------------

    Mat3 Mat3::rotation_tb(f32 rot_x, f32 rot_y, f32 rot_z) psh_no_except {
        f32 xsin = sinf(rot_x);
        f32 ysin = sinf(rot_y);
        f32 zsin = sinf(rot_z);
        f32 xcos = cosf(rot_x);
        f32 ycos = cosf(rot_y);
        f32 zcos = cosf(rot_z);

        // clang-format off
        return Mat3{
            (zcos * ycos), (zcos * ysin * xsin - zsin * xcos), (xcos * ysin * xcos + zsin * xsin),
            (zsin * ycos), (zsin * ysin * xsin + zcos * xcos), (zsin * ysin * xcos - zcos * xsin),
               (-ysin),                  (ycos * xsin),                      (ycos * xcos),
        };
        // clang-format on
    }

    // -------------------------------------------------------------------------------------------------
    // Implementation of the matrix multiplication operations.
    // -------------------------------------------------------------------------------------------------

    Vec2 mat_mul(Mat2 m, Vec2 v) psh_no_except {
        return Vec2{
            (m.buf[0] * v.x) + (m.buf[1] * v.y),
            (m.buf[2] * v.x) + (m.buf[3] * v.y),
        };
    }

    Vec3 mat_mul(Mat3 m, Vec3 v) psh_no_except {
        return Vec3{
            (m.buf[0] * v.x) + (m.buf[1] * v.y) + (m.buf[2] * v.z),
            (m.buf[3] * v.x) + (m.buf[4] * v.y) + (m.buf[5] * v.z),
            (m.buf[6] * v.x) + (m.buf[7] * v.y) + (m.buf[8] * v.z),
        };
    }

    Mat3 mat_mul(Mat3 lhs, Mat3 rhs) psh_no_except {
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

    Vec4 mat_mul(ColMat4 m, Vec4 v) psh_no_except {
        return Vec4{
            (m.buf[0] * v.x) + (m.buf[4] * v.y) + (m.buf[8] * v.z) + (m.buf[12] * v.w),
            (m.buf[1] * v.x) + (m.buf[5] * v.y) + (m.buf[9] * v.z) + (m.buf[13] * v.w),
            (m.buf[2] * v.x) + (m.buf[6] * v.y) + (m.buf[10] * v.z) + (m.buf[14] * v.w),
            (m.buf[3] * v.x) + (m.buf[7] * v.y) + (m.buf[11] * v.z) + (m.buf[15] * v.w),
        };
    }

    ColMat4 mat_mul(ColMat4 lhs, ColMat4 rhs) psh_no_except {
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
