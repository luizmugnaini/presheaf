///                             Presheaf libraryarray
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
#include <psh/memory.hpp>

namespace psh {
    /// Array with run-time known constant capacity.
    ///
    /// The array lifetime is bound to the lifetime of the arena passed at initialization, being
    /// responsible to allocate the memory referenced by the array.
    template <typename T>
    struct psh_api Array {
        T*    buf;
        usize count = 0;

        Array() noexcept = default;

        /// Initialize the array with a given size.
        psh_inline void init(Arena* arena_, usize count_) noexcept {
            psh_assert_msg(this->count == 0, "Tried to re-initialize an initialized Array.");
            psh_assert(arena_ != nullptr);

            this->buf   = arena_->alloc<T>(count_);
            this->count = (this->buf != nullptr) ? count_ : 0;
        }

        /// Construct an array with a given size.
        psh_inline Array(Arena* arena_, usize count_) noexcept {
            this->init(arena_, count_);
        }

        psh_inline usize size_bytes() const noexcept {
            return sizeof(T) * this->count;
        }

        psh_inline T& operator[](usize idx) noexcept {
#if defined(PSH_CHECK_BOUNDS)
            psh_assert_fmt(idx < this->count, "Index %zu out of bounds for Array of size %zu.", idx, this->count);
#endif
            return this->buf[idx];
        }

        psh_inline T const& operator[](usize idx) const noexcept {
#if defined(PSH_CHECK_BOUNDS)
            psh_assert_fmt(idx < this->count, "Index %zu out of bounds for Array of size %zu.", idx, this->count);
#endif
            return this->buf[idx];
        }

        psh_inline T*       begin() noexcept { return this->buf; }
        psh_inline T*       end() noexcept { return psh_ptr_add(this->buf, this->count); }
        psh_inline T const* begin() const noexcept { return static_cast<T const*>(this->buf); }
        psh_inline T const* end() const noexcept { return static_cast<T const*>(psh_ptr_add(this->buf, this->count)); }
    };

    // -----------------------------------------------------------------------------
    // Generating fat pointers.
    // -----------------------------------------------------------------------------

    template <typename T>
    psh_inline FatPtr<T> make_fat_ptr(Array<T>& a) noexcept {
        return FatPtr<T>{a.buf, a.count};
    }

    template <typename T>
    psh_inline FatPtr<T const> make_const_fat_ptr(Array<T> const& a) noexcept {
        return FatPtr<T const>{reinterpret_cast<T const*>(a.buf), a.count};
    }
}  // namespace psh
