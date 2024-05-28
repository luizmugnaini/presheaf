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
/// Description: Utilities for memory-related operations.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/concepts.h>
#include <psh/fat_ptr.h>
#include <psh/not_null.h>
#include <psh/types.h>

/// Add or subtract an offset from a pointer if and only if the pointer is not null.
#define psh_ptr_add(ptr, offset) ((ptr) == nullptr ? nullptr : (ptr) + (offset))
#define psh_ptr_sub(ptr, offset) ((ptr) == nullptr ? nullptr : (ptr) - (offset))

namespace psh {
    // -----------------------------------------------------------------------------
    // - CPU architecture query utilities -
    // -----------------------------------------------------------------------------

    /// Check whether the current architecture is little-endian or big-endian.
    ///
    /// Note: Unfortunately, these checks can only be reliably made at runtime.
    bool arch_is_little_endian() noexcept;
    bool arch_is_bit_endian() noexcept;

    // -----------------------------------------------------------------------------
    // - Memory manipulation utilities -
    // -----------------------------------------------------------------------------

    /// Simple wrapper around `std::memset` that automatically deals with null values.
    ///
    /// Does nothing if `ptr` is a null pointer.
    void memory_set(FatPtr<u8> fat_ptr, i32 fill) noexcept;

    /// Simple wrapper around `std::memcpy`.
    ///
    /// This function will assert that the blocks of memory don't overlap, avoiding undefined
    /// behaviour introduced by `std::memcpy` in this case.
    void memory_copy(u8* dest, u8 const* src, usize size) noexcept;

    /// Simple wrapper around `std::memmove`.
    ///
    /// Does nothing if either `dest` or `src` are null pointers.
    void memory_move(u8* dest, u8 const* src, usize size) noexcept;

    // -----------------------------------------------------------------------------
    // - Alignment utilities -
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
    usize padding_with_header(
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
    usize align_forward(uptr ptr, usize alignment) noexcept;
}  // namespace psh
