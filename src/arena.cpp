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
/// Description: Implementation of the arena memory allocator.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/arena.hpp>

#include "impl_common.hpp"

#define psh_impl_arena_report_out_of_memory(arena, requested_size, requested_alignment)  \
    do {                                                                                 \
        psh_log_error_fmt(                                                               \
            "Arena unable to allocate %zu bytes (with %u bytes of alignment) of memory." \
            " The allocator has only %zu bytes remaining.",                              \
            requested_size,                                                              \
            requested_alignment,                                                         \
            arena->capacity - arena->offset);                                            \
    } while (0)

#define psh_impl_arena_is_empty(arena) (((arena)->capacity == 0) || ((arena)->buf == nullptr))

namespace psh {
    u8* Arena::alloc_align(usize size_bytes, u32 alignment) noexcept {
        if (psh_unlikely(size_bytes == 0)) {
            return nullptr;
        }
        if (psh_unlikely(psh_impl_arena_is_empty(this))) {
            psh_impl_arena_report_out_of_memory(this, size_bytes, alignment);
            psh_impl_return_from_memory_error();
        }

        // Check if there is enough memory.
        uptr memory_addr    = reinterpret_cast<uptr>(this->buf);
        uptr new_block_addr = align_forward(memory_addr + this->offset, alignment);
        if (psh_unlikely(new_block_addr + size_bytes > this->capacity + memory_addr)) {
            psh_impl_arena_report_out_of_memory(this, size_bytes, alignment);
            psh_impl_return_from_memory_error();
        }

        // Commit the new block of memory.
        this->offset = static_cast<usize>(size_bytes + new_block_addr - memory_addr);

        u8* new_block = reinterpret_cast<u8*>(new_block_addr);
        memory_set(new_block, size_bytes, 0);
        return new_block;
    }

    u8* Arena::realloc_align(
        u8*   block,
        usize current_size_bytes,
        usize new_size_bytes,
        u32   alignment) noexcept {
        psh_assert_msg(new_size_bytes != 0, "Don't use realloc to free blocks of memory.");
        psh_assert_msg((block != nullptr) && (current_size_bytes != 0), "Don't use realloc to allocate new memory.");

        if (psh_unlikely(psh_impl_arena_is_empty(this))) {
            psh_impl_arena_report_out_of_memory(this, new_size_bytes, alignment);
            psh_impl_return_from_memory_error();
        }

        // Avoid re-reading this-> multiple times.
        uptr  memory_addr      = reinterpret_cast<uptr>(this->buf);
        uptr  memory_end       = memory_addr + this->capacity;
        usize memory_offset    = this->offset;
        uptr  free_memory_addr = memory_addr + memory_offset;

        uptr block_addr = reinterpret_cast<uptr>(block);

        // Check if the block lies within the allocator's memory.
        if (psh_unlikely((block_addr < memory_addr) || (block_addr >= memory_end))) {
            psh_log_error("ArenaAlloc::realloc called with pointer outside of its domain.");
            psh_impl_return_from_memory_error();
        }

        // Check if the block is already free.
        if (psh_unlikely(block_addr >= free_memory_addr)) {
            psh_log_error("ArenaAlloc::realloc called with a pointer to a free address of the arena domain.");
            psh_impl_return_from_memory_error();
        }

        if (psh_unlikely(current_size_bytes > memory_offset)) {
            psh_log_error_fmt(
                "Arena::realloc called with current_block_size (%zu) surpassing the current offset (%zu) of the "
                "arena, which isn't possible",
                current_size_bytes,
                memory_offset);
            psh_impl_return_from_memory_error();
        }

        // If the block is the last allocated, just bump the offset.
        if (block_addr == free_memory_addr - current_size_bytes) {
            // Check if there is enough space.
            if (psh_unlikely(block_addr + new_size_bytes > memory_end)) {
                psh_log_error_fmt(
                    "Unable to reallocate block from %zu bytes to %zu bytes.",
                    current_size_bytes,
                    new_size_bytes);
                psh_impl_return_from_memory_error();
            }

            this->offset = static_cast<usize>(
                static_cast<isize>(memory_offset) +
                static_cast<isize>(new_size_bytes - current_size_bytes));
            return block;
        }

        // Allocate a new block and copy old memory.
        u8* new_block = this->alloc_align(new_size_bytes, alignment);
        memory_move(new_block, block, psh_min_value(current_size_bytes, new_size_bytes));

        return new_block;
    }
}  // namespace psh
