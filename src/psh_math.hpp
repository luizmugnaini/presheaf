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
/// Description: Mathematical utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <limits.h>
#include "psh_core.hpp"
#include "psh_debug.hpp"

namespace psh {
    psh_global constexpr f32 PI                = 3.14159265359f;
    psh_global constexpr f32 F32_IS_ZERO_RANGE = 1e-6f;

    psh_proc constexpr bool approx_equal(f32 a, f32 b, f32 zero_range = F32_IS_ZERO_RANGE) psh_no_except {
        psh_paranoid_validate_usage({
            psh_assert_msg(zero_range > 0.0f, "Expected the 'within zero range' value to be positive.");
        });
        f32 sub = a - b;
        return (-zero_range < sub) && (sub < zero_range);
    }

    psh_proc constexpr f32 as_radians(f32 deg) psh_no_except {
        return deg * PI / 180.0f;
    }

    psh_proc constexpr u32 no_wrap_sub(u32 a, u32 b) psh_no_except {
        u32 c = a - b;
        return (c <= a) ? c : 0;
    }

    psh_proc constexpr u64 no_wrap_sub(u64 a, u64 b) psh_no_except {
        u64 c = a - b;
        return (c <= a) ? c : 0;
    }
}  // namespace psh
