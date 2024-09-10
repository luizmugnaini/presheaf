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

#include <psh/arena.h>
#include <psh/core.h>
#include <psh/dyn_array.h>

namespace psh {
    // -----------------------------------------------------------------------------
    // - String comparison utilities -
    // -----------------------------------------------------------------------------

    enum struct StrCmpResult { LESS_THAN,
                               EQUAL,
                               GREATER_THAN };

    usize        str_length(strptr str) noexcept;
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

        char operator[](usize index) const noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(index < size_, "Str access out of bounds");
#endif
            return this->buf[index];
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
        // using namespace psh;
        //
        // Arena arena{...};
        // String s{&arena, "Hello"};
        // Buffer<StringView, 3> words = {"World", "Earth", "Terra"};
        //
        // assert(s.join(const_fat_ptr(words), ", ") == Result::OK);
        // assert(strcmp(s.data.buf, "Hello, World, Earth, Terra") == 0);
        // ```
        // Although this example uses initializer lists, you can also achieve the same using the
        // implementation based on `psh::FatPtr`.
        Status join(FatPtr<StringView const> strs, strptr join_cstr = nullptr) noexcept;
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
