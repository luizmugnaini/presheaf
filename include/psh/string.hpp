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
#include <psh/memory.hpp>

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
        .count = str.count,                         \
    }

namespace psh {
    // Forward declaration.
    struct Arena;

    // -------------------------------------------------------------------------------------------------
    // String types.
    // -------------------------------------------------------------------------------------------------

    // Computes the length of a zero-terminated string.
    psh_api usize cstring_length(cstring str) psh_no_except;

    /// A string with guaranteed compile-time known size.
    ///
    /// Note: It is way more ergonomic to use the macro psh_make_str than specifying the length of the
    ///       string, which is silly.
    ///
    /// Example:
    ///
    /// auto my_str = psh_make_str("hey this is a compile time array of constant characters");
    ///
    template <usize count_>
    struct psh_api Str {
        using ValueType = char const;

        char const buf[count_];

        static constexpr usize count = count_ - 1;

        char operator[](usize index) const psh_no_except {
            psh_assert_bounds_check(index, count_);
            return this->buf[index];
        }
    };

    /// Dynamically sized string.
    using String = DynArray<char>;

    /// Immutable view of a string.
    using StringView = FatPtr<char const>;

    template <usize count>
    psh_api psh_inline constexpr StringView make_string_view(Str<count> str) psh_no_except {
        return StringView{str.buf, str.count};
    }

    psh_api psh_inline StringView make_string_view(cstring str) psh_no_except {
        return StringView{str, cstring_length(str)};
    }

    template <usize STR_LENGTH>
    psh_api psh_inline StringView make_string_view(char (&str)[STR_LENGTH]) psh_no_except {
        return StringView{str, STR_LENGTH - 1};
    }

    psh_api psh_inline String make_string(Arena* arena, usize initial_capacity) psh_no_except {
        return make_dynarray<char>(arena, initial_capacity);
    }

    psh_api psh_inline String make_string(Arena* arena, StringView sv) psh_no_except {
        String string;
        dynarray_init(&string, arena, sv.count + 1u);
        string.count = sv.count;

        memory_copy(reinterpret_cast<u8*>(string.buf), reinterpret_cast<u8 const*>(sv.buf), sizeof(char) * sv.count);

        return string;
    }

    psh_api psh_inline StringView make_string_view(String const& string) psh_no_except {
        return StringView{reinterpret_cast<char const*>(string.buf), string.count};
    }

    /// Join an array of string views to a target string data. You can also provide a join element to be
    /// attached to the end of each join.
    ///
    /// Example:
    ///
    /// Arena arena{...};
    /// String s = make_string(&arena, "Hello");
    /// Buffer<StringView, 3> words = {"World", "Earth", "Terra"};
    ///
    /// assert(s.join(psh::make_const_fat_ptr(words), ", "));
    /// assert(string_equal(s.data.buf, "Hello, World, Earth, Terra"));
    ///
    psh_api Status join_strings(String& target, FatPtr<StringView const> join_strings, StringView join_element = {}) psh_no_except;

    // -------------------------------------------------------------------------------------------------
    // String comparison.
    // -------------------------------------------------------------------------------------------------

    enum struct StringCompareResult {
        LESS_THAN,
        EQUAL,
        GREATER_THAN,
    };

    psh_api StringCompareResult string_compare(StringView lhs, StringView rhs) psh_no_except;
    template <usize RHS_LENGTH>
    psh_api psh_inline StringCompareResult string_compare(StringView lhs, char (&rhs)[RHS_LENGTH]) psh_no_except {
        return string_compare(lhs, StringView{rhs, RHS_LENGTH - 1u});
    }
    template <usize RHS_LENGTH>
    psh_api psh_inline StringCompareResult string_compare(StringView lhs, char rhs[RHS_LENGTH]) psh_no_except {
        return string_compare(lhs, StringView{rhs, RHS_LENGTH - 1u});
    }

    psh_api bool string_equal(StringView lhs, StringView rhs) psh_no_except;
    template <usize RHS_LENGTH>
    psh_api psh_inline bool string_equal(StringView lhs, char const (&rhs)[RHS_LENGTH]) psh_no_except {
        usize len = RHS_LENGTH;
        psh_discard_value(len);
        return string_equal(lhs, StringView{rhs, RHS_LENGTH - 1u});
    }
    template <usize RHS_LENGTH>
    psh_api psh_inline bool string_equal(StringView lhs, char const rhs[RHS_LENGTH]) psh_no_except {
        return string_equal(lhs, StringView{rhs, RHS_LENGTH - 1u});
    }

    // -------------------------------------------------------------------------------------------------
    // Character properties.
    // -------------------------------------------------------------------------------------------------

    psh_api constexpr bool is_utf8(char c) psh_no_except {
        return (0x1F < c && c < 0x7F);
    }

}  // namespace psh
