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
/// Description: Buffer with compile-time known size.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/core.h>
#include <psh/fat_ptr.h>
#include <psh/type_utils.h>

namespace psh {
    /// Buffer with a compile-time known size.
    template <typename T, usize size_>
        requires IsObject<T>
    struct Buffer {
        T buf[size_] = {};

        // -----------------------------------------------------------------------------
        // - Size related utilities -
        // -----------------------------------------------------------------------------

        constexpr usize size() const noexcept {
            return size_;
        }

        usize size_bytes() const noexcept {
            return sizeof(T) * size_;
        }

        // -----------------------------------------------------------------------------
        // - Iterator utilities -
        // -----------------------------------------------------------------------------

        constexpr T* begin() noexcept {
            return static_cast<T*>(this->buf);
        }

        constexpr T const* begin() const noexcept {
            return static_cast<T const*>(this->buf);
        }

        constexpr T* end() noexcept {
            return static_cast<T*>(this->buf) + size_;
        }

        constexpr T const* end() const noexcept {
            return static_cast<T const*>(this->buf) + size_;
        }

        // -----------------------------------------------------------------------------
        // - Indexed reads -
        // -----------------------------------------------------------------------------

        constexpr T& operator[](usize idx) noexcept {
            return this->buf[idx];
        }

        constexpr T const& operator[](usize idx) const noexcept {
            return this->buf[idx];
        }
    };

    // -----------------------------------------------------------------------------
    // - Generating fat pointers -
    // -----------------------------------------------------------------------------

    template <typename T, usize size>
    inline FatPtr<T> fat_ptr(Buffer<T, size>& b) noexcept {
        return FatPtr<T>{b.buf, size};
    }

    template <typename T, usize size>
    inline FatPtr<T const> const_fat_ptr(Buffer<T, size> const& b) noexcept {
        return FatPtr<T const>{reinterpret_cast<T const*>(b.buf), size};
    }
}  // namespace psh
