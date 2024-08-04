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
/// Description: Implementation of the arena memory allocator.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/arena.h>

namespace psh {
    // -----------------------------------------------------------------------------
    // - Arena implementation -
    // -----------------------------------------------------------------------------

    u8* Arena::alloc_align(usize size_bytes, u32 alignment) noexcept {
        if (psh_unlikely(size_bytes == 0 || this->size == 0)) {
            return nullptr;
        }

        uptr  memory_addr    = reinterpret_cast<uptr>(this->buf);
        uptr  new_block_addr = align_forward(memory_addr + this->offset, alignment);
        usize block_size     = sizeof(u8) * size_bytes;

        // Check if there is enough memory.
        if (psh_unlikely(new_block_addr + block_size > size + memory_addr)) {
            psh_error_fmt(
                "ArenaAlloc::alloc unable to allocate %zu bytes of memory (%zu bytes required "
                "due to alignment). The allocator has only %zu bytes remaining.",
                block_size,
                wrap_sub(this->size + new_block_addr, (this->offset + memory_addr)),
                block_size - this->offset);
            return nullptr;
        }

        // Commit the new block of memory.
        this->offset = static_cast<usize>(block_size + new_block_addr - memory_addr);

        return reinterpret_cast<u8*>(new_block_addr);
    }

    u8* Arena::zero_alloc_align(usize size_bytes, u32 alignment) noexcept {
        u8* const ptr = this->alloc_align(size_bytes, alignment);
        psh::memory_set(psh::FatPtr{ptr, size_bytes}, 0);
        return ptr;
    }

    u8* Arena::realloc_align(
        u8*   block,
        usize current_size_bytes,
        usize new_size_bytes,
        u32   alignment) noexcept {
        // Check if there is any memory at all.
        if (psh_unlikely((this->size == 0) || (this->buf == nullptr) || (new_size_bytes == 0))) {
            return nullptr;
        }

        // Check if the user wants to allocate a completely new block.
        if (psh_unlikely(block == nullptr) || (current_size_bytes == 0)) {
            return this->alloc_align(new_size_bytes, alignment);
        }

        u8* block_bytes = reinterpret_cast<u8*>(block);
        u8* mem_end     = this->buf + this->size;
        u8* free_mem    = this->buf + this->offset;

        // Check if the block lies within the allocator's memory.
        if (psh_unlikely((block_bytes < this->buf) || (block_bytes >= this->buf + this->size))) {
            psh_error("ArenaAlloc::realloc called with pointer outside of its domain.");
            return nullptr;
        }

        // Check if the block is already free.
        if (psh_unlikely(block_bytes >= free_mem)) {
            psh_error("ArenaAlloc::realloc called with a pointer to a free address of the arena "
                      "domain.");
            return nullptr;
        }

        usize current_block_size = sizeof(u8) * current_size_bytes;
        usize new_block_size     = sizeof(u8) * new_size_bytes;

        psh_assert_msg(
            current_block_size <= this->offset,
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

            this->offset = static_cast<usize>(
                static_cast<isize>(this->offset) +
                static_cast<isize>(new_block_size - current_block_size));
            return block;
        }

        u8* new_block = this->zero_alloc_align(new_size_bytes, alignment);

        // Copy the existing data to the new block.
        usize copy_size = psh_min(current_block_size, new_block_size);
        memory_copy(new_block, block_bytes, copy_size);

        return new_block;
    }

    void Arena::clear() noexcept {
        this->offset = 0;
        int x        = 0;
    }

    ScratchArena Arena::make_scratch() noexcept {
        return ScratchArena{this};
    }

    // -----------------------------------------------------------------------------
    // - Scratch arena implementation -
    // -----------------------------------------------------------------------------

    ScratchArena::ScratchArena(Arena* parent) noexcept {
        if (parent != nullptr) {
            this->arena        = parent;
            this->saved_offset = parent->offset;
        }
    }

    ScratchArena::~ScratchArena() noexcept {
        if (this->arena != nullptr) {
            this->arena->offset = this->saved_offset;
        }
    }

    ScratchArena ScratchArena::decouple() const noexcept {
        return ScratchArena{this->arena};
    }
}  // namespace psh
