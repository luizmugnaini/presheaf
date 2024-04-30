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
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/concepts.h>
#include <psh/fat_ptr.h>
#include <psh/types.h>

namespace psh {
    /// Buffer with a compile-time known size.
    template <typename T, usize size_>
        requires IsObject<T>
    struct Buffer {
        T buf_[size_]{};

        [[nodiscard]] constexpr T* buf() noexcept {
            return static_cast<T*>(buf_);
        }

        [[nodiscard]] constexpr T const* const_buf() const noexcept {
            return static_cast<T const*>(buf_);
        }

        [[nodiscard]] constexpr usize size() const noexcept {
            return size_;
        }

        [[nodiscard]] usize size_bytes() const noexcept {
            return sizeof(T) * size_;
        }

        [[nodiscard]] FatPtr<T> as_fat_ptr() noexcept {
            return FatPtr{buf_, size_};
        }

        [[nodiscard]] constexpr FatPtr<T const> as_const_fat_ptr() const noexcept {
            return FatPtr{static_cast<T const*>(buf_), size_};
        }

        [[nodiscard]] constexpr T* begin() noexcept {
            return static_cast<T*>(buf_);
        }

        [[nodiscard]] constexpr T const* begin() const noexcept {
            return static_cast<T const*>(buf_);
        }

        [[nodiscard]] constexpr T* end() noexcept {
            return static_cast<T*>(buf_) + size_;
        }

        [[nodiscard]] constexpr T const* end() const noexcept {
            return static_cast<T const*>(buf_) + size_;
        }

        [[nodiscard]] constexpr T& operator[](usize idx) noexcept {
            return buf_[idx];
        }

        [[nodiscard]] constexpr T const& operator[](usize idx) const noexcept {
            return buf_[idx];
        }
    };
}  // namespace psh
