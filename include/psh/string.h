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
#include <psh/dyn_array.h>
#include <psh/intrinsics.h>
#include <psh/option.h>
#include <psh/types.h>

/// Create a string literal and a string view at compile time from a given C-string literal.
///
/// Note: Use this macro with care, you should only use it with literal strings, otherwise the
///       length of the string won't be computed corrected by the compile and you'll obtain the
///       size of a pointer as the length of the string.
#define psh_str(cstr_literal)          \
    psh::Str<sizeof((cstr_literal))> { \
        (cstr_literal)                 \
    }
#define psh_string_view(cstr_literal)              \
    psh::StringView {                              \
        (cstr_literal), sizeof((cstr_literal)) - 1 \
    }

namespace psh {
    enum class StrCmpResult { Unknown, LessThan, Equal, GreaterThan };

    usize        str_size(strptr str) noexcept;
    StrCmpResult str_cmp(strptr lhs, strptr rhs) noexcept;
    bool         str_equal(strptr lhs, strptr rhs) noexcept;
    bool         is_utf8(char c) noexcept;

    /// A string with guaranteed compile-time known size.
    ///
    /// Note: It is way more ergonomic to use the macro `psh_str` than specifying the length of the
    ///       string, which is silly.
    ///
    /// Example:
    /// ```cpp
    /// psh::Str my_str = psh_str("hey this is a compile time array of constant characters");
    /// ```
    template <usize size_>
    struct Str {
        char const buf[size_];

        /// The size of the string, disregarding its null-terminator.
        constexpr usize size() const noexcept {
            return size_ - 1;
        }
    };

    /// Immutable view of a string.
    struct StringView {
        FatPtr<char const> const data;

        constexpr explicit StringView() noexcept = default;
        constexpr explicit StringView(strptr _str, usize _length) noexcept : data{_str, _length} {}
        StringView(strptr _str) noexcept;
    };

    /// Dynamically allocated string.
    struct String {
        DynArray<char> data;

        explicit constexpr String() = default;
        explicit String(Arena* arena, usize capacity) noexcept;
        explicit String(Arena* arena, StringView sv) noexcept;
        void init(Arena* arena, usize capacity) noexcept;
        void init(Arena* arena, StringView sv) noexcept;

        /// Make a view from the string.
        StringView view() const noexcept;

        // Join an array of string views to the current string data. You can also provide a string
        // to be attached to the end of each join.
        //
        // Example:
        // ```cpp
        // psh::Arena arena{...};
        // psh::String s{&arena, "Hello"};
        // assert(s.join({"World", "Earth", "Terra"}, ", ") == psh::Result::OK);
        // assert(std::strcmp(s.data.buf, "Hello, World, Earth, Terra") == 0);
        // ```
        // Although this example uses initializer lists, you can also achieve the same using the
        // implementation based on `psh::FatPtr`.
        Status join(FatPtr<StringView const> strs, strptr join_cstr = nullptr) noexcept;
        Status join(std::initializer_list<StringView> strs, strptr join_cstr = nullptr) noexcept;
    };
}  // namespace psh
