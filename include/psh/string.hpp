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
/// Description: String related types and utility functions.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/core.hpp>
#include <psh/dynarray.hpp>

// -------------------------------------------------------------------------------------------------
// Compile-time string type construction macros.
// -------------------------------------------------------------------------------------------------

/// Create a string literal and a string view at compile time from a given c-string literal.
///
/// Note: Use this macro with care, you should only use it with literal strings. Otherwise the
///       length of the string won't be computed corrected by the compiler and you may obtain the
///       size of a pointer instead the length of the string.
#define psh_comptime_make_str(cstr_literal) \
    psh::Str<sizeof((cstr_literal))> {      \
        (cstr_literal)                      \
    }
#define psh_comptime_make_string_view(cstr_literal) \
    psh::StringView {                               \
        .buf   = cstr_literal,                      \
        .count = sizeof(cstr_literal) - 1u,         \
    }
#define psh_comptime_make_string_view_from_str(str) \
    psh::StringView {                               \
        .buf   = str.buf,                           \
        .count = str.count(),                       \
    }

namespace psh {
    // Forward declaration.
    struct Arena;

    // -------------------------------------------------------------------------------------------------
    // String types.
    // -------------------------------------------------------------------------------------------------

    // Computes the length of a zero-terminated string.
    psh_api usize str_length(strptr str) psh_no_except;

    /// String literal type.
    struct psh_api StringLiteral {
        strptr str;

        template <usize N>
        consteval StringLiteral(char const (&str_)[N]) psh_no_except
            : str{str_} {}
    };

    /// A string with guaranteed compile-time known size.
    ///
    /// Note: It is way more ergonomic to use the macro psh_make_str than specifying the length of the
    ///       string, which is silly.
    ///
    /// Example:
    ///
    /// auto my_str = psh_make_str("hey this is a compile time array of constant characters");
    ///
    template <usize size_>
    struct psh_api Str {
        char const buf[size_];

        /// The character count of the string, disregarding its null-terminator.
        constexpr usize count() const psh_no_except {
            return size_ - 1u;
        }

        char operator[](usize index) const psh_no_except {
            psh_assert_bounds_check(index, size_, "Access out of bounds (%zu) for string of size %zu", index, size_);
            return this->buf[index];
        }
    };

    /// Immutable view of a string.
    using StringView = FatPtr<char const>;

    /// Dynamically sized string.
    using String = DynArray<char>;

    template <usize size_>
    psh_api psh_inline constexpr StringView make_string_view(Str<size_> str) psh_no_except {
        return {str.buf, size_ - 1u};
    }

    psh_api psh_inline StringView make_string_view(strptr str) psh_no_except {
        return {str, str_length(str)};
    }

    psh_api psh_inline String make_string(Arena* arena, StringView sv) psh_no_except {
        String string{arena, sv.count + 1u};

        memory_copy(reinterpret_cast<u8*>(string.buf), reinterpret_cast<u8 const*>(sv.buf), sizeof(char) * sv.count);
        string.count = sv.count;

        return string;
    }

    psh_api psh_inline StringView make_string_view(String const& string) psh_no_except {
        return make_const_fat_ptr(string);
    }

    /// Join an array of string views to a target string data. You can also provide a join element to be
    /// attached to the end of each join.
    ///
    /// Example:
    ///
    /// psh::Arena arena{...};
    /// psh::String s = make_string(&arena, "Hello");
    /// psh::Buffer<StringView, 3> words = {"World", "Earth", "Terra"};
    ///
    /// assert(s.join(psh::make_const_fat_ptr(words), ", "));
    /// assert(psh::str_cmp(s.data.buf, "Hello, World, Earth, Terra") == psh::StrCmpResult::EQUAL);
    ///
    /// Although this example uses initializer lists, you can also achieve the same using the
    /// implementation based on psh::FatPtr.
    psh_api Status join_strings(String& target, FatPtr<StringView const> join_strings, StringView join_element = {}) psh_no_except;

    // -------------------------------------------------------------------------------------------------
    // String comparison utilities.
    // -------------------------------------------------------------------------------------------------

    enum struct StrCmpResult {
        LESS_THAN,
        EQUAL,
        GREATER_THAN,
    };

    psh_api StrCmpResult            str_cmp(strptr lhs, strptr rhs) psh_no_except;
    psh_api StrCmpResult            str_cmp(StringView lhs, StringView rhs) psh_no_except;
    psh_api psh_inline StrCmpResult str_cmp(StringView lhs, strptr rhs) psh_no_except {
        return str_cmp(lhs, make_string_view(rhs));
    }

    psh_api bool            str_equal(strptr lhs, strptr rhs) psh_no_except;
    psh_api bool            str_equal(StringView lhs, StringView rhs) psh_no_except;
    psh_api psh_inline bool str_equal(StringView lhs, strptr rhs) psh_no_except {
        return str_equal(lhs, make_string_view(rhs));
    }

    psh_api constexpr bool is_utf8(char c) psh_no_except {
        return (0x1F < c && c < 0x7F);
    }

}  // namespace psh
