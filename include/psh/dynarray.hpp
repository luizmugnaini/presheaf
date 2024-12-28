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

#include <psh/assert.hpp>
#include <psh/core.hpp>
#include <psh/memory.hpp>

namespace psh {
    namespace impl::dynarray {
        static constexpr usize DEFAULT_INITIAL_CAPACITY = 4;
        static constexpr usize RESIZE_CAPACITY_FACTOR   = 2;
    }  // namespace impl::dynarray

    /// Run-time variable length array.
    ///
    /// A dynamic array has its lifetime bound to its associated arena.
    template <typename T>
    struct psh_api DynArray {
        T*     buf;
        Arena* arena;
        usize  capacity = 0;
        usize  count    = 0;

        DynArray() psh_no_except = default;

        /// Initialize the dynamic array with a given capacity.
        psh_inline void init(Arena* arena_, usize capacity_ = impl::dynarray::DEFAULT_INITIAL_CAPACITY) psh_no_except {
            psh_assert_msg(this->count == 0, "Tried to re-initialize an initialized DynArray");

            this->buf      = memory_alloc<T>(arena_, capacity_);
            this->arena    = arena_;
            this->capacity = (this->buf != nullptr) ? capacity_ : 0;
        }

        /// Construct a dynamic array with a given capacity.
        psh_inline DynArray(Arena* arena_, usize capacity_ = impl::dynarray::DEFAULT_INITIAL_CAPACITY) psh_no_except {
            this->init(arena_, capacity_);
        }

        /// Grow the capacity of the dynamic array underlying buffer.
        Status grow(u32 factor = impl::dynarray::RESIZE_CAPACITY_FACTOR) psh_no_except {
            usize previous_capacity = this->capacity;

            usize  new_capacity;
            T*     new_buf;
            Arena* this_arena = this->arena;
            if (psh_likely(previous_capacity != 0)) {
                new_capacity = previous_capacity * factor;
                new_buf      = memory_realloc<T>(this_arena, this->buf, previous_capacity, new_capacity);
            } else {
                new_capacity = impl::dynarray::DEFAULT_INITIAL_CAPACITY;
                new_buf      = memory_alloc<T>(this_arena, new_capacity);
            }

            Status status = STATUS_FAILED;
            if (psh_likely(new_buf != nullptr)) {
                this->buf      = new_buf;
                this->capacity = new_capacity;

                status = STATUS_OK;
            }

            return status;
        }

        Status reserve(usize new_capacity) psh_no_except {
            // @NOTE: If T is a struct with a pointer to itself, this method will fail hard and create
            //       a massive horrible memory bug. DO NOT use this array structure with types having
            //       this property.
            T*     new_buf;
            Arena* this_arena = this->arena;
            if (this->capacity == 0) {
                new_buf = memory_alloc<T>(this_arena, new_capacity);
            } else {
                new_buf = memory_realloc<T>(this_arena, this->buf, this->capacity, new_capacity);
            }

            Status status = STATUS_FAILED;
            if (psh_likely(new_buf != nullptr)) {
                this->buf      = new_buf;
                this->capacity = new_capacity;

                status = STATUS_OK;
            }

            return status;
        }

        /// Inserts a new element to the end of the dynamic array.
        Status push(T new_element) psh_no_except {
            usize previous_count = this->count;

            Status status = STATUS_OK;
            if (this->capacity == previous_count) {
                status = this->grow();
            }

            if (psh_likely(status)) {
                this->buf[previous_count] = new_element;
                this->count               = previous_count + 1;
            }

            return status;
        }

        /// Insert a collection of new elements to the end of the dynamic array.
        Status push(FatPtr<T const> new_elements) psh_no_except {
            usize previous_count = this->count;

            Status status = STATUS_OK;
            if (this->capacity < new_elements.count + previous_count) {
                status = this->reserve(previous_count + new_elements.count);
            }

            if (psh_likely(status)) {
                memory_copy(
                    reinterpret_cast<u8*>(this->buf + previous_count),
                    reinterpret_cast<u8 const*>(new_elements.buf),
                    sizeof(T) * new_elements.count);
                this->count = previous_count + new_elements.count;
            }

            return status;
        }

        /// Try to pop the last element of the dynamic array.
        Status pop() psh_no_except {
            Status status = STATUS_FAILED;

            usize previous_count = this->count;
            if (psh_likely(previous_count > 0)) {
                this->count = previous_count - 1u;
                status      = STATUS_OK;
            }

            return status;
        }

        /// Try to remove a buffer element at a given index.
        ///
        /// This will move all of the buffer contents above the removed element index down one.
        Status ordered_remove(usize idx) psh_no_except {
            usize previous_count = this->count;

            if (psh_unlikely(idx >= previous_count)) {
                return STATUS_FAILED;
            }

            // If the element isn't the last we have to copy the array content with overlap.
            if (idx != previous_count - 1u) {
                u8*       dst = reinterpret_cast<u8*>(this->buf + idx);
                u8 const* src = reinterpret_cast<u8 const*>(this->buf + (idx + 1));
                memory_move(dst, src, sizeof(T) * (previous_count - idx - 1u));
            }

            this->count = previous_count - 1u;

            return STATUS_OK;
        }

        /// Try to remove a buffer element at a given index.
        ///
        /// This won't preserve the current ordering of the buffer.
        Status unordered_remove(usize idx) psh_no_except {
            usize previous_count = this->count;

            if (psh_unlikely(idx >= previous_count)) {
                return STATUS_FAILED;
            }

            this->buf[idx] = this->buf[previous_count - 1u];
            this->count    = previous_count - 1u;

            return STATUS_OK;
        }

        /// Clear the dynamic array data, resetting its size.
        psh_inline void clear() psh_no_except {
            this->count = 0;
        }

        psh_impl_generate_container_boilerplate(T, this->buf, this->count)
    };
}  // namespace psh
