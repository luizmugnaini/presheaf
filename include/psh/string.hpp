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

#include <psh/arena.hpp>
#include <psh/core.hpp>
#include <psh/dyn_array.hpp>

namespace psh {
    // -----------------------------------------------------------------------------
    // String comparison utilities.
    // -----------------------------------------------------------------------------

    enum struct StrCmpResult {
        LESS_THAN,
        EQUAL,
        GREATER_THAN,
    };

    usize        str_length(strptr str) noexcept;
    StrCmpResult str_cmp(strptr lhs, strptr rhs) noexcept;
    bool         str_equal(strptr lhs, strptr rhs) noexcept;

    constexpr bool is_utf8(char c) noexcept {
        return (0x1F < c && c < 0x7F);
    }

    // -----------------------------------------------------------------------------
    // String types.
    // -----------------------------------------------------------------------------

    /// String literal type.
    struct StringLiteral {
        strptr str;

        template <usize N>
        consteval StringLiteral(char const (&str_)[N]) noexcept
            : str{str_} {}
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
#if defined(PSH_CHECK_BOUNDS)
            psh_assert_fmt(index < size_, "Access out of bounds (%zu) for string of size %zu", index, size_);
#endif
            return this->buf[index];
        }
    };

    /// Immutable view of a string.
    struct StringView {
        FatPtr<char const> data;

        constexpr StringView() noexcept = default;

        template <usize size_>
        constexpr StringView(Str<size_> str) noexcept
            : data{str.buf, size_ - 1} {}

        constexpr StringView(strptr str, usize size) noexcept
            : data{str, size} {}

        StringView(strptr str) noexcept
            : data{str, str_length(str)} {}
    };

    /// Dynamically allocated string.
    struct String {
        DynArray<char> data;

        constexpr String() = default;
        String(Arena* arena, usize size) noexcept;
        String(Arena* arena, StringView sv) noexcept;

        void init(Arena* arena, StringView sv) noexcept;

        /// Make a view from the string.
        StringView view() const noexcept;

        /// Join an array of string views to the current string data. You can also provide a string
        /// to be attached to the end of each join.
        ///
        /// Example:
        /// ```cpp
        /// psh::Arena arena{...};
        /// psh::String s{&arena, "Hello"};
        /// psh::Buffer<StringView, 3> words = {"World", "Earth", "Terra"};
        ///
        /// assert(s.join(psh::const_fat_ptr(words), ", "));
        /// assert(psh::str_cmp(s.data.buf, "Hello, World, Earth, Terra") == psh::StrCmpResult::EQUAL);
        /// ```
        /// Although this example uses initializer lists, you can also achieve the same using the
        /// implementation based on `psh::FatPtr`.
        Status join(FatPtr<StringView const> join_strings, StringView join = {}) noexcept;
    };

    psh_inline String::String(Arena* arena, usize size) noexcept {
        this->data.init(arena, size);
    }

    psh_inline String::String(Arena* arena, StringView sv) noexcept {
        this->init(arena, sv);
    }

    psh_inline StringView String::view() const noexcept {
        return StringView{this->data.buf, this->data.size};
    }
}  // namespace psh

// -----------------------------------------------------------------------------
// Compile-time string type construction macros.
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
#define psh_string_view(cstr_literal)             \
    psh::StringView {                             \
        (cstr_literal), sizeof(cstr_literal) - 1, \
    }
