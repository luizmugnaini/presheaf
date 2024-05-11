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
/// Description: String related types and utility functions.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/arena.h>
#include <psh/assert.h>
#include <psh/intrinsics.h>
#include <psh/types.h>

#include <string>

namespace psh {
    /// Compute the length of a zero-terminated non-null string.
    constexpr usize str_len(strptr str) noexcept {
        return std::char_traits<char>::length(str);
    }

    enum class StrCmpResult { Unknown, LessThan, Equal, GreaterThan };

    /// Compare two strings lexicographically up to `size` bytes.
    constexpr StrCmpResult str_cmp(strptr lhs, strptr rhs, usize size) {
        i32 const res = std::char_traits<char>::compare(lhs, rhs, size);
        if (res == 0) return StrCmpResult::Equal;
        if (res < 0) return StrCmpResult::LessThan;
        return StrCmpResult::GreaterThan;
    }

    /// Runtime equivalent of `str_cmp`.
    StrCmpResult str_cmp(strptr lhs, strptr rhs);

    /// Check if two strings are equal up to `size` bytes.
    constexpr bool str_equal(strptr lhs, strptr rhs, usize size) {
        return (std::char_traits<char>::compare(lhs, rhs, size) == 0);
    }

    /// Runtime equivalent of `str_equal`.
    bool str_equal(strptr lhs, strptr rhs);

    struct StringView;

    /// Dynamically allocated string.
    struct String {
        Arena* arena    = nullptr;
        usize  size     = 0;
        usize  capacity = 0;
        char*  buf      = nullptr;

        explicit constexpr String() = default;
        explicit String(Arena* _arena, usize _capacity) noexcept;
        explicit String(Arena* _arena, usize _size, usize _capacity, char* _buf) noexcept;

        StringView view() const noexcept;

        // TODO(luiz): formatting functions.
    };

    /// Immutable view of a string.
    struct StringView {
        strptr const buf    = nullptr;
        usize const  length = 0;

        constexpr StringView() noexcept = default;
        constexpr StringView(strptr _str) noexcept : buf{_str}, length{str_len(_str)} {}
        constexpr StringView(strptr _str, usize _length) noexcept : buf{_str}, length{_length} {}

        bool is_empty() const noexcept;

        bool operator==(StringView const& other) const noexcept;
        bool operator==(strptr other_str) const noexcept;
    };

    constexpr StringView operator"" _sv(strptr str, usize len) {
        return StringView{str, len};
    }
}  // namespace psh
