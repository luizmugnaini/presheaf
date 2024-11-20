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

#include <psh/arena.hpp>
#include <psh/core.hpp>
#include <psh/memory.hpp>
#include <psh/option.hpp>

namespace psh {
    namespace impl::dyn_array {
        static constexpr usize DEFAULT_INITIAL_CAPACITY = 4;
        static constexpr usize RESIZE_CAPACITY_FACTOR   = 2;
    }  // namespace impl::dyn_array

    /// Run-time variable length array.
    ///
    /// A dynamic array has its lifetime bound to its associated arena.
    template <typename T>
    struct psh_api DynArray {
        T*     buf;
        Arena* arena;
        usize  capacity = 0;
        usize  count    = 0;

        // -----------------------------------------------------------------------------
        // Constructors and initializers.
        // -----------------------------------------------------------------------------

        DynArray() noexcept = default;

        /// Initialize the dynamic array with a given capacity.
        psh_inline void init(Arena* arena_, usize capacity_ = impl::dyn_array::DEFAULT_INITIAL_CAPACITY) noexcept {
            this->arena    = arena_;
            this->capacity = capacity_;

            if (psh_likely(capacity_ != 0)) {
                psh_assert_msg(this->arena != nullptr, "DynArray initialization called with non-zero capacity but an empty arena.");

                this->buf = arena->alloc<T>(capacity_);
                psh_assert_msg(this->buf != nullptr, "DynArray initialization unable to acquire enough bytes of memory.");
            }
        }

        /// Construct a dynamic array with a given capacity.
        psh_inline DynArray(Arena* arena_, usize capacity_ = impl::dyn_array::DEFAULT_INITIAL_CAPACITY) noexcept {
            this->init(arena_, capacity_);
        }

        // -----------------------------------------------------------------------------
        // Read methods.
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
            return psh_ptr_add(static_cast<T const*>(this->buf), this->count);
        }

        psh_inline T& operator[](usize idx) noexcept {
#if defined(PSH_CHECK_BOUNDS)
            psh_assert_fmt(idx < this->count, "Index %zu out of bounds for DynArray of size %zu.", idx, this->count);
#endif
            return this->buf[idx];
        }

        psh_inline T const& operator[](usize idx) const noexcept {
#if defined(PSH_CHECK_BOUNDS)
            psh_assert_fmt(idx < this->count, "Index %zu out of bounds for DynArray of size %zu.", idx, this->count);
#endif
            return this->buf[idx];
        }

        /// Get a pointer to the last element of the dynamic array.
        psh_inline T* peek() const noexcept {
            return (this->count == 0) ? nullptr : &this->buf[this->count - 1];
        }

        // -----------------------------------------------------------------------------
        // Memory resizing methods.
        // -----------------------------------------------------------------------------

        psh_inline usize size_bytes() noexcept {
            return this->count * sizeof(T);
        }

        /// Resize the dynamic array underlying buffer.
        void grow(u32 factor = impl::dyn_array::RESIZE_CAPACITY_FACTOR) noexcept {
            if (psh_likely(this->buf != nullptr)) {
                // Reallocate the existing buffer.
                usize previous_capacity = this->capacity;
                this->capacity *= factor;
                this->buf = arena->realloc<T>(this->buf, previous_capacity, this->capacity);
            } else {
                // Create a new buffer with a default capacity.
                this->capacity = impl::dyn_array::DEFAULT_INITIAL_CAPACITY;
                this->buf      = arena->alloc<T>(this->capacity);
            }

            if (psh_likely(this->capacity != 0)) {
                psh_assert_msg(this->buf != nullptr, "DynArray failed to grow, allocator out of memory.");
            }
        }

        Status resize(usize new_capacity) noexcept {
            // @NOTE: If T is a struct with a pointer to itself, this method will fail hard and create
            //       a massive horrible memory bug. DO NOT use this array structure with types having
            //       this property.
            T* new_buf = arena->realloc<T>(this->buf, this->capacity, new_capacity);

            if (psh_unlikely(new_buf == nullptr)) {
                // @TODO: should this be a fatal error?
                psh_log_error("DynArray failed to resize, allocator out of memory");
                return STATUS_FAILED;
            }

            // Commit the resizing.
            this->buf      = new_buf;
            this->capacity = new_capacity;

            return STATUS_OK;
        }

        // -----------------------------------------------------------------------------
        // Memory manipulation.
        // -----------------------------------------------------------------------------

        /// Inserts a new element to the end of the dynamic array.
        void push(T new_element) noexcept {
            if (this->capacity == this->count) {
                this->grow();
            }
            this->buf[this->count++] = new_element;
        }

        void push(FatPtr<T const> new_elements) noexcept {
            if (this->capacity < new_elements.count + this->count) {
                this->resize(this->count + new_elements.count);
            }

            memory_copy(reinterpret_cast<u8*>(this->buf), reinterpret_cast<u8 const*>(new_elements.buf), sizeof(T) * new_elements.count);
            this->count += new_elements.count;
        }

        /// Try to pop the last element of the dynamic array.
        Status pop() noexcept {
            Status res = STATUS_FAILED;
            if (psh_likely(this->count > 0)) {
                this->count -= 1;
                res = STATUS_OK;
            }
            return res;
        }

        /// Try to remove a dynamic array element at a given index.
        Status remove(usize idx) noexcept {
#if defined(PSH_CHECK_BOUNDS)
            if (psh_unlikely(idx >= this->count)) {
                psh_log_error_fmt("Index %zu out of bounds for DynArray of count %zu.", idx, this->count);
                return STATUS_FAILED;
            }
#endif

            // If the element isn't the last we have to copy the array content with overlap.
            if (idx != this->count - 1) {
                u8*       dst = reinterpret_cast<u8*>(this->buf + idx);
                u8 const* src = reinterpret_cast<u8 const*>(this->buf + (idx + 1));
                memory_move(dst, src, sizeof(T) * (this->count - idx - 1));
            }

            this->count -= 1;

            return STATUS_OK;
        }

        /// Clear the dynamic array data, resetting its size.
        psh_inline void clear() noexcept {
            this->count = 0;
        }
    };

    // -----------------------------------------------------------------------------
    // Generating fat pointers.
    // -----------------------------------------------------------------------------

    template <typename T>
    psh_inline FatPtr<T> make_fat_ptr(DynArray<T>& d) noexcept {
        return FatPtr{d.buf, d.count};
    }

    template <typename T>
    psh_inline FatPtr<T const> make_const_fat_ptr(DynArray<T> const& d) noexcept {
        return FatPtr{static_cast<T const*>(d.buf), d.count};
    }
}  // namespace psh
