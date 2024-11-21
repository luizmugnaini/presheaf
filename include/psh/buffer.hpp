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
/// Description: Buffer with compile-time known size.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/core.hpp>
#include <psh/fat_ptr.hpp>

namespace psh {
    /// Buffer with a compile-time known size.
    template <typename T, usize count_>
    struct psh_api Buffer {
        T buf[count_] = {};

        constexpr usize count() const noexcept {
            return count_;
        }

        constexpr usize size_bytes() const noexcept {
            return sizeof(T) * count_;
        }

        constexpr T& operator[](usize idx) noexcept {
#if defined(PSH_CHECK_BOUNDS)
            psh_assert_fmt(idx < count_, "Index %zu out of bounds for Buffer of size %zu.", idx, count_);
#endif
            return this->buf[idx];
        }

        constexpr T const& operator[](usize idx) const noexcept {
#if defined(PSH_CHECK_BOUNDS)
            psh_assert_fmt(idx < count_, "Index %zu out of bounds for Buffer of size %zu.", idx, count_);
#endif
            return this->buf[idx];
        }

        constexpr T*       begin() noexcept { return this->buf; }
        constexpr T*       end() noexcept { return this->buf + count_; }
        constexpr T const* begin() const noexcept { return static_cast<T const*>(this->buf); }
        constexpr T const* end() const noexcept { return static_cast<T const*>(this->buf) + count_; }
    };

    // -----------------------------------------------------------------------------
    // Generating fat pointers.
    // -----------------------------------------------------------------------------

    template <typename T, usize count>
    constexpr FatPtr<T> make_fat_ptr(Buffer<T, count>& b) noexcept {
        return FatPtr<T>{b.buf, count};
    }

    template <typename T, usize count>
    constexpr FatPtr<T const> make_const_fat_ptr(Buffer<T, count> const& b) noexcept {
        return FatPtr<T const>{reinterpret_cast<T const*>(b.buf), count};
    }
}  // namespace psh
