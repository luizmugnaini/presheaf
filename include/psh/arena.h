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
/// Description: Arena memory allocator, also known as a linear allocator.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>
#pragma once

#include <psh/assert.h>
#include <psh/intrinsics.h>
#include <psh/io.h>
#include <psh/math.h>
#include <psh/mem_utils.h>
#include <psh/types.h>
#include <cstring>

namespace psh {
    /// Arena allocator
    ///
    /// The arena allocator is great for the management of temporary allocation of memory, since an
    /// allocation takes nothing more than incrementing an offset.
    ///
    /// Note:
    ///     * The arena does not own memory, thus it is not responsible for the freeing of it.
    ///     * Any functions that return a raw pointer should be checked for nullity since allocation
    ///       may fail.
    struct Arena {
        u8*   memory   = nullptr;  ///< Not-owned memory block managed by the arena allocator.
        usize offset   = 0;        ///< The current offset to the free-space in the memory block.
        usize capacity = 0;        ///< The capacity, in bytes, of the memory block.

        constexpr explicit Arena() noexcept = default;

        /// Create an arena with a given block of memory and capacity.
        explicit Arena(u8* _memory, usize _capacity) noexcept
            : memory{_memory}, capacity{_capacity} {
            if (psh_likely(capacity != 0)) {
                psh_assert_msg(
                    memory != nullptr,
                    "Arena created with inconsistent data: non-zero capacity but null memory");
            }
        }

        /// Reset the offset of the allocator.
        inline void clear() noexcept {
            offset = 0;
        }

        /// Allocates a block of memory that fits a given number of entities of type `T`.
        ///
        /// Parameters:
        ///     * length: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        T* alloc(usize length) noexcept {
            if (psh_unlikely(length == 0 || capacity == 0)) {
                return nullptr;
            }

            uptr const  memory_addr    = reinterpret_cast<uptr>(memory);
            uptr const  new_block_addr = align_forward(memory_addr + offset, alignof(T));
            usize const size           = sizeof(T) * length;

            // Check if there is enough memory.
            if (psh_unlikely(new_block_addr + size > capacity + memory_addr)) {
                psh_error_fmt(
                    "ArenaAlloc::alloc unable to allocate %zu bytes of memory (%zu bytes required "
                    "due to alignment). The allocator has only %zu bytes remaining.",
                    size,
                    wrap_sub(size + new_block_addr, (offset + memory_addr)),
                    capacity - offset);
                return nullptr;
            }

            // Commit the new block of memory.
            offset = static_cast<usize>(size + new_block_addr - memory_addr);

            return reinterpret_cast<T*>(new_block_addr);
        }

        /// Allocate a zeroed block of memory.
        ///
        /// Allocates a block of memory of a given size for a given type, accounting for the
        /// alignment needed for the given type `T`. Additionally the block is zeroed before
        /// returning to the caller.
        ///
        /// Parameters:
        ///     * length: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        T* zero_alloc(usize length) noexcept {
            T* const ptr = alloc<T>(length);
            std::memset(ptr, 0, length);
            return ptr;
        }

        /// Reallocate a block of memory of a given type.
        ///
        /// The memory alignment required for the type `T` is automatically handled for the user.
        ///
        /// Parameters:
        ///     * block: Pointer to the start of the memory block to be resized.
        ///     * current_capacity: Current number of entities of type `T` that `block` is capable
        ///       of holding.
        ///     * new_capacity: Number of entities of type `T` that the new memory block should be
        ///       able to contain.
        template <typename T>
        T* realloc(T* block, usize current_capacity, usize new_capacity) noexcept {
            // Check if there is any memory at all.
            if (psh_unlikely(capacity == 0 || memory == nullptr || new_capacity == 0)) {
                return nullptr;
            }

            // Check if the user wants to allocate a completely new block.
            if (psh_unlikely(block == nullptr || current_capacity == 0)) {
                return alloc<T>(new_capacity);
            }

            u8* const block_bytes = reinterpret_cast<u8*>(block);
            u8* const mem_end     = memory + capacity;
            u8* const free_mem    = memory + offset;

            // Check if the block lies within the allocator's memory.
            if (psh_unlikely((block_bytes < memory) || (block_bytes >= memory + capacity))) {
                psh_error("ArenaAlloc::realloc called with pointer outside of its domain.");
                return nullptr;
            }

            // Check if the block is already free.
            if (psh_unlikely(block_bytes >= free_mem)) {
                psh_error(
                    "ArenaAlloc::realloc called with a pointer to a free address of the arena "
                    "domain.");
                return nullptr;
            }

            usize const current_size = sizeof(T) * current_capacity;
            usize const new_size     = sizeof(T) * new_capacity;

            psh_assert_msg(
                current_size <= offset,
                "Arena::realloc called with current_size surpassing the current offset of the "
                "arena, which isn't possible");

            // If the block is the last allocated, just bump the offset.
            if (block_bytes == free_mem - current_size) {
                // Check if there is enough space.
                if (psh_unlikely(block_bytes + new_size > mem_end)) {
                    psh_error_fmt(
                        "ArenaAlloc::realloc unable to reallocate block from %zu bytes to %zu "
                        "bytes.",
                        current_size,
                        new_size);
                    return nullptr;
                }

                offset = static_cast<usize>(
                    static_cast<isize>(offset) + static_cast<isize>(new_size - current_size));
                return block;
            }

            auto* const new_mem = zero_alloc<u8>(new_capacity);

            // Copy the existing data to the new block.
            usize const copy_size = psh_min(current_size, new_size);
            memory_copy(new_mem, block_bytes, copy_size);

            return reinterpret_cast<T*>(new_mem);
        }

        /// Scratch arena.
        ///
        /// This allocator is used to save the state of the parent arena at creation time and
        /// restore the parent arena offset state at destruction time.
        ///
        /// You can nest many scratch arenas throughout different lifetimes by decoupling the
        /// current scratch arena into a new one.
        struct Scratch {
            Arena* arena;
            usize  saved_offset = 0;

            Scratch(Scratch&&) = default;

            /// Create a new scratch arena out of an existing arena.
            constexpr explicit Scratch(Arena* parent) noexcept
                : arena{parent}, saved_offset{parent->offset} {}

            /// Reset the parent offset.
            ~Scratch() noexcept {
                arena->offset = saved_offset;
            }

            /// Create a new scratch arena with the current state of the parent.
            Scratch decouple() const noexcept {
                return Scratch{arena};
            }

            // The scratch arena should never be passed around by reference because it defeats its
            // purpose. You should always opt for `Scratch::decouple` in such cases.
            Scratch()                          = delete;
            Scratch(Scratch&)                  = delete;
            Scratch(Scratch const&)            = delete;
            Scratch& operator=(Scratch)        = delete;
            Scratch& operator=(Scratch&)       = delete;
            Scratch& operator=(Scratch const&) = delete;
        };

        /// Create a new scratch arena.
        Scratch make_scratch() noexcept {
            return Scratch{this};
        }
    };

    using ScratchArena = Arena::Scratch;
}  // namespace psh
