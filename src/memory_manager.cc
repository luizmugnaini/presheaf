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

#include <psh/arena.h>
#include <psh/stack.h>
#include <psh/types.h>

#include <cstdlib>

namespace psh {
    void MemoryManager::init(usize capacity) noexcept {
        allocator.init(reinterpret_cast<u8*>(std::malloc(capacity)), capacity);
    }

    MemoryManager::MemoryManager(usize capacity) noexcept {
        this->init(capacity);
    }

    MemoryManager::~MemoryManager() noexcept {
        std::free(allocator.memory);
    }

    Option<Arena> MemoryManager::make_arena(usize size) noexcept {
        u8* mem = alloc<u8>(size);
        return mem == nullptr ? Option<Arena>{} : Option{Arena{mem, size}};
    }

    bool MemoryManager::pop() noexcept {
        bool result = allocator.pop();
        if (psh_likely(result)) {
            --allocation_count;
        }
        return result;
    }

    bool MemoryManager::clear_until(u8 const* block) noexcept {
        u8 const* const mem_start = allocator.memory;

        // Check if the block lies within the allocator's memory.
        if (psh_unlikely((block < mem_start) || (block > mem_start + allocator.previous_offset))) {
            if (block > mem_start + allocator.capacity) {
                log(LogLevel::Error,
                    "MemoryManager::clear_until called with a pointer outside of the stack "
                    "mem_start region.");
                return false;
            }

            log(LogLevel::Error,
                "MemoryManager::clear_until called with a pointer to an already free region "
                "of the stack mem_start.");
            return false;
        }

        // Pop the top memory block until popping `block` or reaching the end of the allocator.
        //
        // NOTE: If we were given the incorrect address, we end up clearing the whole memory.
        for (;;) {
            u8 const* top_block = allocator.top();
            if (psh_unlikely(top_block == mem_start)) {
                break;
            }

            if (psh_likely(allocator.pop())) {
                --allocation_count;
            }

            if (psh_unlikely(top_block == block)) {
                break;
            }
        }

        return true;
    }

    void MemoryManager::reset() noexcept {
        u8* mem = allocator.memory;
        zero_struct(this);
        allocator.memory = mem;
    }
}  // namespace psh
