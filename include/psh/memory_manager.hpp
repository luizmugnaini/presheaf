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
/// Description: A memory manager that is capable of managing the whole allocation system of an
///              application.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/arena.hpp>
#include <psh/core.hpp>
#include <psh/option.hpp>
#include <psh/stack.hpp>

namespace psh {
    /// A stack allocator manager that can be used as the central memory resource of an application.
    ///
    /// Note: any method returning a pointer should be checked for nullity, since allocation may
    ///       fail.
    struct psh_api MemoryManager {
        usize allocation_count = 0;
        Stack allocator        = {};

        constexpr MemoryManager() noexcept = default;

        void init(usize capacity) noexcept;
        MemoryManager(usize capacity) noexcept;

        ~MemoryManager() noexcept;

        /// Make a new arena allocator with a given size.
        Option<Arena> make_arena(usize size) noexcept;

        /// Request a region of memory of a given type `T`.
        ///
        /// Parameters:
        ///     * count: Number of entities of type `T` that should fit in the requested memory
        ///                 region.
        template <typename T>
        T* alloc(usize count) noexcept;

        /// Reallocate a region of memory created by the manager.
        ///
        /// Parameters:
        ///     * `block`: Pointer to the memory block that should be reallocated.
        ///     * `new_count`: The new length that the memory block should have.
        template <typename T>
        T* realloc(T* block, usize new_length) noexcept;

        /// Try to free the last allocated block of memory.
        Status pop() noexcept;

        /// Try to reset the allocator offset until the specified memory block.
        ///
        /// Note: If the caller passes a pointer to a wrong address and we can't tell that easily,
        ///       the stack will be *completely cleaned*, beware!
        ///
        /// Parameters:
        ///     * `block`: Pointer to the memory block that should be freed (all blocks above
        ///                the given one will also be freed). If this pointer is null, outside of
        ///                the stack allocator buffer, or already free, the program return false and
        ///                won't panic.
        Status clear_until(u8 const* block) noexcept;

        /// Resets the manager by zeroing the memory offset and statistics.
        void clear() noexcept;

        // @NOTE: Required bullshit when compiling as a DLL since the compiler will require all
        //       standard member functions to be defined.
        MemoryManager& operator=(MemoryManager&) = delete;
    };

    template <typename T>
    T* MemoryManager::alloc(usize length) noexcept {
        T* const new_mem = this->allocator.alloc<T>(length);
        if (new_mem != nullptr) {
            ++this->allocation_count;
        }
        return new_mem;
    }

    template <typename T>
    T* MemoryManager::realloc(T* block, usize new_length) noexcept {
        T* const new_mem = this->allocator.realloc<T>(block, new_length);
        if (new_mem != block) {
            this->allocation_count += 1;
        }
        return new_mem;
    }
}  // namespace psh
