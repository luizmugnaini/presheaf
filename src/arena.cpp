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
        psh_error_fmt(                                                                   \
            "Arena unable to allocate %zu bytes (with %u bytes of alignment) of memory." \
            " The allocator has only %zu bytes remaining.",                              \
            requested_size,                                                              \
            requested_alignment,                                                         \
            arena->size - arena->offset);                                                \
    } while (0)

#define psh_impl_arena_is_empty(arena) (((arena)->size == 0) || ((arena)->buf == nullptr))

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
        if (psh_unlikely(new_block_addr + size_bytes > this->size + memory_addr)) {
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
        if (psh_unlikely(psh_impl_arena_is_empty(this))) {
            psh_impl_arena_report_out_of_memory(this, new_size_bytes, alignment);
            psh_impl_return_from_memory_error();
        }

        if (psh_unlikely(new_size_bytes == 0)) {
            psh_error_fmt(
                "Arena requested to reallocate block from %zu bytes to 0 bytes, which isn't possible.",
                current_size_bytes);
            psh_impl_return_from_memory_error();
        }

        if (psh_unlikely((block == nullptr) || (current_size_bytes == 0))) {
            return this->alloc_align(new_size_bytes, alignment);
        }

        u8* block_bytes = reinterpret_cast<u8*>(block);
        u8* mem_end     = this->buf + this->size;
        u8* free_mem    = this->buf + this->offset;

        // Check if the block lies within the allocator's memory.
        if (psh_unlikely((block_bytes < this->buf) || (block_bytes >= this->buf + this->size))) {
            psh_error("ArenaAlloc::realloc called with pointer outside of its domain.");
            psh_impl_return_from_memory_error();
        }

        // Check if the block is already free.
        if (psh_unlikely(block_bytes >= free_mem)) {
            psh_error("ArenaAlloc::realloc called with a pointer to a free address of the arena domain.");
            psh_impl_return_from_memory_error();
        }

        usize current_block_size = sizeof(u8) * current_size_bytes;
        usize new_block_size     = sizeof(u8) * new_size_bytes;

        if (psh_unlikely(current_size_bytes > this->offset)) {
            psh_error_fmt(
                "Arena::realloc called with current_block_size (%zu) surpassing the current offset (%zu) of the "
                "arena, which isn't possible",
                current_size_bytes,
                this->offset);
            psh_impl_return_from_memory_error();
        }

        // If the block is the last allocated, just bump the offset.
        if (block_bytes == free_mem - current_size_bytes) {
            // Check if there is enough space.
            if (psh_unlikely(block_bytes + new_size_bytes > mem_end)) {
                psh_error_fmt(
                    "ArenaAlloc::realloc unable to reallocate block from %zu bytes to %zu bytes.",
                    current_size_bytes,
                    new_size_bytes);
                psh_impl_return_from_memory_error();
            }

            this->offset = static_cast<usize>(
                static_cast<isize>(this->offset) +
                static_cast<isize>(new_block_size - current_block_size));
            return block;
        }

        u8* new_block = this->alloc_align(new_size_bytes, alignment);

        // Copy the existing data to the new block.
        usize copy_size = psh_min_val(current_block_size, new_block_size);
        memory_copy(new_block, block_bytes, copy_size);

        return new_block;
    }

    void Arena::clear() noexcept {
        this->offset = 0;
    }

    ArenaCheckpoint Arena::make_checkpoint() noexcept {
        return ArenaCheckpoint {
#if defined(PSH_DEBUG)
            .arena = this,
#endif
            .offset = this->offset,
        };
    }

    void Arena::restore_state(ArenaCheckpoint& checkpoint) noexcept {
#if defined(PSH_DEBUG)
        psh_assert_msg(
            checkpoint.arena == this,
            "Tried to restore the arena to a checkpoint of a distinct originating arena.");
        psh_assert_msg_fmt(
            checkpoint.offset <= this->offset,
            "Tried to restore an arena to an invalid checkpoint, you cannot restore the arena to an "
            "offset bigger than the current. Checkpoint offset: %zu, Arena current offset: %zu.",
            checkpoint.offset,
            this->offset);

        checkpoint.arena = nullptr;  // Invalidate the checkpoint for further uses.
#endif
        this->offset = checkpoint.offset;
    }

    ScratchArena::ScratchArena(Arena* parent) noexcept {
        if (psh_likely(parent != nullptr)) {
            this->arena        = parent;
            this->saved_offset = parent->offset;
        }
    }

    ScratchArena::~ScratchArena() noexcept {
        if (psh_likely(this->arena != nullptr)) {
            this->arena->offset = this->saved_offset;
        }
    }

    ScratchArena ScratchArena::decouple() const noexcept {
        return ScratchArena{this->arena};
    }

    ScratchArena Arena::make_scratch() noexcept {
        return ScratchArena{this};
    }
}  // namespace psh
