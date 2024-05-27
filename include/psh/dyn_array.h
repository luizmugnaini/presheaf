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
/// Description: Dynamic array.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/arena.h>
#include <psh/mem_utils.h>
#include <psh/option.h>
#include <psh/types.h>

#include <initializer_list>

namespace psh {
    /// Resizing factor for the `DynArray::grow` method.
    static constexpr usize DYNARRAY_RESIZE_CAPACITY_FACTOR   = 2;
    /// The initial capacity given to a `DynArray` if none is specified.
    static constexpr usize DYNARRAY_DEFAULT_INITIAL_CAPACITY = 4;

    /// Dynamic array managed by an arena allocator.
    ///
    /// The dynamic array's buffer shouldn't be destroyed by the array itself, since it *never* owns
    /// the memory.
    template <typename T>
        requires IsObject<T>
    struct DynArray {
        Arena* arena    = nullptr;
        usize  size     = 0;
        usize  capacity = 0;
        T*     buf      = nullptr;

        explicit constexpr DynArray() noexcept = default;

        /// Initialize the dynamic array with a given capacity.
        void init(Arena* _arena, usize _capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY) noexcept {
            arena    = _arena;
            capacity = _capacity;

            if (psh_likely(capacity != 0)) {
                psh_assert_msg(
                    arena != nullptr,
                    "DynArray::init called with non-zero capacity but an empty arena");

                buf = arena->zero_alloc<T>(capacity);
                psh_assert_msg(
                    buf != nullptr,
                    "DynArray::init unable to acquire enough bytes of memory");
            }
        }

        /// Construct a dynamic array with a given capacity.
        explicit DynArray(
            Arena* _arena,
            usize  _capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY) noexcept {
            this->init(_arena, _capacity);
        }

        /// Initialize the dynamic array with the contents of an initializer list, and optionally
        /// reserve a given capacity.
        void init(
            std::initializer_list<T> const& list,
            Arena*                          _arena,
            Option<usize> const&            _capacity = {}) noexcept {
            arena    = _arena;
            size     = list.size();
            capacity = _capacity.val_or(DYNARRAY_RESIZE_CAPACITY_FACTOR * size);

            psh_assert_msg(
                size <= capacity,
                "DynArray::init called with inconsistent data: capacity less than the size");

            if (psh_likely(capacity != 0)) {
                psh_assert_msg(
                    arena != nullptr,
                    "DynArray::init called with non-zero capacity but an empty arena");

                buf = arena->alloc<T>(capacity);
                psh_assert_msg(buf != nullptr, "DynArray::init unable to allocate enough memory");
            }

            std::memcpy(
                reinterpret_cast<u8*>(buf),
                reinterpret_cast<u8 const*>(list.begin()),
                sizeof(T) * size);
        }

        /// Construct a dynamic array with the contents of an initializer list, and optionally
        /// reserve a given capacity.
        explicit DynArray(
            std::initializer_list<T> list,
            Arena*                   _arena,
            Option<usize>            _capacity = {}) noexcept {
            this->init(list, _arena, _capacity);
        }

        /// Initialize the dynamic array with the contents of a fat pointer, and optionally reserve
        /// a given capacity.
        void
        init(FatPtr<T const> fptr, Arena* _arena, Option<usize> const& _capacity = {}) noexcept {
            arena    = _arena;
            size     = fptr.size;
            capacity = _capacity.val_or(DYNARRAY_RESIZE_CAPACITY_FACTOR * size);

            psh_assert_msg(
                size <= capacity,
                "DynArray::init called with inconsistent data: capacity less than the size");

            if (psh_likely(capacity != 0)) {
                psh_assert_msg(
                    arena != nullptr,
                    "DynArray::init called with non-zero capacity but an empty arena");

                buf = arena->alloc<T>(capacity);
                psh_assert_msg(buf != nullptr, "DynArray::init unable to allocate enough memory");
            }

            std::memcpy(
                reinterpret_cast<u8*>(buf),
                reinterpret_cast<u8 const*>(fptr.begin()),
                fptr.size_bytes());
        }

