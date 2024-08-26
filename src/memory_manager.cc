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
