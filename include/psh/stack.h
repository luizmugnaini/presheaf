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
/// Description: Stack memory allocator.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/core.h>
#include <psh/log.h>
#include <psh/math.h>
#include <psh/mem_utils.h>
#include <psh/option.h>
#include <cstring>

namespace psh {
    /// Header associated with each memory block in the stack allocator.
    ///
    /// Memory layout:
    ///
    /// ```md
    ///           `previous_offset`                     |-`capacity`-|
    ///                  ^                              ^            ^
    ///                  |                              |            |
    ///  |previous header|previous memory|+++++++|header|  memory    |
    ///                                  ^              ^
    ///                                  |--`padding`---|
    /// ```
    ///
    /// where "header" represents this current header, and "memory" represents the memory block
    /// associated to this header.
    struct StackHeader {
        /// Padding, in bytes, needed for the alignment of the memory block associated with the
        /// header. The padding accounts for both the size of the header and the needed alignment.
        usize padding;

        /// The capacity, in bytes, of the memory block associated with this header.
        usize capacity;

        /// Pointer offset, relative to the stack allocator memory  block, to the start of the
        /// memory address of the last allocated block (after its header).
        usize previous_offset;
    };

    /// Stack memory allocator.
    ///
    /// Note: The stack allocator **doesn't own** its memory, but merely manages it. That being
    ///       said, a stack allocator will never call `malloc` or `free`
    ///
    /// Memory layout:
    ///
    /// ```md
    ///          previous
    ///           offset                          current
    ///        for header 2                       offset
    ///             ^                               ^
    ///             |                               |
    ///    |header 1|memory 1|++++|header 2|memory 2| free space |
    ///    ^                 ^             ^                     ^
    ///    |                 |---padding---|                     |
    ///  start                             |                    end
    ///    |                            previous                 |
    ///    |                             offset                  |
    ///    |                                                     |
    ///    |--------------------- capacity ----------------------|
    /// ```
    ///
    /// Where each block of memory is preceded by a padding that comprises both the alignment needed
    /// for the memory block and its corresponding header. This header will store the size of the
    /// padding and the offset to the start the previous memory block with respect to itself.
    ///
    /// It is to be noted that the pointers returned by the methods associated to `StackAlloc` are
    /// all raw pointers. This means that if you get a memory block via `StackAlloc::alloc` and
    /// later free it via a `StackAlloc::clear_at`, you'll end up with a dangling pointer and
    /// use-after-free problems may arise if you later read from this pointer. This goes to say that
    /// the user should know how to correctly handle their memory reads and writes.
    struct Stack {
        u8*   buf             = nullptr;
        usize capacity        = 0;
        usize offset          = 0;
        usize previous_offset = 0;

        // -----------------------------------------------------------------------------
        // - Constructors and initializers -
        // -----------------------------------------------------------------------------

        Stack() noexcept = default;
        void init(u8* _buf, usize _capacity) noexcept;
        Stack(u8* _buf, usize _capacity) noexcept;

        // -----------------------------------------------------------------------------
        // - Allocated memory information -
        // -----------------------------------------------------------------------------

        /// Gets the total size, in bytes, of the memory used by the allocator.
        usize used() const noexcept;

        /// Gets a pointer to the memory of the last allocated memory block of the stack.
        u8* top() const noexcept;

        /// Gets a pointer to the header associated to the top memory block of the stack.
        StackHeader const* top_header() const noexcept;

        /// Get the capacity of the top memory block.
        usize top_capacity() const noexcept;

        /// Get the previous offset of the top memory block.
        usize top_previous_offset() const noexcept;

        /// Gets a pointer to the header associated to the given memory block.
        StackHeader const* header_of(u8 const* block) const noexcept;

        /// Get the capacity of the given memory block.
        usize capacity_of(u8 const* mem) const noexcept;

        /// Get the previous offset of the given memory block.
        usize previous_offset_of(u8 const* mem) const noexcept;

        // -----------------------------------------------------------------------------
        // - Allocation methods -
        // -----------------------------------------------------------------------------

        /// Allocates a new block of memory.
        ///
        /// Parameters:
        ///     * `length`: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        T* alloc(usize length) noexcept;

        /// Allocate a new zeroed block of memory.
        ///
        /// Parameters:
        ///     * `length`: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        T* zero_alloc(usize length) noexcept;

        /// Reallocate a block of memory of a given type.
        ///
        /// Parameters:
        ///     * `block`: Pointer to the start of the memory block to be resized.
        ///     * `new_length`: Number of entities of type `T` that the new memory block should be
        ///                     able to contain.
        ///
        /// Note: If the new length is zero, we proceed to clean the whole stack up until the given
        ///       block.
        template <typename T>
        T* realloc(T const* block, usize new_length) noexcept;

