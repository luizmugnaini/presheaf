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

#include <psh/core.h>
#include <psh/log.h>
#include <limits>

namespace psh {
    constexpr f32 PI                = 3.14159265359f;
    constexpr f32 F32_IS_ZERO_RANGE = 1e-6f;

    constexpr bool f32_approx_equal(f32 a, f32 b, f32 zero_range = F32_IS_ZERO_RANGE) noexcept {
#if defined(PSH_DEBUG)
        psh_assert_msg(zero_range > 0.0f, "Expected the 'within zero range' value to be positive");
#endif
        f32 sub = a - b;
        return (-zero_range < sub) && (sub < zero_range);
    }

    /// Add two values wrapping the result to the corresponding maximal numeric limit.
    template <typename T>
    constexpr T wrap_add(T a, T b) noexcept {
        T c = a + b;
        return (c >= a) ? c : std::numeric_limits<T>::max();
    }

    /// Subtract two values wrapping the result to the corresponding minimal numeric limit.
    template <typename T>
    constexpr T wrap_sub(T a, T b) noexcept {
        T c = a - b;
        return (c <= a) ? c : std::numeric_limits<T>::min();
    }

    constexpr f32 as_radians(f32 deg) noexcept {
        return deg * PI / 180.0f;
    }

    template <typename T>
    constexpr T next_multiple(T current, T mul) noexcept {
#if defined(PSH_DEBUG)
        psh_assert_msg(mul != 0, "next_multiple expected the multiple base to be non-zero");
#endif
        return mul * static_cast<T>(static_cast<i64>(current / mul) + 1);
    }
}  // namespace psh
