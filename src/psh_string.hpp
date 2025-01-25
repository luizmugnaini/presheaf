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

#include <stdarg.h>
#include "psh_core.hpp"
#include "psh_memory.hpp"

namespace psh {
    // Forward declaration.
    struct Arena;

    // -------------------------------------------------------------------------------------------------
    // Character properties.
    // -------------------------------------------------------------------------------------------------

    psh_proc psh_inline bool char_is_utf8(char c) psh_no_except {
        return ((0x1F < c) && (c < 0x7F));
    }

    psh_proc psh_inline bool char_is_blank(char c) psh_no_except {
        return (c == ' ')
               || (c == '\t')
               || (c == '\f')
               || (c == '\v')
               || (c == '\n')
               || (c == '\r');
    }

    psh_proc psh_inline bool char_is_end_of_line(char c) psh_no_except {
        return (c == '\n') || (c == '\r');
    }

    psh_proc psh_inline bool char_is_digit(char c) psh_no_except {
        return ('0' <= c) && (c <= '9');
    }

    psh_proc psh_inline bool char_is_alphabetic(char c) psh_no_except {
        return (('A' <= c) && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
    }

    psh_proc psh_inline bool char_is_alphanumeric(char c) psh_no_except {
        return (char_is_alphabetic(c) || char_is_digit(c));
    }

    psh_proc psh_inline char char_to_lower(char c) psh_no_except {
        return (('A' <= c) && (c <= 'Z')) ? ('a' + (c - 'A')) : c;
    }

    psh_proc psh_inline char char_to_upper(char c) psh_no_except {
        return (('A' <= c) && (c <= 'Z')) ? ('A' + (c - 'a')) : c;
    }

    psh_proc psh_inline i32 char_to_digit(char c) psh_no_except {
        psh_paranoid_validate_usage({
            psh_assert_fmt(char_is_digit(c), "Expected character (%c) to be a digit (between '0' and '9').", c);
        });
        return static_cast<i32>(c - '0');
    }

    psh_proc psh_inline char digit_to_char(i32 value) psh_no_except {
        psh_paranoid_validate_usage({
            psh_assert_fmt((0 <= value) && (value <= 9), "Expected value (%d) to be a digit (between 0 and 9).", value);
        });
        return '0' + static_cast<char>(value);
    }

    // -------------------------------------------------------------------------------------------------
    // String types.
    // -------------------------------------------------------------------------------------------------

    // Computes the length of a zero-terminated string.
    psh_proc usize cstring_length(cstring str) psh_no_except;

    /// Dynamically sized string.
    using DynamicString = DynamicArray<char>;

    /// Immutable string type with an associated length.
    struct String {
        cstring buf;
        usize   count = 0;

        using ValueType = char const;
        psh_inline char operator[](usize idx) const psh_no_except {
            psh_assert_bounds_check(idx, this->count);
            return this->buf[idx];
        }
        psh_inline char const* begin() const psh_no_except { return this->buf; }
        psh_inline char const* end() const psh_no_except { return this->buf + this->count; }
    };

/// Create a string with compile-time known length from a given c-string literal.
///
/// Note: Use this macro with care, you should only use it with literal strings. Otherwise the
///       length of the string won't be computed corrected by the compiler and you may obtain the
///       size of a pointer instead the length of the string.
#define psh_comptime_make_string(cstr_literal)    \
    psh::String {                                 \
        .buf   = cstr_literal,                    \
        .count = psh_usize_of(cstr_literal) - 1u, \
    }

    template <usize STR_LENGTH>
    psh_proc psh_inline constexpr String make_string(char (&str)[STR_LENGTH]) psh_no_except {
        return String{str, STR_LENGTH - 1};
    }

    psh_proc psh_inline String make_string(cstring str) psh_no_except {
        return String{str, cstring_length(str)};
    }

    psh_proc psh_inline String make_string(DynamicString& string) psh_no_except {
        return String{string.buf, string.count};
    }

    psh_proc psh_inline DynamicString make_dynamic_string(Arena* arena, usize initial_capacity) psh_no_except {
        return make_dynamic_array<char>(arena, initial_capacity);
    }

    psh_proc psh_inline DynamicString make_dynamic_string(Arena* arena, String sv) psh_no_except {
        DynamicString string;
        init_dynamic_array(&string, arena, sv.count + 1u);
        string.count = sv.count;

        memory_copy(reinterpret_cast<u8*>(string.buf), reinterpret_cast<u8 const*>(sv.buf), psh_usize_of(char) * sv.count);

        return string;
    }

    /// Join an array of string views to a target string data. You can also provide a join element to be
    /// attached to the end of each join.
    ///
    /// Example:
    ///
    /// Arena arena{...};
    /// DynamicString s = make_dynamic_string(&arena, "Hello");
    /// Buffer<String, 3> words = {"World", "Earth", "Terra"};
    ///
    /// assert(string_join(s, make_const_fat_ptr(words), ", ");
    /// assert(string_equal(s.data.buf, "Hello, World, Earth, Terra"));
    ///
    psh_proc Status join_strings(
        DynamicString&       target,
        FatPtr<String const> join_strings,
        String               join_element = {}) psh_no_except;

    // -------------------------------------------------------------------------------------------------
    // String comparison.
    // -------------------------------------------------------------------------------------------------

    enum struct StringCompareResult {
        LESS_THAN,
        EQUAL,
        GREATER_THAN,
    };

    psh_proc StringCompareResult string_compare(String lhs, String rhs) psh_no_except;
    template <usize RHS_LENGTH>
    psh_proc psh_inline StringCompareResult string_compare(String lhs, char (&rhs)[RHS_LENGTH]) psh_no_except {
        return string_compare(lhs, String{rhs, RHS_LENGTH - 1u});
    }
    template <usize RHS_LENGTH>
    psh_proc psh_inline StringCompareResult string_compare(String lhs, char rhs[RHS_LENGTH]) psh_no_except {
        return string_compare(lhs, String{rhs, RHS_LENGTH - 1u});
    }

    psh_proc bool string_equal(String lhs, String rhs) psh_no_except;
    template <usize RHS_LENGTH>
    psh_proc psh_inline bool string_equal(String lhs, char const (&rhs)[RHS_LENGTH]) psh_no_except {
        usize len = RHS_LENGTH;
        psh_discard_value(len);
        return string_equal(lhs, String{rhs, RHS_LENGTH - 1u});
    }
    template <usize RHS_LENGTH>
    psh_proc psh_inline bool string_equal(String lhs, char const rhs[RHS_LENGTH]) psh_no_except {
        return string_equal(lhs, String{rhs, RHS_LENGTH - 1u});
    }

    // -------------------------------------------------------------------------------------------------
    // String formatting.
    //
    // @TODO: improve the integration with the Presheaf types.
    // @TODO: implement functions by extracting stuff from stb_sprintf.
    // -------------------------------------------------------------------------------------------------

    /// Convert a va_list arg list into a buffer. This function always returns a zero-terminated
    /// string (unlike regular vsnprintf).
    psh_proc i32 string_format_list(char* buf, i32 count, cstring fmt, va_list va) psh_no_except;

    /// Convert an arg list into a buffer. This function always returns a zero-terminated string
    /// (unlike regular snprintf).
    psh_proc psh_attribute_fmt(3) i32 string_format(char* buf, i32 count, cstring fmt, ...) psh_no_except;
}  // namespace psh
