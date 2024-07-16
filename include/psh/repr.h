///                            Presheaf Library
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
/// Description: Conversion between number representations.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/arena.h>
#include <psh/string.h>
#include <psh/type_utils.h>

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
