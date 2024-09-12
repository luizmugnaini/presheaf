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
        if (val == 0) {
            return String{arena, StringView{"0b0"}};
        }

        constexpr i32 BIT_COUNT = psh_type_bit_count(T);
        String        repr{arena, 2 + BIT_COUNT + 1};

        repr.data.buf[0] = '0';
        repr.data.buf[1] = 'b';
        repr.data.size += 2;

        i32 leading_zeros_count = 0;
        for (i32 idx = BIT_COUNT - 1; idx >= 0; --idx) {
            if (psh_bit_at(val, idx) == 0) {
                ++leading_zeros_count;
            } else {
                break;
            }
        }

        for (i32 idx = BIT_COUNT - 1 - leading_zeros_count; idx >= 0; --idx) {
            repr.data[repr.data.size++] = digit_to_char(psh_bit_at(val, idx));
        }

        return repr;
    }
}  // namespace psh
