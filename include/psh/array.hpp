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

#include <psh/arena.hpp>
#include <psh/core.hpp>
#include <psh/memory_utils.hpp>

namespace psh {
    /// Array with run-time known constant capacity.
    ///
    /// The array lifetime is bound to the lifetime of the arena passed at initialization, being
    /// responsible to allocate the memory referenced by the array.
    template <typename T>
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

                this->buf = _arena->alloc<T>(this->size);
                psh_assert_msg(this->buf != nullptr, ERROR_INIT_OUT_OF_MEMORY);
            }
        }

        /// Construct an array with a given size.
        Array(Arena* _arena, usize _size) noexcept {
            this->init(_arena, _size);
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
    FatPtr<T> fat_ptr(Array<T>& a) noexcept {
        return FatPtr{a.buf, a.size};
    }

    template <typename T>
    FatPtr<T const> const_fat_ptr(Array<T> const& a) noexcept {
        return FatPtr{reinterpret_cast<T const*>(a.buf), a.size};
    }
}  // namespace psh
