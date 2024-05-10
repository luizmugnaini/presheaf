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

namespace psh {
    /// Adds an offset to a pointer considering null pointers..
    ///
    /// If the pointer is null then a null pointer is returned, otherwise we return the resulting
    /// pointer addition.
    template <typename T>
    constexpr T* ptr_add(T* ptr, uptr offset) noexcept {
        return (ptr == nullptr) ? nullptr : ptr + offset;
    }

    /// Subtracts an offset to a pointer considering null pointers..
    ///
    /// If the pointer is null then a null pointer is returned, otherwise we return the resulting
    /// pointer subtraction.
    template <typename T>
    constexpr T* ptr_sub(T* ptr, uptr offset) noexcept {
        return (ptr == nullptr) ? nullptr : ptr - offset;
    }

    /// Return a number with all bits set to 0 but the one at position `n`.
    constexpr i32 bit(i32 n) noexcept {
        return 1 << n;
    }

    /// Return a number with all bits set to 1 but the one at position `n`.
    constexpr u32 clear_bit(u32 n) noexcept {
        return ~(1u << n);
    }

    /// Return the given byte as the high byte of a 16 bit unsigned integer.
    constexpr u16 as_high_u16(u8 b) {
        return static_cast<u16>(b << 8);
    }

    template <typename T>
    using MatchFn = bool(T lhs, T rhs);

    /// Check if a range given by a fat pointer contains a given `match` element.
    template <typename T>
        requires IsObject<T> && TriviallyCopyable<T>
    bool contains(T match, FatPtr<T const> container, MatchFn<T>* match_fn) {
        psh_assert_msg(match_fn != nullptr, "contains expected a valid match function");
        bool found = false;
        for (auto const& m : container) {
            if (match_fn(match, m)) {
                found = true;
                break;
            }
        }
        return found;
    }

    /// Check if a range given by a fat pointer contains a given `match` element.
    template <typename T>
        requires IsObject<T> && TriviallyCopyable<T> && Reflexive<T>
    bool contains(T match, FatPtr<T const> container) {
        bool found = false;
        for (auto const& m : container) {
            if (match == m) {
                found = true;
                break;
            }
        }
        return found;
    }

    /// Simple wrapper around `std::memset` that automatically deals with null values.
    ///
    /// Does nothing if `ptr` is a null pointer.
    void memory_set(FatPtr<u8> fat_ptr, i32 fill) noexcept;

    /// Override the contents of a fat pointer with a given element.
    ///
    /// This is the virtually same as `memory_set` but can copy elements of any type. However it
    /// will be slower.
    template <typename T>
        requires TriviallyCopyable<T>
    void fill(FatPtr<T> fat_ptr, T _fill) noexcept {
        for (auto& elem : fat_ptr) {
            elem = _fill;
        }
    }

    /// Given a pointer to a structure, zeroes every field of said instance.
    template <typename T>
        requires NotPointer<T>
    void zero_struct(T* obj) noexcept {
        memory_set(fat_ptr_as_bytes(obj, 1), 0);
    }

    /// Simple wrapper around `std::memcpy`.
    ///
    /// This function will assert that the blocks of memory don't overlap, avoiding undefined
    /// behaviour introduced by `std::memcpy` in this case.
    void memory_copy(u8* dest, u8 const* src, usize size) noexcept;

    /// Simple wrapper around `std::memmove`.
    ///
    /// Does nothing if either `dest` or `src` are null pointers.
    void memory_move(u8* dest, u8 const* src, usize size) noexcept;

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
