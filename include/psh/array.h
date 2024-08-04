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
/// Description: Fixed size array types whose size is only known at runtime, analogous to a VLA, but
///              has its memory bound to a parent arena allocator.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/arena.h>
#include <psh/core.h>
#include <psh/mem_utils.h>
#include <psh/type_utils.h>
#include <initializer_list>

namespace psh {
    /// Array with run-time known constant capacity.
    ///
    /// The array lifetime is bound to the lifetime of the arena passed at initialization, being
    /// responsible to allocate the memory referenced by the array.
    template <typename T>
        requires IsObject<T>
    struct Array {
        T*    buf  = nullptr;
        usize size = 0;

        // -----------------------------------------------------------------------------
        // - Constructors and initializers -
        // -----------------------------------------------------------------------------

        Array() noexcept = default;

        /// Initialize the array with a given size.
        void init(Arena* _arena, usize _size) noexcept {
            this->size = _size;
            if (psh_likely(this->size != 0)) {
                psh_assert_msg(
                    _arena != nullptr,
                    "Array::init called with non-zero size but null arena");

                this->buf = _arena->zero_alloc<T>(this->size);
                psh_assert_msg(this->buf != nullptr, "Array::init unable to allocate enough memory");
            }
        }

        /// Construct an array with a given size.
        Array(Arena* _arena, usize _size) noexcept {
            this->init(_arena, _size);
        }

        /// Initialize the array with a list of elements.
        ///
        /// Copies the contents of the initializer list into the array's memory, so its lifetime is
        /// not bound do the initializer list.
        void init(std::initializer_list<T> list, Arena* _arena) noexcept {
            this->size = list.size;
            if (psh_likely(this->size != 0)) {
                psh_assert_msg(
                    _arena != nullptr,
                    "Array::init called with non-zero size but null arena");

                this->buf = _arena->alloc<T>(this->size);
                psh_assert_msg(this->buf != nullptr, "Array unable to allocate enough memory");
            }

            // Copy initializer list content.
            std::memcpy(
                reinterpret_cast<u8*>(buf),
                reinterpret_cast<u8 const*>(list.begin()),
                sizeof(T) * list.size);
        }

        /// Construct an array with the contents of an initializer list.
        Array(std::initializer_list<T> list, Arena* _arena) noexcept {
            this->init(list, _arena);
        }

        /// Initialize the array with the elements of a fat pointer.
        ///
        /// Copies the contents of the fat pointer into the array's memory, so its lifetime is not
        /// bound to fat pointer.
        void init(FatPtr<T> const& fptr, Arena* _arena) noexcept {
            this->size = fptr.size;
            if (psh_likely(this->size != 0)) {
                psh_assert_msg(
                    _arena != nullptr,
                    "Array::init called with non-zero size but null arena");

                this->buf = _arena->alloc<T>(this->size);
                psh_assert_msg(this->buf != nullptr, "Array unable to allocate enough memory");
            }

            // Copy buffer content.
            std::memcpy(
                reinterpret_cast<u8*>(this->buf),
                reinterpret_cast<u8 const*>(fptr.buf),
                fptr.size_bytes());
        }

        /// Construct an array with the contents of a fat pointer.
        Array(FatPtr<T> const& fptr, Arena* _arena) noexcept {
            this->init(fptr, _arena);
        }

        // -----------------------------------------------------------------------------
        // - Size related utilities -
        // -----------------------------------------------------------------------------

        bool is_empty() const noexcept {
            return (this->size == 0);
        }

        usize size_bytes() const noexcept {
            return sizeof(T) * this->size;
        }

        // -----------------------------------------------------------------------------
        // - Iterator utilities -
        // -----------------------------------------------------------------------------

        T* begin() noexcept {
            return this->buf;
        }

        T const* begin() const noexcept {
            return static_cast<T const*>(this->buf);
        }

        T* end() noexcept {
            return psh_ptr_add(this->buf, this->size);
        }

        T const* end() const noexcept {
            return psh_ptr_add(static_cast<T const*>(this->buf), this->size);
        }

        // -----------------------------------------------------------------------------
        // - Indexed reads -
        // -----------------------------------------------------------------------------

        T& operator[](usize index) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(index < this->size, "Array::operator[] index out of bounds");
#endif
            return this->buf[index];
        }

        T const& operator[](usize index) const noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(index < this->size, "Array::operator[] index out of bounds");
#endif
            return this->buf[index];
        }
    };

    // -----------------------------------------------------------------------------
    // - Generating fat pointers -
    // -----------------------------------------------------------------------------

    template <typename T>
    inline FatPtr<T> fat_ptr(Array<T>& a) noexcept {
        return FatPtr{a.buf, a.size};
    }

    template <typename T>
    inline FatPtr<T const> const_fat_ptr(Array<T> const& a) noexcept {
        return FatPtr{reinterpret_cast<T const*>(a.buf), a.size};
    }
}  // namespace psh
