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
/// Description: Fat pointer type.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/assert.hpp>
#include <psh/core.hpp>

namespace psh {
    /// Fat pointer, holds a pointer to a buffer and its corresponding size.
    template <typename T>
    struct psh_api FatPtr {
        T*    buf;
        usize count = 0;

        psh_impl_generate_container_boilerplate(T, this->buf, this->count)
    };

    /// Try to remove a buffer element at a given index.
    ///
    /// This will move all of the buffer contents above the removed element index down one.
    template <typename T>
    Status ordered_remove(FatPtr<T>& fptr, usize idx) psh_no_except {
        usize  previous_count = fptr.count;
        Status status         = (idx < previous_count);

        if (psh_likely(status)) {
            if (idx != previous_count - 1) {
                // If the element isn't the last we have to copy the array content with overlap.
                u8*       dst = reinterpret_cast<u8*>(fptr.buf + idx);
                u8 const* src = reinterpret_cast<u8 const*>(fptr.buf + (idx + 1));
                memory_move(dst, src, sizeof(T) * (previous_count - idx - 1u));
            }

            fptr->count = previous_count - 1;
        }

        return status;
    }

    template <typename Container, typename T = Container::ValueType>
    psh_inline FatPtr<T> make_slice(Container& c, usize start_idx, usize slice_count) psh_no_except {
        psh_static_assert_valid_mutable_container_type(Container, c);
        psh_assert_bounds_check(slice_count, c.count + 1);

        return FatPtr<T>{c.buf + start_idx, slice_count};
    }

    template <typename Container, typename T = Container::ValueType>
    psh_inline FatPtr<T const> make_const_slice(Container const& c, usize start_idx, usize slice_count) psh_no_except {
        psh_static_assert_valid_const_container_type(Container, c);
        psh_assert_bounds_check(start_idx + slice_count - 1, c.count + 1);

        return FatPtr<T const>{c.buf + start_idx, slice_count};
    }

    template <typename Container, typename T = Container::ValueType>
    psh_inline FatPtr<T> make_fat_ptr(Container& c) psh_no_except {
        psh_static_assert_valid_mutable_container_type(Container, c);

        return FatPtr<T>{c.buf, c.count};
    }

    template <typename Container, typename T = Container::ValueType>
    psh_inline FatPtr<T const> make_const_fat_ptr(Container const& c) psh_no_except {
        psh_static_assert_valid_const_container_type(Container, c);

        return FatPtr<T const>{reinterpret_cast<T const*>(c.buf), c.count};
    }
}  // namespace psh
