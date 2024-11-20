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

#include <psh/assert.hpp>
#include <psh/core.hpp>

namespace psh {
    /// Fat pointer, holds a pointer to a buffer and its corresponding size.
    template <typename T>
    struct psh_api FatPtr {
        T*    buf;
        usize count = 0;

        psh_inline FatPtr<T> slice(usize start, usize slice_count) noexcept {
            psh_assert_fmt(slice_count <= this->count, "Slice element count (%zu) surpasses the FatPtr count (%zu).", slice_count, this->count);
            return FatPtr<T>{psh_ptr_add(this->buf, start), slice_count};
        }

        psh_inline usize size_bytes() const noexcept {
            return sizeof(T) * this->count;
        }

        // -----------------------------------------------------------------------------
        // Iterator utilities.
        // -----------------------------------------------------------------------------

        psh_inline T* begin() noexcept {
            return this->buf;
        }

        psh_inline T const* begin() const noexcept {
            return static_cast<T const*>(this->buf);
        }

        psh_inline T* end() noexcept {
            return psh_ptr_add(this->buf, this->count);
        }

        psh_inline T const* end() const noexcept {
            return static_cast<T const*>(psh_ptr_add(this->buf, this->count));
        }

        // -----------------------------------------------------------------------------
        // Indexed reads.
        // -----------------------------------------------------------------------------

        psh_inline T& operator[](usize idx) noexcept {
#if defined(PSH_CHECK_BOUNDS)
            psh_assert_fmt(idx < this->count, "Index %zu out of bounds for FatPtr with size %zu.", idx, this->count);
#endif
            return this->buf[idx];
        }

        psh_inline T const& operator[](usize idx) const noexcept {
#if defined(PSH_CHECK_BOUNDS)
            psh_assert_fmt(idx < this->count, "Index %zu out of bounds for FatPtr with size %zu.", idx, this->count);
#endif
            return this->buf[idx];
        }
    };

    // -----------------------------------------------------------------------------
    // Fat pointer creation for common usage patterns.
    // -----------------------------------------------------------------------------

    template <typename T>
    psh_inline FatPtr<u8> make_fat_ptr_as_bytes(T* buf, usize count) noexcept {
        return FatPtr<u8>{reinterpret_cast<u8*>(buf), sizeof(T) * count};
    }

    template <typename T>
    psh_inline FatPtr<u8 const> make_const_fat_ptr_as_bytes(T const* buf, usize count) noexcept {
        return FatPtr<u8 const>{reinterpret_cast<u8 const*>(buf), sizeof(T) * count};
    }

    template <typename T>
    psh_inline FatPtr<T> make_fat_ptr(T* ptr) noexcept {
        return FatPtr<T>{ptr, 1};
    }

    template <typename T>
    psh_inline FatPtr<T const> make_const_fat_ptr(T const* ptr) noexcept {
        return FatPtr<T const>{ptr, 1};
    }
}  // namespace psh
