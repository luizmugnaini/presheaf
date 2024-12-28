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

#include <psh/assert.hpp>
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

        psh_impl_generate_container_boilerplate(T, this->buf, this->count)
    };

    template <typename T>
    psh_inline void array_init(Array<T>* array, Arena* arena, usize count) psh_no_except {
        psh_assert_not_null(array);
        psh_assert_msg(array->count == 0, "Tried to re-initialize an Array.");

        T* buf       = memory_alloc<T>(arena, count);
        array->buf   = buf;
        array->count = (buf != nullptr) ? count : 0;
    }
}  // namespace psh
