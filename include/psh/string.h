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
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/arena.h>
#include <psh/assert.h>
#include <psh/core.h>
#include <psh/dyn_array.h>
#include <psh/option.h>

namespace psh {
    // -----------------------------------------------------------------------------
    // - String comparison utilities -
    // -----------------------------------------------------------------------------

    enum struct StrCmpResult { LESS_THAN,
                               EQUAL,
                               GREATER_THAN };

    usize        str_size(strptr str) noexcept;
    StrCmpResult str_cmp(strptr lhs, strptr rhs) noexcept;
    bool         str_equal(strptr lhs, strptr rhs) noexcept;
    bool         is_utf8(char c) noexcept;

    // -----------------------------------------------------------------------------
    // - String types -
    // -----------------------------------------------------------------------------

    /// String literal type.
    struct StringLiteral {
        strptr str;

        template <usize N>
        consteval StringLiteral(char const (&_str)[N]) noexcept
            : str{_str} {}
    };

    /// A string with guaranteed compile-time known size.
    ///
    /// Note: It is way more ergonomic to use the macro `psh_str` than specifying the length of the
    ///       string, which is silly.
    ///
    /// Example:
    /// ```cpp
    /// auto my_str = psh_str("hey this is a compile time array of constant characters");
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

        constexpr StringView() noexcept = default;
        constexpr StringView(strptr _str, usize _length) noexcept
            : data{_str, _length} {}
        StringView(strptr _str) noexcept;
    };

    /// Dynamically allocated string.
    struct String {
        DynArray<char> data;

        String() = default;
        String(Arena* arena, usize capacity) noexcept;
        String(Arena* arena, StringView sv) noexcept;
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

// -----------------------------------------------------------------------------
// - Compile-time string type construction macros -
// -----------------------------------------------------------------------------

/// Create a string literal and a string view at compile time from a given c-string literal.
///
/// Note: Use this macro with care, you should only use it with literal strings. Otherwise the
///       length of the string won't be computed corrected by the compiler and you may obtain the
///       size of a pointer instead the length of the string.
#define psh_str(cstr_literal)          \
    psh::Str<sizeof((cstr_literal))> { \
        (cstr_literal)                 \
    }
#define psh_string_view(cstr_literal)              \
    psh::StringView {                              \
        (cstr_literal), sizeof((cstr_literal)) - 1 \
    }
