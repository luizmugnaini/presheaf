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

namespace psh {
    // Forward declaration.
    struct Arena;

    // -------------------------------------------------------------------------------------------------
    // Character properties.
    // -------------------------------------------------------------------------------------------------

    psh_api psh_inline bool char_is_utf8(char c) psh_no_except {
        return ((0x1F < c) && (c < 0x7F));
    }

    psh_api psh_inline bool char_is_blank(char c) psh_no_except {
        return (c == ' ')
               || (c == '\t')
               || (c == '\f')
               || (c == '\v');
    }

    psh_api psh_inline bool char_is_end_of_line(char c) psh_no_except {
        return (c == '\n') || (c == '\r');
    }

    psh_api psh_inline bool char_is_digit(char c) psh_no_except {
        return ('0' <= c) && (c <= '9');
    }

    psh_api psh_inline bool char_is_alphabetic(char c) psh_no_except {
        return (('A' <= c) && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
    }

    psh_api psh_inline bool char_is_alphanumeric(char c) psh_no_except {
        return (char_is_alphabetic(c) || char_is_digit(c));
    }

    psh_api psh_inline char char_to_lower(char c) psh_no_except {
        return (('A' <= c) && (c <= 'Z')) ? ('a' + (c - 'A')) : c;
    }

    psh_api psh_inline char char_to_upper(char c) psh_no_except {
        return (('A' <= c) && (c <= 'Z')) ? ('A' + (c - 'a')) : c;
    }

    psh_api psh_inline i32 char_to_digit(char c) psh_no_except {
        psh_paranoid_validate_usage({
            psh_assert_fmt(char_is_digit(c), "Expected character (%c) to be a digit (between '0' and '9').", c);
        });
        return static_cast<i32>(c - '0');
    }

    psh_api psh_inline char digit_to_char(i32 value) psh_no_except {
        psh_paranoid_validate_usage({
            psh_assert_fmt((0 <= value) && (value <= 9), "Expected value (%d) to be a digit (between 0 and 9).", value);
        });
        return '0' + static_cast<char>(value);
    }

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
            psh_validate_usage(psh_assert_bounds_check(index, count_));
            return this->buf[index];
        }
    };

    /// Dynamically sized string.
    using String = DynamicArray<char>;

    /// Immutable view of a string.
    using StringView = FatPtr<char const>;

    template <usize count>
    psh_api constexpr StringView make_string_view(Str<count> str) psh_no_except {
        return StringView{str.buf, str.count};
    }

    template <usize STR_LENGTH>
    psh_api constexpr StringView make_string_view(char (&str)[STR_LENGTH]) psh_no_except {
        return StringView{str, STR_LENGTH - 1};
    }

    psh_api psh_inline StringView make_string_view(cstring str) psh_no_except {
        return StringView{str, cstring_length(str)};
    }

    psh_api psh_inline String make_string(Arena* arena, usize initial_capacity) psh_no_except {
        return make_dynamic_array<char>(arena, initial_capacity);
    }

    psh_api psh_inline String make_string(Arena* arena, StringView sv) psh_no_except {
        String string;
        dynamic_array_init(&string, arena, sv.count + 1u);
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
    psh_api Status join_strings(
        String&                  target,
        FatPtr<StringView const> join_strings,
        StringView               join_element = {}) psh_no_except;

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
    // String formatting.
    //
    // @TODO: improve the integration with the Presheaf types.
    // @TODO: implement functions by extracting stuff from stb_sprintf.
    // -------------------------------------------------------------------------------------------------

    using StringFormatCallbackFunction = char*(cstring* buf, void* user, i32 len);

    i32 string_format_list(char* buf, i32 count, cstring fmt, va_list va) psh_no_except;
    i32 string_format_list_with_callback(
        StringFormatCallbackFunction* callback,
        void*                         user,
        char*                         buf,
        cstring                       fmt,
        va_list                       va) psh_no_except;

    psh_attribute_fmt(3) psh_inline i32 string_format(char* buf, i32 count, cstring fmt, ...) psh_no_except {
        i32 result;
        {
            va_list args;
            va_start(args, fmt);
            result = string_format_list(buf, count, fmt, args);
            va_end(args);
        }
        return result;
    }
}  // namespace psh
