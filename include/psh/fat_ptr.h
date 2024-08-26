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
/// Description: Fat pointer type.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/assert.h>
#include <psh/core.h>
#include <psh/type_utils.h>

namespace psh {
    /// Fat pointer, holds a pointer to a buffer and its corresponding size.
    template <typename T>
    struct FatPtr {
        T*    buf  = nullptr;
        usize size = 0;

        FatPtr<T> slice(usize start_idx, usize end = 0) noexcept {
            psh_assert(end <= this->size);
            return FatPtr<T>{psh_ptr_add(this->buf, start_idx), end - start_idx};
        }

        // -----------------------------------------------------------------------------
        // - Size related utilities -
        // -----------------------------------------------------------------------------

        constexpr usize size_bytes() const noexcept {
            return sizeof(T) * this->size;
        }

        constexpr bool is_empty() const noexcept {
            return (this->size == 0);
        }

        // -----------------------------------------------------------------------------
        // - Iterator utilities -
        // -----------------------------------------------------------------------------

        constexpr T* begin() noexcept {
            return this->buf;
        }

        constexpr T const* begin() const noexcept {
            return static_cast<T const*>(this->buf);
        }

        constexpr T* end() noexcept {
            return (this->buf == nullptr) ? nullptr : (this->buf + this->size);
        }

        constexpr T const* end() const noexcept {
            return (this->buf == nullptr) ? nullptr : static_cast<T const*>(this->buf + this->size);
        }

        // -----------------------------------------------------------------------------
        // - Indexed reads -
        // -----------------------------------------------------------------------------

        constexpr T& operator[](usize idx) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(idx < this->size, "Index out of bounds for fat pointer");
#endif
            return this->buf[idx];
        }

        constexpr T const& operator[](usize idx) const noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(idx < this->size, "Index out of bounds for fat pointer");
#endif
            return this->buf[idx];
        }
    };

    // -----------------------------------------------------------------------------
    // - Fat pointer creation for common usage patterns -
    // -----------------------------------------------------------------------------

    template <typename T>
    FatPtr<u8> fat_ptr_as_bytes(T* buf, usize length) noexcept {
        return FatPtr<u8>{reinterpret_cast<u8*>(buf), sizeof(T) * length};
    }

    template <typename T>
    FatPtr<u8 const> fat_ptr_as_bytes(T const* buf, usize length) noexcept {
        return FatPtr<u8 const>{reinterpret_cast<u8 const*>(buf), sizeof(T) * length};
    }
}  // namespace psh
