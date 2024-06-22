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
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#pragma once

#include <psh/assert.h>
#include <psh/intrinsics.h>
#include <psh/log.h>
#include <psh/math.h>
#include <psh/mem_utils.h>
#include <psh/types.h>
#include <cstring>

namespace psh {
    struct Arena;  // Forward declaration.

    /// Scratch arena.
    ///
    /// This allocator is used to save the state of the parent arena at creation time and
    /// restore the parent arena offset state at destruction time.
    ///
    /// You can nest many scratch arenas throughout different lifetimes by decoupling the
    /// current scratch arena into a new one.
    ///
    /// # Intended usage pattern
    ///
    /// ```cpp
    /// f32 do_temp_work_and_restore_arena(ScratchArena&& s) {
    ///    DynArray<f32> arr{s.arena, 3};
    ///    arr.push(4.0f);
    ///    arr.push(5.5f);
    ///    arr.push(20.5f);
    ///
    ///    f32 sum = 0;
    ///    for (f32 x : arr) sum += x;
    ///
    ///    return sum;
    /// }
    ///
    /// i32* alloc_in_scratch_lifetime(ScratchArena& s, usize count) {
    ///     return s.arena.alloc<i32>(count);
    /// }
    ///
    /// int main() {
    ///     Arena a{...};
    ///
    ///     u8* x = a.alloc<u8>(1024);  // `a` has offset 1024.
    ///     {
    ///         ScratchArena s = a.make_scratch();  // Record the offset 1024.
    ///
    ///         i32 y = s.arena.alloc<u8>(32);  // `a` has an offset of 1024 + 4 * 32 = 1152.
    ///
    ///         // Record the offset 1152 into a new scratch arena, pass it a function, which
    ///         // will possibly allocate. When the function returns, `a` will go back to the
    ///         // 1152 offset.
    ///         f32 result = do_temp_work_and_restore_arena(s.decouple());
    ///
    ///         // Pass `s` to a function that will allocate 100 * 4 = 400 bits, bumping the
    ///         // offset of `a` to 1152 + 400 = 1552.
    ///         i32* val = alloc_in_scratch_lifetime(s, 100);
    ///     }
    ///     // `a` goes back to having an offset of 1024.
    ///
    ///     return 0;
    /// }
    /// ```
    ///
    /// # Bad usage
    ///
    /// You should avoid the following usage pattern at all costs
    /// ```cpp
    /// f32* bad_alloc(ScratchArena const& s, usize count) {
    ///     return s.arena.alloc<f32>(count);
    /// }
    ///
    /// int main() {
    ///     Arena a{...}; // `a` has 0 offset.
    ///     {
    ///         ScratchArena s = a.make_scratch(); // Record the 0 offset.
    ///
    ///         u32 foo = s.arena.alloc<u32>(10); // `a` has offset 4 * 10 = 40.
    ///
    ///         // - Records the offset `40`.
    ///         // - Allocates 30 * 4 = 120 bits internally.
    ///         // - Restore the arena offset to `40` when returning.
    ///         // - The resulting `bad_ptr` can be overwritten by any subsequent allocation.
    ///         f32* bad_ptr = bad_alloc(s.decouple(), 30);
    ///
    ///         // Will overwrite the memory region "pertaining" to `bad_ptr`
    ///         i64* over_ptr s.arena.alloc<i64>(50);
    ///     }
    ///     // `a` is restored to offset 0.
    ///     return 0;
    /// }
    /// ```
    /// If you passed `s` directly to `bad_alloc` you wouldn't have this problem because
    /// `bad_alloc` wouldn't restore the arena offset when returning and the arena would have an
    /// offset of 40 + 120 = 160. In order to mitigate this usage pattern, you should never have
    /// a function signature accepting `ScratchArena const&` as a parameter, since the callee
    /// may pass the scratch as an rvalue. In order to truly solve the above code problem, you
    /// should substitute `ScratchArena const&` of `bad_alloc` by `ScratchArena&` since this
    /// prohibits passing the scratch arena by rvalue.
    ///
    /// In general, your options will then be to pass as `ScratchArena&` or `ScratchArena&&`.
    struct ScratchArena {
        Arena* arena;
        usize  saved_offset = 0;

        ScratchArena(Arena* arena) noexcept;
        ~ScratchArena() noexcept;

        /// Create a new scratch arena with the current state of the parent.
        ScratchArena decouple() const noexcept;
    };

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
        u8*   buf    = nullptr;  ///< Not-owned memory block managed by the arena allocator.
        usize size   = 0;        ///< The capacity, in bytes, of the memory block.
        usize offset = 0;        ///< The current offset to the free-space in the memory block.

        // -----------------------------------------------------------------------------
        // - Allocation methods -
        // -----------------------------------------------------------------------------

