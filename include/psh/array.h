/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
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
/// Description: Fixed size array types whose size is only known at runtime, also known as variable
///              length array (VLA).
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/arena.h>
#include <psh/concepts.h>
#include <psh/mem_utils.h>
#include <psh/types.h>

#include <initializer_list>

namespace psh {
    /// Array with run-time known constant capacity.
    template <typename T>
        requires IsObject<T>
    struct Array {
        usize size_ = 0;
        T*    buf_  = nullptr;

        explicit constexpr Array() noexcept = default;

        /// Initialize the array with a given size.
        void init(Arena* _arena, usize _size) noexcept {
            size_ = _size;
            if (size_ != 0) {
                psh_assert_msg(
                    _arena != nullptr,
                    "Array::init called with non-zero size but null arena");

                buf_ = _arena->zero_alloc<T>(_size);
                psh_assert_msg(buf_ != nullptr, "Array::init unable to allocate enough memory");
            }
        }

        /// Construct an array with a given size.
        explicit Array(Arena* _arena, usize _size) noexcept {
            this->init(_arena, _size);
        }

        /// Initialize the array with the contents of an initializer list.
        void init(std::initializer_list<T> list, Arena* _arena) noexcept {
            size_ = list.size();
            if (size_ != 0) {
                psh_assert_msg(
                    _arena != nullptr,
                    "Array::init called with non-zero size but null arena");

                buf_ = _arena->alloc<T>(size_);
                psh_assert_msg(buf_ != nullptr, "Array unable to allocate enough memory");
            }

            // Copy initializer list content.
            memory_copy(
                reinterpret_cast<u8*>(buf_),
                reinterpret_cast<u8 const*>(list.begin()),
                sizeof(T) * list.size());
        }

        /// Construct an array with the contents of an initializer list.
        explicit Array(std::initializer_list<T> list, Arena* _arena) noexcept {
            this->init(list, _arena);
        }

        /// Initialize the array with the contents of a fat pointer.
        void init(FatPtr<T> const& fptr, Arena* _arena) noexcept {
            size_ = fptr.size;
            if (size_ != 0) {
                psh_assert_msg(
                    _arena != nullptr,
                    "Array::init called with non-zero size but null arena");

                buf_ = _arena->alloc<T>(fptr.size);
                psh_assert_msg(buf_ != nullptr, "Array unable to allocate enough memory");
            }

            // Copy buffer content.
            memory_copy(
                reinterpret_cast<u8*>(buf_),
                reinterpret_cast<u8 const*>(fptr.buf),
                fptr.size_bytes());
        }

        /// Construct an array with the contents of a fat pointer.
        explicit Array(FatPtr<T> const& fptr, Arena* _arena) noexcept {
            this->init(fptr, _arena);
        }

        [[nodiscard]] constexpr T* buf() noexcept {
            return buf_;
        }

        [[nodiscard]] constexpr T const* const_buf() const noexcept {
            return buf_;
        }

        [[nodiscard]] constexpr usize size() const noexcept {
            return size_;
        }

        [[nodiscard]] constexpr bool is_empty() const noexcept {
            return (size_ == 0);
        }

        [[nodiscard]] constexpr usize size_bytes() const noexcept {
            return sizeof(T) * size_;
        }

        [[nodiscard]] constexpr FatPtr<T> as_fat_ptr() noexcept {
            return FatPtr{buf_, size_};
        }

        [[nodiscard]] constexpr FatPtr<T const> as_const_fat_ptr() const noexcept {
            return FatPtr{static_cast<T const*>(buf_), size_};
        }

        void fill(T _fill) noexcept
            requires TriviallyCopyable<T>
        {
            psh::fill(this->as_fat_ptr(), _fill);
        }

        [[nodiscard]] constexpr T* begin() noexcept {
            return buf_;
        }

        [[nodiscard]] constexpr T const* begin() const noexcept {
            return static_cast<T const*>(buf_);
        }

        [[nodiscard]] constexpr T* end() noexcept {
            return ptr_add(buf_, size_);
        }

        [[nodiscard]] constexpr T const* end() const noexcept {
            return ptr_add(static_cast<T const*>(buf_), size_);
        }

        [[nodiscard]] constexpr T& operator[](usize index) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(index < size_, "Array::operator[] index out of bounds");
#endif
            return buf_[index];
        }

        [[nodiscard]] constexpr T const& operator[](usize index) const noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(index < size_, "Array::operator[] index out of bounds");
#endif
            return buf_[index];
        }
    };
}  // namespace psh
