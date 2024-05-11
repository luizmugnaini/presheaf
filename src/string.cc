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
/// Description: Implementation of the string related types and utility functions.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#include <psh/string.h>

#include <cstring>

namespace psh {
    StrCmpResult str_cmp(strptr lhs, strptr rhs) {
        i32 const cmp = std::strcmp(lhs, rhs);

        StrCmpResult res;
        if (cmp == 0) {
            res = StrCmpResult::Equal;
        } else if (cmp < 0) {
            res = StrCmpResult::LessThan;
        } else {
            res = StrCmpResult::GreaterThan;
        }

        return res;
    }

    bool str_equal(strptr lhs, strptr rhs) {
        return (std::strcmp(lhs, rhs) == 0);
    }

    String::String(Arena* _arena, usize _capacity) noexcept : arena{_arena}, capacity{_capacity} {
        if (psh_unlikely(capacity == 0)) return;

        psh_assert_msg(
            arena != nullptr,
            "String constructed with inconsistent data: non-zero size but null arena");

        buf = arena->alloc<char>(capacity);
        psh_assert_msg(buf != nullptr, "String unable to acquire enough memory");
    }

    String::String(Arena* _arena, usize _size, usize _capacity, char* _buf) noexcept
        : arena{_arena}, size{_size}, capacity{_capacity}, buf{_buf} {
        psh_assert_msg(
            size <= capacity,
            "String constructed with inconsistent data: size greater than capacity");

        if (psh_unlikely(size == 0)) return;

        psh_assert_msg(
            arena != nullptr,
            "String constructed with inconsistent data: non-zero size but null arena");
        psh_assert_msg(
            buf != nullptr,
            "String constructed with inconsistent data: non-zero size but null buffer");
    }
}  // namespace psh
