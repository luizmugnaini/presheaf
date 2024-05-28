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
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/intrinsics.h>
#include <psh/io.h>
#include <psh/math.h>
#include <psh/mem_utils.h>
#include <psh/option.h>
#include <psh/types.h>
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
    /// said,
    ///       a stack allocator will never call `malloc` or `free`
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
        /// Pointer to the memory region managed by the allocator.
        u8* memory = nullptr;

        /// Total capacity, in bytes, of the allocator.
        usize capacity = 0;

        /// Offset, in bytes, to the start of the free space region.
        usize offset = 0;

        /// Pointer offset relative to the start of the memory address of the last allocated block
        /// (after its header).
        usize previous_offset = 0;

        constexpr explicit Stack() noexcept = default;

        explicit Stack(u8* _memory, usize _capacity) noexcept
            : memory{_memory}, capacity{_capacity} {
            if (psh_likely(capacity != 0)) {
                psh_assert_msg(
                    memory != nullptr,
                    "Stack constructed with non-zero capacity but null memory");
            }
        }

        void init(u8* _memory, usize _capacity) noexcept {
            memory   = _memory;
            capacity = _capacity;
            if (psh_likely(capacity != 0)) {
                psh_assert_msg(
                    memory != nullptr,
                    "Stack::init called with non-zero capacity but null memory");
            }
        }

        /// Gets the total size, in bytes, of the memory used by the allocator.
        usize used() const noexcept {
            return offset;
        }

        /// Gets a pointer to the memory of the last allocated memory block of the stack.
        u8* top() const noexcept {
            return psh_ptr_add(memory, previous_offset);
        }

        /// Gets a pointer to the header associated to the top memory block of the stack.
        StackHeader const* top_header() const noexcept {
            return reinterpret_cast<StackHeader const*>(
                psh_ptr_add(memory, previous_offset - sizeof(StackHeader)));
        }

        /// Get the capacity of the top memory block.
        usize top_capacity() const noexcept {
            StackHeader const* header = this->top_header();
            return (header == nullptr) ? 0 : header->capacity;
        }

        /// Get the previous offset of the top memory block.
        usize top_previous_offset() const noexcept {
            StackHeader const* header = this->top_header();
            return (header == nullptr) ? 0 : header->previous_offset;
        }

        /// Gets a pointer to the header associated to the given memory block.
        StackHeader const* header_of(u8 const* block) const noexcept {
            u8 const* block_header = block + sizeof(StackHeader);
            bool      valid        = true;

            if (psh_unlikely(block == nullptr)) {
                valid = false;
            }
            if (psh_unlikely((block < memory) || (block >= memory + capacity))) {
                psh_error("StackAlloc::header_of called with a pointer to a block of memory "
                          "outside of the stack allocator scope.");
                valid = false;
            }
            if (psh_unlikely(block > memory + previous_offset)) {
                psh_error(
                    "StackAlloc::header_of called with a pointer to a freed block of memory.");
                valid = false;
            }
            if (psh_unlikely(block_header < memory)) {
                psh_error("StackAlloc::header_of expected the memory block header to be contained "
                          "in the stack allocator scope.");
                valid = false;
            }

            return (!valid) ? nullptr : reinterpret_cast<StackHeader const*>(block_header);
        }

        /// Get the capacity of the given memory block.
        usize capacity_of(u8 const* mem) const noexcept {
            StackHeader const* header = this->header_of(mem);
            return (header == nullptr) ? 0 : header->capacity;
        }

        /// Get the previous offset of the given memory block.
        usize previous_offset_of(u8 const* mem) const noexcept {
            auto* const header = this->header_of(mem);
            return (header == nullptr) ? 0 : header->previous_offset;
        }

        /// Allocates a new block of memory.
        ///
        /// Parameters:
        ///     * `length`: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        T* alloc(usize length) noexcept {
            if (psh_unlikely(length == 0)) {
                return nullptr;
            }

            usize const new_block_size = sizeof(T) * length;
            u8* const   free_mem       = memory + offset;
            usize const padding        = padding_with_header(
                reinterpret_cast<uptr>(free_mem),
                alignof(T),
                sizeof(StackHeader),
                alignof(StackHeader));
            usize const required = padding + new_block_size;

            if (psh_unlikely(required > capacity - offset)) {
                psh_error_fmt(
                    "StackAlloc::alloc unable to allocate %zu bytes of memory (%zu bytes required "
                    "due to alignment and padding). The stack allocator has only %zu bytes "
                    "remaining.",
                    new_block_size,
                    required,
                    capacity - offset);
                return nullptr;
            }

            // Address to the start of the new block of memory.
            u8* new_block = free_mem + padding;

            // Write to the header associated with the new block of memory.
            StackHeader* new_header =
                reinterpret_cast<StackHeader*>(new_block - sizeof(StackHeader));
            new_header->padding         = padding;
            new_header->capacity        = new_block_size;
            new_header->previous_offset = previous_offset;

            // Update the stack offsets.
            previous_offset = offset + padding;
            offset += padding + new_block_size;

            return reinterpret_cast<T*>(new_block);
        }

        /// Allocate a new zeroed block of memory.
        ///
        /// Parameters:
        ///     * `length`: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        T* zero_alloc(usize length) noexcept {
            T* ptr = alloc<T>(length);
            std::memset(ptr, 0, length);
            return ptr;
        }

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
        T* realloc(T const* block, usize new_length) noexcept {
            u8 const* ublock = reinterpret_cast<u8 const*>(block);

            if (psh_unlikely(new_length == 0)) {
                this->clear_at(ublock);
                return nullptr;
            }

            usize const new_size = sizeof(T) * new_length;

            // If `ptr` is the last allocated block, just adjust the offsets.
            if (ublock == this->top()) {
                offset = previous_offset + new_size;
                return block;
            }

            // Check if the address is within the allocator's memory.
            if (psh_unlikely((ublock < memory) || (ublock >= memory + capacity))) {
                psh_error("StackAlloc::realloc called with a pointer outside of the memory region "
                          "managed by the stack allocator.");
                return nullptr;
            }

            // Check if the address is already free.
            if (psh_unlikely(ublock >= memory + offset)) {
                psh_error("StackAlloc::realloc called with a free block of memory (use-after-free "
                          "error).");
                return nullptr;
            }

            StackHeader const* header =
                reinterpret_cast<StackHeader const*>(ublock - sizeof(StackHeader));

            // Check memory availability.
            if (psh_unlikely(new_size > capacity - offset)) {
                psh_error_fmt(
                    "StackAlloc::realloc cannot reallocate memory from size %zu to %zu. Only %zu "
                    "bytes of memory remaining.",
                    header->capacity,
                    new_size,
                    capacity - offset);
                return nullptr;
            }

            T* new_mem = alloc<T>(new_length);

            usize const copy_size = psh_min(header->capacity, new_size);
            memory_copy(
                reinterpret_cast<u8*>(new_mem),
                reinterpret_cast<u8 const*>(block),
                copy_size);

            return new_mem;
        }

        /// Tries to pop the last memory block allocated by the given stack.
        ///
        /// This function won't panic if the stack is empty, it will simply return false.
        Status pop() noexcept {
            if (psh_unlikely(previous_offset == 0)) {
                return Status::Failed;
            }

            u8 const*          top = memory + previous_offset;
            StackHeader const* top_header =
                reinterpret_cast<StackHeader const*>(top - sizeof(StackHeader));

            offset          = previous_offset - top_header->padding;
            previous_offset = top_header->previous_offset;
            return Status::OK;
        }

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
        Status clear_at(u8 const* block) noexcept {
            if (psh_unlikely(block == nullptr)) {
                return Status::Failed;
            }

            // Check if the block is within the allocator's memory.
            if (psh_unlikely((block < memory) || (block > memory + previous_offset))) {
                strptr fail_reason =
                    (block > memory + capacity)
                        ? "StackAlloc::free_at called with a pointer outside of the stack "
                          "allocator memory region."
                        : "StackAlloc::free_at called with a pointer to an already free region of "
                          "the stack allocator memory.";
                psh_error(fail_reason);
                return Status::Failed;
            }

            StackHeader const* header =
                reinterpret_cast<StackHeader const*>(block - sizeof(StackHeader));

            offset = wrap_sub(
                wrap_sub(reinterpret_cast<uptr>(block), header->padding),
                reinterpret_cast<uptr>(memory));
            previous_offset = header->previous_offset;

            return Status::OK;
        }

        /// Reset the allocator's offset.
        void clear() noexcept {
            offset          = 0;
            previous_offset = 0;
        }
    };
}  // namespace psh
