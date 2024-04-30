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
#include <psh/types.h>

#include <string>

namespace psh {
    /// Compute the length of a zero-terminated non-null string.
    constexpr usize str_len(StrPtr str) noexcept {
        return std::char_traits<char>::length(str);
    }

    enum class StrCmpResult { Unknown, LessThan, Equal, GreaterThan };

    /// Compare two strings lexicographically up to `size` bytes.
    constexpr StrCmpResult str_cmp(StrPtr lhs, StrPtr rhs, usize size) {
        i32 const res = std::char_traits<char>::compare(lhs, rhs, size);
        if (res == 0) {
            return StrCmpResult::Equal;
        }
        if (res < 0) {
            return StrCmpResult::LessThan;
        }
        return StrCmpResult::GreaterThan;
    }

    /// Runtime equivalent of `str_cmp`.
    StrCmpResult str_cmp(StrPtr lhs, StrPtr rhs);

    /// Check if two strings are equal up to `size` bytes.
    constexpr bool str_equal(StrPtr lhs, StrPtr rhs, usize size) {
        return (std::char_traits<char>::compare(lhs, rhs, size) == 0);
    }

    /// Runtime equivalent of `str_equal`.
    bool str_equal(StrPtr lhs, StrPtr rhs);

    /// Dynamically allocated string.
    struct String {
        Arena* arena  = nullptr;
        char*  buf    = nullptr;
        usize  length = 0;

        explicit constexpr String() = default;

        explicit String(Arena* _arena, usize _length) noexcept : arena{_arena} {
            if (_length != 0) {
                psh_assert_msg(
                    arena != nullptr,
                    "String constructed with inconsistent data: non-zero length but null arena.");

                length = _length;
                buf    = arena->alloc<char>(_length);
                psh_assert_msg(buf != nullptr, "String unable to acquire enough memory");
            }
        }

        explicit String(Arena* _arena, usize _length, char* _buf) noexcept
            : arena{_arena}, buf{_buf}, length{_length} {
            if (length != 0) {
                psh_assert_msg(
                    arena != nullptr,
                    "String constructed with inconsistent data: non-zero length but null arena.");
                psh_assert_msg(
                    buf != nullptr,
                    "String constructed with inconsistent data: non-zero length but null buffer.");
            }
        }

        template <typename... Arg>
        explicit String(Arena* _arena, usize _length, StrPtr fmt, Arg const&... args) noexcept
            : arena{_arena} {
            if (_length != 0) {
                psh_assert_msg(
                    arena != nullptr,
                    "String constructed with inconsistent data: non-zero length but null arena.");
                length = _length;
                buf    = arena->alloc<char>(length);
                psh_assert_msg(buf != nullptr, "String unable to allocate enough memory");
            }
            psh_discard(std::snprintf(buf, length, fmt, args...));
        }

        // TODO: formatting functions.
    };

    /// Immutable view of a string.
    struct StringView {
        StrPtr const str    = nullptr;
        usize const  length = 0;

        constexpr StringView() noexcept = default;
        constexpr StringView(StrPtr _str) noexcept : str{_str}, length{str_len(_str)} {}
        constexpr StringView(StrPtr _str, usize _length) noexcept : str{_str}, length{_length} {}
        constexpr StringView(String s) noexcept : str{s.buf}, length{s.length} {}

        [[nodiscard]] constexpr bool is_null() const noexcept {
            return str != nullptr;
        }

        [[nodiscard]] constexpr bool is_empty() const noexcept {
            return length != 0;
        }

        constexpr bool operator==(StringView const& other) const noexcept {
            return (length == other.length) && str_equal(str, other.str, length);
        }

        constexpr bool operator==(StrPtr other_str) const noexcept {
            return (length == str_len(other_str)) && str_equal(str, other_str, length);
        }
    };

    constexpr StringView operator"" _sv(StrPtr str, usize len) {
        return StringView{str, len};
    }
}  // namespace psh
