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
/// Description: Utilities for memory-related operations.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/core.hpp>
#include <psh/fat_ptr.hpp>

namespace psh {
    // -----------------------------------------------------------------------------
    // CPU architecture query utilities.
    // -----------------------------------------------------------------------------

    /// Check whether the current architecture is little-endian or big-endian.
    ///
    /// Note: Unfortunately, these checks can only be reliably made at runtime.
    psh_api bool arch_is_little_endian() noexcept;
    psh_api bool arch_is_bit_endian() noexcept;

    // -----------------------------------------------------------------------------
    // Memory manipulation utilities.
    // -----------------------------------------------------------------------------

    /// Simple wrapper around `memset` that automatically deals with null values.
    ///
    /// Does nothing if `ptr` is a null pointer.
    psh_api void memory_set(void* buf, usize size_bytes, i32 fill) noexcept;

    template <typename T>
    void zero_struct(T& s) noexcept {
        memory_set(reinterpret_cast<u8*>(&s), sizeof(T), 0);
    }

    /// Simple wrapper around `memcpy`.
    ///
    /// This function will assert that the blocks of memory don't overlap, avoiding undefined
    /// behaviour introduced by `memcpy` in this case.
    psh_api void memory_copy(void* psh_restrict_ptr dst, void const* psh_restrict_ptr src, usize size_bytes) noexcept;

    template <typename T>
    void memory_copy(FatPtr<T> dst, FatPtr<T const> src, usize copy_count) noexcept {
        psh_assert_fmt(
            copy_count <= dst.size,
            "Destination cannot hold %zu elements (max %zu).",
            copy_count,
            dst.size);
        psh_assert_fmt(
            copy_count <= src.size,
            "Source doesn't have %zu elements (max %zu).",
            copy_count,
            src.size);

        memory_copy(dst.buf, src.buf, sizeof(T) * copy_count);
    }

    /// Simple wrapper around `memmove`.
    ///
    /// Does nothing if either `dst` or `src` are null pointers.
    psh_api void memory_move(void* psh_restrict_ptr dst, void const* psh_restrict_ptr src, usize size_bytes) noexcept;

    template <typename T>
    void memory_move(FatPtr<T> dst, FatPtr<T const> src, usize copy_count) noexcept {
        psh_assert(copy_count <= dst.size);
        psh_assert(copy_count <= src.size);

        memory_move(dst.buf, src.buf, sizeof(T) * copy_count);
    }

    // -----------------------------------------------------------------------------
    // Alignment utilities.
    // -----------------------------------------------------------------------------

    /// Compute the padding needed for the alignment of the memory and header.
    ///
    /// The padding should contain the header, thus it is ensured that `padding >= header_size`.
    /// Both the alignment needed for the new memory block as the alignment required by the header
    /// will be accounted when calculating the padding.
    ///
    /// Parameters:
    ///     * ptr: The current memory address.
    ///     * alignment: The alignment requirement for the new memory block.
    ///     * header_size: The total size of the header associated to the new memory block.
    ///     * header_alignment: The alignment required by the header.
    ///
    /// Return: The resulting padding with respect to `ptr` that should satisfy the alignment
    ///         requirements, as well as accommodating the associated header.
    psh_api usize padding_with_header(
        uptr  ptr,
        usize alignment,
        usize header_size,
        usize header_alignment) noexcept;

    /// Compute the next address that satisfies a given alignment.
    ///
    /// The alignment should always be a power of two.
    ///
    /// Parameters:
    ///     * ptr: The starting address.
    ///     * alignment: The alignment requirement.
    ///
    /// Return: The next address, relative to `ptr` that satisfies the alignment requirement imposed
    ///         by `alignment`.
    psh_api usize align_forward(uptr ptr, usize alignment) noexcept;
}  // namespace psh
