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
/// Description: Stack memory allocator.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/assert.hpp>
#include <psh/core.hpp>

namespace psh {
    /// Header associated with each memory block in the stack allocator.
    ///
    /// Memory layout:
    ///
    /// ```md
    ///           `previous_offset`                      |-`capacity`-|
    ///                  ^                               ^            ^
    ///                  |                               |            |
    ///  |previous header|previous memory|++++++++|header|   memory   |
    ///                                  ^               ^
    ///                                  |---`padding`---|
    /// ```
    ///
    /// where "header" represents this current header, and "memory" represents the memory block
    /// associated to this header.
    struct psh_api StackHeader {
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
    struct psh_api Stack {
        u8*   buf;
        usize capacity        = 0;
        usize offset          = 0;
        usize previous_offset = 0;

        constexpr Stack() = default;

        psh_inline void init(FatPtr<u8> memory) psh_noexcept {
            psh_assert_msg(this->capacity == 0, "Tried to re-initialize an initialized Stack.");

            this->buf             = memory.buf;
            this->capacity        = memory.count;
            this->offset          = 0;
            this->previous_offset = 0;
        }

        psh_inline Stack(FatPtr<u8> memory) psh_noexcept {
            this->init(memory);
        }

        // -----------------------------------------------------------------------------
        // Allocated memory information.
        // -----------------------------------------------------------------------------

        /// Gets the total size, in bytes, of the memory used by the allocator.
        usize used() const psh_noexcept;

        /// Gets a pointer to the memory of the last allocated memory block of the stack.
        u8* top() const psh_noexcept;

        /// Gets a pointer to the header associated to the top memory block of the stack.
        StackHeader const* top_header() const psh_noexcept;

        /// Get the size of the top memory block.
        usize top_size() const psh_noexcept;

        /// Get the previous offset of the top memory block.
        usize top_previous_offset() const psh_noexcept;

        /// Gets a pointer to the header associated to the given memory block.
        StackHeader const* header_of(u8 const* block) const psh_noexcept;

        /// Get the size of the given memory block.
        usize size_of(u8 const* mem) const psh_noexcept;

        /// Get the previous offset of the given memory block.
        usize previous_offset_of(u8 const* mem) const psh_noexcept;

        // -----------------------------------------------------------------------------
        // Allocation methods.
        //
        // @NOTE: All allocation procedures will zero-out the whole allocated block.
        // -----------------------------------------------------------------------------

        u8* alloc_align(usize size_bytes, u32 alignment) psh_noexcept;

        u8* realloc_align(u8* block, usize new_size_bytes, u32 alignment) psh_noexcept;

        /// Allocates a new block of memory.
        ///
        /// Parameters:
        ///     * `count`: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        psh_inline T* alloc(usize count) psh_noexcept {
            return reinterpret_cast<T*>(this->alloc_align(sizeof(T) * count, alignof(T)));
        }

        /// Reallocate a block of memory of a given type.
        ///
        /// Parameters:
        ///     * `block`: Pointer to the start of the memory block to be resized.
        ///     * `new_count`: Number of entities of type `T` that the new memory block should be
        ///                     able to contain.
        ///
        /// Note: If the new count is zero, we proceed to clean the whole stack up until the given
        ///       block.
        template <typename T>
        psh_inline T* realloc(T* block, usize new_count) psh_noexcept {
            return reinterpret_cast<T*>(this->realloc_align(block, sizeof(T) * new_count));
        }

        // -----------------------------------------------------------------------------
        // Memory manipulation utilities.
        // -----------------------------------------------------------------------------

        /// Tries to pop the last memory block allocated by the given stack.
        ///
        /// This function won't panic if the stack is empty, it will simply return false.
        Status pop() psh_noexcept;

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
        Status clear_at(u8 const* block) psh_noexcept;

        /// Reset the allocator's offset.
        psh_inline void clear() psh_noexcept {
            this->offset          = 0;
            this->previous_offset = 0;
        }
    };
}  // namespace psh
