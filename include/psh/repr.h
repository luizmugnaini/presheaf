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
/// Description: Conversion between number representations.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/arena.h>
#include <psh/string.h>

namespace psh {
    inline char digit_to_char(u8 digit) noexcept {
        psh_assert_msg(digit < 10, "Expected digit to be between 0 and 9");
        return '0' + digit;
    }

    /// Get the binary representation of a number
    template <typename T>
    String binary_repr(Arena* arena, T val) noexcept {
        constexpr usize BIT_COUNT = psh_value_bit_count(val);
        String          repr{arena, BIT_COUNT + 1};

        repr.data.size = BIT_COUNT;
        for (usize idx = 0; idx < repr.data.size; ++idx) {
            repr.data[repr.data.size - 1 - idx] = digit_to_char(psh_bit_at(val, idx));
        }
        return repr;
    }
}  // namespace psh
