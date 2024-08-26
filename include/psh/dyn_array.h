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
/// Description: Dynamic array.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/arena.h>
#include <psh/core.h>
#include <psh/mem_utils.h>
#include <psh/option.h>
#include <initializer_list>

namespace psh {
    /// Run-time variable length array.
    ///
    /// A dynamic array has its lifetime bound to its associated arena.
    template <typename T>
        requires IsObject<T>
    struct DynArray {
        Arena* arena    = nullptr;
        usize  size     = 0;
        usize  capacity = 0;
        T*     buf      = nullptr;

        static constexpr usize DYNARRAY_DEFAULT_INITIAL_CAPACITY = 4;
        static constexpr usize DYNARRAY_RESIZE_CAPACITY_FACTOR   = 2;

        // -----------------------------------------------------------------------------
        // - Diagnostic messages -
        // -----------------------------------------------------------------------------

        static constexpr strptr ERROR_INIT_INCONSISTENT_ARENA =
            "DynArray initialization called with non-zero capacity but an empty arena";
        static constexpr strptr ERROR_INIT_OUT_OF_MEMORY =
            "DynArray initialization unable to acquire enough bytes of memory";
        static constexpr strptr ERROR_INIT_INCONSISTENT_SIZE_AND_CAPACITY =
            "DynArray initialization called with inconsistent data: capacity less than the size";
        static constexpr strptr ERROR_ACCESS_OUT_OF_BOUNDS = "DynArray access out of bounds";
        static constexpr strptr ERROR_RESIZE_OUT_OF_MEMORY =
            "DynArray cannot be resized because its allocator is out of memory";

        // -----------------------------------------------------------------------------
        // - Constructors and initializers -
        // -----------------------------------------------------------------------------

        DynArray() noexcept = default;

        /// Initialize the dynamic array with a given capacity.
        void init(Arena* _arena, usize _capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY) noexcept {
            this->arena    = _arena;
            this->capacity = _capacity;

            if (psh_likely(this->capacity != 0)) {
                psh_assert_msg(this->arena != nullptr, ERROR_INIT_INCONSISTENT_ARENA);

                this->buf = arena->zero_alloc<T>(this->capacity);
                psh_assert_msg(this->buf != nullptr, ERROR_INIT_OUT_OF_MEMORY);
            }
        }

        /// Construct a dynamic array with a given capacity.
        DynArray(Arena* _arena, usize _capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY) noexcept {
            this->init(_arena, _capacity);
        }

        /// Initialize the dynamic array with the contents of an initializer list, and optionally
        /// reserve a given capacity.
        void init(
            std::initializer_list<T> const& list,
            Arena*                          _arena,
            Option<usize> const&            _capacity = {}) noexcept {
            this->arena    = _arena;
            this->size     = list.size();
            this->capacity = _capacity.val_or(DYNARRAY_RESIZE_CAPACITY_FACTOR * this->size);
            psh_assert_msg(this->size <= this->capacity, ERROR_INIT_INCONSISTENT_SIZE_AND_CAPACITY);

            if (psh_likely(this->capacity != 0)) {
                psh_assert_msg(this->arena != nullptr, ERROR_INIT_INCONSISTENT_ARENA);

                this->buf = arena->alloc<T>(this->capacity);
                psh_assert_msg(this->buf != nullptr, ERROR_INIT_OUT_OF_MEMORY);
            }

            std::memcpy(
                reinterpret_cast<u8*>(this->buf),
                reinterpret_cast<u8 const*>(list.begin()),
                sizeof(T) * this->size);
        }

        /// Construct a dynamic array with the contents of an initializer list, and optionally
        /// reserve a given capacity.
        DynArray(
            std::initializer_list<T> list,
            Arena*                   _arena,
            Option<usize>            _capacity = {}) noexcept {
            this->init(list, _arena, _capacity);
        }

        /// Initialize the dynamic array with the contents of a fat pointer, and optionally reserve
        /// a given capacity.
        void
        init(FatPtr<T const> fptr, Arena* _arena, Option<usize> const& _capacity = {}) noexcept {
            this->arena    = _arena;
            this->size     = fptr.size;
            this->capacity = _capacity.val_or(DYNARRAY_RESIZE_CAPACITY_FACTOR * this->size);
            psh_assert_msg(this->size <= this->capacity, ERROR_INIT_INCONSISTENT_SIZE_AND_CAPACITY);

            if (psh_likely(this->capacity != 0)) {
                psh_assert_msg(this->arena != nullptr, ERROR_INIT_INCONSISTENT_ARENA);

                this->buf = arena->alloc<T>(this->capacity);
                psh_assert_msg(this->buf != nullptr, ERROR_INIT_OUT_OF_MEMORY);
            }

            std::memcpy(
                reinterpret_cast<u8*>(this->buf),
                reinterpret_cast<u8 const*>(fptr.begin()),
                fptr.size_bytes());
        }

