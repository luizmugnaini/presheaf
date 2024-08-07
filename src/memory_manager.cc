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
/// Description: Implementation of the memory allocation manager.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/memory_manager.h>

#include <cstdlib>
#include <cstring>

namespace psh {
    void MemoryManager::init(usize capacity) noexcept {
        this->allocator.init(reinterpret_cast<u8*>(psh_malloc(capacity)), capacity);
    }

    MemoryManager::MemoryManager(usize capacity) noexcept {
        this->init(capacity);
    }

    MemoryManager::~MemoryManager() noexcept {
        psh_free(this->allocator.buf);
    }

    Option<Arena> MemoryManager::make_arena(usize size) noexcept {
        u8* mem = alloc<u8>(size);
        return (mem == nullptr) ? Option<Arena>{} : Option<Arena>{Arena{mem, size}};
    }

    Status MemoryManager::pop() noexcept {
        Status st = this->allocator.pop();
        if (psh_likely(st == Status::OK)) {
            --this->allocation_count;
        }
        return st;
    }

    Status MemoryManager::clear_until(u8 const* block) noexcept {
        u8 const* mem_start = this->allocator.buf;

        // Check if the block lies within the allocator's memory.
        if (psh_unlikely((block < mem_start) || (block > mem_start + this->allocator.previous_offset))) {
            strptr fail_reason =
                (block > mem_start + this->allocator.capacity)
                    ? "MemoryManager::clear_until called with a pointer outside of the stack "
                      "mem_start region."
                    : "MemoryManager::clear_until called with a pointer to an already free region "
                      "of the stack mem_start.";
            psh_error(fail_reason);
            return Status::FAILED;
        }

        // Pop the top memory block until popping `block` or reaching the end of the allocator.
        //
        // NOTE: If we were given the incorrect address, we end up clearing the whole memory.
        for (;;) {
            u8 const* top_block = this->allocator.top();
            if (psh_unlikely(top_block == mem_start)) {
                break;
            }
            if (psh_likely(this->allocator.pop() == Status::OK)) {
                --this->allocation_count;
            }
            if (psh_unlikely(top_block == block)) {
                break;
            }
        }

        return Status::OK;
    }

    void MemoryManager::clear() noexcept {
        this->allocation_count = 0;
        this->allocator.clear();
    }
}  // namespace psh