        // -----------------------------------------------------------------------------
        // - Memory manipulation utilities -
        // -----------------------------------------------------------------------------

        /// Tries to pop the last memory block allocated by the given stack.
        ///
        /// This function won't panic if the stack is empty, it will simply return false.
        Status pop() noexcept;

        /// Tries to reset the office to the start of the header of the block pointed by `ptr`.
        ///
        /// Parameters:
        ///     * `block`: Pointer to the memory block that should be freed (all blocks above `ptr`
        ///                will also be freed).
        ///
        /// Note:
        ///     * If `block` is null, we simply return `false`.
        ///     * If `block` doesn't correspond to a correct memory block we'll never be able to
        ///       match its location and therefore we'll end up clearing the entirety of the stack.
        ///       If this is your goal, prefer using StackAlloc::clear() instead.
        Status clear_at(u8 const* block) noexcept;

        /// Reset the allocator's offset.
        void clear() noexcept;
    };

    // -----------------------------------------------------------------------------
    // - Implementation of the stack allocation methods -
    // -----------------------------------------------------------------------------

    template <typename T>
    T* Stack::alloc(usize length) noexcept {
        if (psh_unlikely(length == 0)) {
            return nullptr;
        }

        usize const new_block_size = sizeof(T) * length;
        u8* const   free_mem       = this->buf + this->offset;
        usize const padding        = padding_with_header(
            reinterpret_cast<uptr>(free_mem),
            alignof(T),
            sizeof(StackHeader),
            alignof(StackHeader));
        usize const required = padding + new_block_size;

        if (psh_unlikely(required > this->capacity - this->offset)) {
            psh_error_fmt(
                "StackAlloc::alloc unable to allocate %zu bytes of memory (%zu bytes required "
                "due to alignment and padding). The stack allocator has only %zu bytes "
                "remaining.",
                new_block_size,
                required,
                this->capacity - this->offset);
            return nullptr;
        }

        // Address to the start of the new block of memory.
        u8* new_block = free_mem + padding;

        // Write to the header associated with the new block of memory.
        StackHeader* new_header     = reinterpret_cast<StackHeader*>(new_block - sizeof(StackHeader));
        new_header->padding         = padding;
        new_header->capacity        = new_block_size;
        new_header->previous_offset = this->previous_offset;

        // Update the stack offsets.
        this->previous_offset = this->offset + padding;
        this->offset += padding + new_block_size;

        return reinterpret_cast<T*>(new_block);
    }

    template <typename T>
    T* Stack::zero_alloc(usize length) noexcept {
        T* ptr = alloc<T>(length);
        std::memset(ptr, 0, length);
        return ptr;
    }

    template <typename T>
    T* Stack::realloc(T const* block, usize new_length) noexcept {
        u8 const* ublock = reinterpret_cast<u8 const*>(block);

        if (psh_unlikely(new_length == 0)) {
            this->clear_at(ublock);
            return nullptr;
        }

        usize const new_size = sizeof(T) * new_length;

        // If `ptr` is the last allocated block, just adjust the offsets.
        if (ublock == this->top()) {
            this->offset = this->previous_offset + new_size;
            return block;
        }

        // Check if the address is within the allocator's memory.
        if (psh_unlikely((ublock < this->buf) || (ublock >= this->buf + this->capacity))) {
            psh_error("StackAlloc::realloc called with a pointer outside of the memory region "
                      "managed by the stack allocator.");
            return nullptr;
        }

        // Check if the address is already free.
        if (psh_unlikely(ublock >= this->buf + this->offset)) {
            psh_error("StackAlloc::realloc called with a free block of memory (use-after-free "
                      "error).");
            return nullptr;
        }

        StackHeader const* header =
            reinterpret_cast<StackHeader const*>(ublock - sizeof(StackHeader));

        // Check memory availability.
        if (psh_unlikely(new_size > this->capacity - this->offset)) {
            psh_error_fmt(
                "StackAlloc::realloc cannot reallocate memory from size %zu to %zu. Only %zu "
                "bytes of memory remaining.",
                header->capacity,
                new_size,
                this->capacity - this->offset);
            return nullptr;
        }

        T* new_mem = alloc<T>(new_length);

        usize const copy_size = psh_min(header->capacity, new_size);
        memory_copy(reinterpret_cast<u8*>(new_mem), reinterpret_cast<u8 const*>(block), copy_size);

        return new_mem;
    }
}  // namespace psh
