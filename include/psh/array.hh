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
/// Description: Fixed size array types whose size is only known at runtime, analogous to a VLA, but
///              has its memory bound to a parent arena allocator.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <initializer_list>
#include <psh/arena.hh>
#include <psh/core.hh>
#include <psh/memory_utils.hh>
#include <psh/type_utils.hh>

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
        // - Diagnostic messages -
        // -----------------------------------------------------------------------------

        static constexpr strptr ERROR_INIT_INCONSISTENT_ARENA =
            "Array initialization called with non-zero capacity but an empty arena";
        static constexpr strptr ERROR_INIT_OUT_OF_MEMORY =
            "Array initialization unable to acquire enough bytes of memory";
        static constexpr strptr ERROR_ACCESS_OUT_OF_BOUNDS = "DynArray access out of bounds";

        // -----------------------------------------------------------------------------
        // - Constructors and initializers -
        // -----------------------------------------------------------------------------

        Array() noexcept = default;

        /// Initialize the array with a given size.
        void init(Arena* _arena, usize _size) noexcept {
            this->size = _size;
            if (psh_likely(this->size != 0)) {
                psh_assert_msg(_arena != nullptr, ERROR_INIT_INCONSISTENT_ARENA);

                this->buf = _arena->zero_alloc<T>(this->size);
                psh_assert_msg(this->buf != nullptr, ERROR_INIT_OUT_OF_MEMORY);
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
                psh_assert_msg(_arena != nullptr, ERROR_INIT_INCONSISTENT_ARENA);

                this->buf = _arena->alloc<T>(this->size);
                psh_assert_msg(this->buf != nullptr, ERROR_INIT_OUT_OF_MEMORY);
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
                psh_assert_msg(_arena != nullptr, ERROR_INIT_INCONSISTENT_ARENA);

                this->buf = _arena->alloc<T>(this->size);
                psh_assert_msg(this->buf != nullptr, ERROR_INIT_OUT_OF_MEMORY);
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
            psh_assert_msg(index < this->size, ERROR_ACCESS_OUT_OF_BOUNDS);
#endif
            return this->buf[index];
        }

        T const& operator[](usize index) const noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(index < this->size, ERROR_ACCESS_OUT_OF_BOUNDS);
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
