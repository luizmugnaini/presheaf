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
        usize size = 0;
        T*    buf  = nullptr;

        explicit constexpr Array() noexcept = default;

        /// Initialize the array with a given size.
        void init(Arena* _arena, usize _size) noexcept {
            size = _size;
            if (psh_likely(size != 0)) {
                psh_assert_msg(
                    _arena != nullptr,
                    "Array::init called with non-zero size but null arena");

                buf = _arena->zero_alloc<T>(_size);
                psh_assert_msg(buf != nullptr, "Array::init unable to allocate enough memory");
            }
        }

        /// Construct an array with a given size.
        explicit Array(Arena* _arena, usize _size) noexcept {
            this->init(_arena, _size);
        }

        /// Initialize the array with the contents of an initializer list.
        void init(std::initializer_list<T> list, Arena* _arena) noexcept {
            size = list.size;
            if (psh_likely(size != 0)) {
                psh_assert_msg(
                    _arena != nullptr,
                    "Array::init called with non-zero size but null arena");

                buf = _arena->alloc<T>(size);
                psh_assert_msg(buf != nullptr, "Array unable to allocate enough memory");
            }

            // Copy initializer list content.
            memory_copy(
                reinterpret_cast<u8*>(buf),
                reinterpret_cast<u8 const*>(list.begin()),
                sizeof(T) * list.size);
        }

        /// Construct an array with the contents of an initializer list.
        explicit Array(std::initializer_list<T> list, Arena* _arena) noexcept {
            this->init(list, _arena);
        }

        /// Initialize the array with the contents of a fat pointer.
        void init(FatPtr<T> const& fptr, Arena* _arena) noexcept {
            size = fptr.size;
            if (psh_likely(size != 0)) {
                psh_assert_msg(
                    _arena != nullptr,
                    "Array::init called with non-zero size but null arena");

                buf = _arena->alloc<T>(fptr.size);
                psh_assert_msg(buf != nullptr, "Array unable to allocate enough memory");
            }

            // Copy buffer content.
            memory_copy(
                reinterpret_cast<u8*>(buf),
                reinterpret_cast<u8 const*>(fptr.buf),
                fptr.size_bytes());
        }

        /// Construct an array with the contents of a fat pointer.
        explicit Array(FatPtr<T> const& fptr, Arena* _arena) noexcept {
            this->init(fptr, _arena);
        }

        constexpr bool is_empty() const noexcept {
            return (size == 0);
        }

        constexpr usize size_bytes() const noexcept {
            return sizeof(T) * size;
        }

        constexpr FatPtr<T> fat_ptr() noexcept {
            return FatPtr{buf, size};
        }

        constexpr FatPtr<T const> const_fat_ptr() const noexcept {
            return FatPtr{static_cast<T const*>(buf), size};
        }

        void fill(T _fill) noexcept
            requires TriviallyCopyable<T>
        {
            psh::fill(FatPtr{buf, size}, _fill);
        }

        constexpr T* begin() noexcept {
            return buf;
        }

        constexpr T const* begin() const noexcept {
            return static_cast<T const*>(buf);
        }

        constexpr T* end() noexcept {
            return ptr_add(buf, size);
        }

        constexpr T const* end() const noexcept {
            return ptr_add(static_cast<T const*>(buf), size);
        }

        constexpr T& operator[](usize index) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(index < size, "Array::operator[] index out of bounds");
#endif
            return buf[index];
        }

        constexpr T const& operator[](usize index) const noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(index < size, "Array::operator[] index out of bounds");
#endif
            return buf[index];
        }
    };
}  // namespace psh