        /// Construct a dynamic array with the contents of a fat pointer, and optionally reserve a
        /// given capacity.
        DynArray(FatPtr<T const> fptr, Arena* _arena, Option<usize> _capacity = {}) noexcept {
            this->init(fptr, _arena, _capacity);
        }

        // -----------------------------------------------------------------------------
        // - Size related utilities -
        // -----------------------------------------------------------------------------

        bool is_empty() const noexcept {
            return (this->size == 0);
        }

        usize size_bytes() noexcept {
            return this->size * sizeof(T);
        }

        usize capacity_bytes() const noexcept {
            return this->capacity * sizeof(T);
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

        /// Get a pointer to the last element of the dynamic array.
        T* peek() const noexcept {
            return (this->size == 0) ? nullptr : &this->buf[this->size - 1];
        }

        // -----------------------------------------------------------------------------
        // - Indexed reads -
        // -----------------------------------------------------------------------------

        T& operator[](usize idx) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(idx < this->size, ERROR_ACCESS_OUT_OF_BOUNDS);
#endif
            return this->buf[idx];
        }

        T const& operator[](usize idx) const noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(idx < this->size, ERROR_ACCESS_OUT_OF_BOUNDS);
#endif
            return this->buf[idx];
        }

        // -----------------------------------------------------------------------------
        // - Memory resizing methods -
        // -----------------------------------------------------------------------------

        /// Resize the dynamic array underlying buffer.
        void grow(u32 factor = DYNARRAY_RESIZE_CAPACITY_FACTOR) noexcept {
            if (psh_likely(this->buf != nullptr)) {
                // Reallocate the existing buffer.
                usize previous_capacity = this->capacity;
                this->capacity *= factor;
                this->buf = arena->realloc<T>(this->buf, previous_capacity, this->capacity);
            } else {
                // Create a new buffer with a default capacity.
                this->capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY;
                this->buf      = arena->zero_alloc<T>(this->capacity);
            }

            if (psh_likely(this->capacity != 0)) {
                psh_assert_msg(this->buf != nullptr, ERROR_RESIZE_OUT_OF_MEMORY);
            }
        }

        Status resize(usize new_capacity) noexcept {
            T* new_buf = arena->realloc<T>(this->buf, this->capacity, new_capacity);

            if (psh_unlikely(new_buf == nullptr)) {
                // TODO: should this be a fatal error?
                psh_error(ERROR_RESIZE_OUT_OF_MEMORY);
                return Status::FAILED;
            }

            // Commit the resizing.
            this->buf      = new_buf;
            this->capacity = new_capacity;

            return Status::OK;
        }

        // -----------------------------------------------------------------------------
        // - Memory manipulation -
        // -----------------------------------------------------------------------------

        /// Inserts a new element to the end of the dynamic array.
        void push(T new_element) noexcept {
            if (this->capacity == this->size) {
                this->grow();
            }
            this->buf[this->size++] = new_element;
        }

        /// Try to pop the last element of the dynamic array.
        Status pop() noexcept {
            Status res = Status::FAILED;
            if (psh_likely(this->size > 0)) {
                this->size -= 1;
                res = Status::OK;
            }
            return res;
        }

        /// Try to remove a dynamic array element at a given index.
        Status remove(usize idx) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            if (psh_unlikely(idx >= this->size)) {
                psh_error(ERROR_ACCESS_OUT_OF_BOUNDS);
                return Status::FAILED;
            }
#endif

            // If the element isn't the last we have to copy the array content with overlap.
            if (idx != this->size - 1) {
                u8*       dest = reinterpret_cast<u8*>(this->buf) + sizeof(T) * idx;
                u8 const* src  = reinterpret_cast<u8*>(this->buf) + sizeof(T) * (idx + 1);
                memory_move(dest, src, sizeof(T) * (this->size - idx - 1));
            }

            this->size -= 1;

            return Status::OK;
        }

        /// Clear the dynamic array data, resetting its size.
        void clear() noexcept {
            this->size = 0;
        }
    };

    // -----------------------------------------------------------------------------
    // - Generating fat pointers -
    // -----------------------------------------------------------------------------

    template <typename T>
    inline FatPtr<T> fat_ptr(DynArray<T>& d) noexcept {
        return FatPtr{d.buf, d.size};
    }

    template <typename T>
    inline FatPtr<T const> const_fat_ptr(DynArray<T> const& d) noexcept {
        return FatPtr{static_cast<T const*>(d.buf), d.size};
    }
}  // namespace psh
