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
/// Description: Fat pointer type.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/assert.h>
#include <psh/types.h>

namespace psh {
    /// Fat pointer, holds a pointer to a buffer and its corresponding size.
    template <typename T>
    struct FatPtr {
        T*    buf_  = nullptr;
        usize size_ = 0;

        constexpr T* buf() noexcept {
            return buf_;
        }

        constexpr T const* const_buf() const noexcept {
            return buf_;
        }

        constexpr usize size() const noexcept {
            return size_;
        }

        constexpr usize size_bytes() const noexcept {
            return sizeof(T) * size_;
        }

        constexpr T* begin() noexcept {
            return buf_;
        }

        constexpr T const* begin() const noexcept {
            return static_cast<T const*>(buf_);
        }

        constexpr T* end() noexcept {
            return (buf_ == nullptr) ? nullptr : buf_ + size_;
        }

        constexpr T const* end() const noexcept {
            return (buf_ == nullptr) ? nullptr : static_cast<T const*>(buf_ + size_);
        }

        constexpr T& operator[](usize idx) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(idx < size_, "Index out of bounds for fat pointer");
#endif
            return buf_[idx];
        }

        constexpr T const& operator[](usize idx) const noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(idx < size_, "Index out of bounds for fat pointer");
#endif
            return buf_[idx];
        }
    };

    template <typename T>
    FatPtr<u8> fat_ptr_as_bytes(T* buf, usize length) noexcept {
        return FatPtr{reinterpret_cast<u8*>(buf), sizeof(T) * length};
    }

    template <typename T>
    FatPtr<u8 const> fat_ptr_as_bytes(T const* buf, usize length) noexcept {
        return FatPtr{reinterpret_cast<u8 const*>(buf), sizeof(T) * length};
    }
}  // namespace psh