        /// Allocates a block of memory that fits a given number of entities of type `T`.
        ///
        /// Parameters:
        ///     * count: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        T* alloc(usize count) noexcept;

        /// Allocate a zeroed block of memory.
        ///
        /// Allocates a block of memory of a given size for a given type, accounting for the
        /// alignment needed for the given type `T`. Additionally the block is zeroed before
        /// returning to the caller.
        ///
        /// Parameters:
        ///     * count: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        T* zero_alloc(usize count) noexcept;

        /// Reallocate a block of memory of a given type.
        ///
        /// The memory alignment required for the type `T` is automatically handled for the user.
        ///
        /// Parameters:
        ///     * block: Pointer to the start of the memory block to be resized.
        ///     * current_count: Current number of entities of type `T` that `block` is capable
        ///       of holding.
        ///     * new_count: Number of entities of type `T` that the new memory block should be
        ///       able to contain.
        template <typename T>
        T* realloc(T* block, usize current_count, usize new_count) noexcept;

        // -----------------------------------------------------------------------------
        // - Temporary memory management -
        // -----------------------------------------------------------------------------

        /// Reset the offset of the allocator.
        void clear() noexcept;

        /// Create a new scratch arena with the current offset state.
        ScratchArena make_scratch() noexcept;
    };

    // -----------------------------------------------------------------------------
    // - Implementation of the arena allocation methods -
    // -----------------------------------------------------------------------------

    template <typename T>
    T* Arena::alloc(usize count) noexcept {
        if (psh_unlikely(count == 0 || size == 0)) {
            return nullptr;
        }

        uptr  memory_addr    = reinterpret_cast<uptr>(buf);
        uptr  new_block_addr = align_forward(memory_addr + offset, alignof(T));
        usize block_size     = sizeof(T) * count;

        // Check if there is enough memory.
        if (psh_unlikely(new_block_addr + block_size > size + memory_addr)) {
            psh_error_fmt(
                "ArenaAlloc::alloc unable to allocate %zu bytes of memory (%zu bytes required "
                "due to alignment). The allocator has only %zu bytes remaining.",
                block_size,
                wrap_sub(size + new_block_addr, (offset + memory_addr)),
                block_size - offset);
            return nullptr;
        }

        // Commit the new block of memory.
        offset = static_cast<usize>(block_size + new_block_addr - memory_addr);

        return reinterpret_cast<T*>(new_block_addr);
    }

    template <typename T>
    T* Arena::zero_alloc(usize count) noexcept {
        T* const ptr = alloc<T>(count);
        std::memset(ptr, 0, count);
        return ptr;
    }

    template <typename T>
    T* Arena::realloc(T* block, usize current_count, usize new_count) noexcept {
        // Check if there is any memory at all.
        if (psh_unlikely(size == 0 || buf == nullptr || new_count == 0)) {
            return nullptr;
        }

        // Check if the user wants to allocate a completely new block.
        if (psh_unlikely(block == nullptr || current_count == 0)) {
            return alloc<T>(new_count);
        }

        u8* block_bytes = reinterpret_cast<u8*>(block);
        u8* mem_end     = buf + size;
        u8* free_mem    = buf + offset;

        // Check if the block lies within the allocator's memory.
        if (psh_unlikely((block_bytes < buf) || (block_bytes >= buf + size))) {
            psh_error("ArenaAlloc::realloc called with pointer outside of its domain.");
            return nullptr;
        }

        // Check if the block is already free.
        if (psh_unlikely(block_bytes >= free_mem)) {
            psh_error("ArenaAlloc::realloc called with a pointer to a free address of the arena "
                      "domain.");
            return nullptr;
        }

        usize current_block_size = sizeof(T) * current_count;
        usize new_block_size     = sizeof(T) * new_count;

        psh_assert_msg(
            current_block_size <= offset,
            "Arena::realloc called with current_block_size surpassing the current offset of the "
            "arena, which isn't possible");

        // If the block is the last allocated, just bump the offset.
        if (block_bytes == free_mem - current_block_size) {
            // Check if there is enough space.
            if (psh_unlikely(block_bytes + new_block_size > mem_end)) {
                psh_error_fmt(
                    "ArenaAlloc::realloc unable to reallocate block from %zu bytes to %zu "
                    "bytes.",
                    current_block_size,
                    new_block_size);
                return nullptr;
            }

            offset = static_cast<usize>(
                static_cast<isize>(offset) +
                static_cast<isize>(new_block_size - current_block_size));
            return block;
        }

        u8* new_block = zero_alloc<u8>(new_count);

        // Copy the existing data to the new block.
        usize copy_size = psh_min(current_block_size, new_block_size);
        memory_copy(new_block, block_bytes, copy_size);

        return reinterpret_cast<T*>(new_block);
    }
}  // namespace psh