        /// Construct a dynamic array with the contents of a fat pointer, and optionally reserve a
        /// given capacity.
        explicit DynArray(
            FatPtr<T const> fptr,
            Arena*          _arena,
            Option<usize>   _capacity = {}) noexcept {
            this->init(fptr, _arena, _capacity);
        }

        bool is_empty() const noexcept {
            return (size == 0);
        }

        usize size_bytes() noexcept {
            return size * sizeof(T);
        }

        usize capacity_bytes() const noexcept {
            return capacity * sizeof(T);
        }

        FatPtr<T> fat_ptr() noexcept {
            return FatPtr{buf, size};
        }

        FatPtr<T const> const_fat_ptr() const noexcept {
            return FatPtr{static_cast<T const*>(buf), size};
        }

        /// Get a pointer to the last element of the dynamic array.
        T* peek() const noexcept {
            return (this->size == 0) ? nullptr : &buf[size - 1];
        }

        T* begin() noexcept {
            return buf;
        }

        T const* begin() const noexcept {
            return static_cast<T const*>(buf);
        }

        T* end() noexcept {
            return psh_ptr_add(buf, size);
        }

        T const* end() const noexcept {
            return psh_ptr_add(static_cast<T const*>(buf), size);
        }

        T& operator[](usize idx) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(idx < size, "Index out of bounds for dynamic array");
#endif
            return buf[idx];
        }

        T const& operator[](usize idx) const noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(idx < size, "Index out of bounds for dynamic array");
#endif
            return buf[idx];
        }

        /// Fill the entire dynamic array's buffer (up to its current capacity) with a given
        /// element.
        void fill(T _fill) noexcept
            requires TriviallyCopyable<T>
        {
            psh::fill(fat_ptr(*this), _fill);
            size = capacity;
        }

        /// Inserts a new element to the end of the dynamic array.
        ///
        /// If the dynamic array capacity is full, the capacity is doubled by making a reallocation
        /// of the underlying memory buffer.
        void push(T new_element) noexcept {
            if (capacity == size) {
                this->grow();
            }
            buf[size++] = new_element;
        }

        /// Resize the dynamic array underlying buffer.
        void grow(u32 factor = DYNARRAY_RESIZE_CAPACITY_FACTOR) noexcept {
            if (psh_likely(buf != nullptr)) {
                // Reallocate the existing buffer.
                usize const previous_capacity = capacity;
                capacity *= factor;
                buf = arena->realloc<T>(buf, previous_capacity, capacity);
            } else {
                // Create a new buffer with a default capacity.
                capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY;
                buf      = arena->zero_alloc<T>(capacity);
            }

            if (psh_likely(capacity != 0)) {
                psh_assert_msg(buf != nullptr, "DynArray::grow unable to rellocate buffer");
            }
        }

        Status resize(usize new_capacity) noexcept {
            T* new_buf = arena->realloc<T>(buf, capacity, new_capacity);

            if (psh_unlikely(new_buf == nullptr)) {
                psh_error_fmt(
                    "DynArray::resize failed to resize capacity from %zu to %zu",
                    capacity,
                    new_capacity);
                return Status::Failed;
            }

            // Commit the resizing.
            buf      = new_buf;
            capacity = new_capacity;

            return Status::OK;
        }

        /// Try to pop the last element of the dynamic array.
        Status pop() noexcept {
            Status res = Status::Failed;
            if (psh_likely(size > 0)) {
                --size;
                res = Status::OK;
            }
            return res;
        }

        /// Try to remove a dynamic array element at a given index.
        Status remove(usize idx) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            if (psh_unlikely(idx >= size)) {
                psh_error_fmt(
                    "DynArray::remove index %zu is out of bounds for dynamic array of size "
                    "%zu.",
                    idx,
                    size);
                return Status::Failed;
            }
#endif

            if (idx != size - 1) {
                // If the element isn't the last we have copy the array content with overlap.
                u8*       dest = reinterpret_cast<u8*>(buf) + sizeof(T) * idx;
                u8 const* src  = reinterpret_cast<u8*>(buf) + sizeof(T) * (idx + 1);
                memory_move(dest, src, sizeof(T) * (size - idx - 1));
            }
            --size;
            return Status::OK;
        }

        /// Clear the dynamic array data, resetting its size.
        void clear() noexcept {
            size = 0;
        }
    };
}  // namespace psh
