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
/// Description: A memory manager that is capable of managing the whole allocation system of an
///              application.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#pragma once

#include <psh/arena.h>
#include <psh/mem_utils.h>
#include <psh/option.h>
#include <psh/stack.h>
#include <psh/types.h>

namespace psh {
    /// A stack allocator manager that can be used as the central memory resource of an application.
    ///
    /// Note: any method returning a pointer should be checked for nullity, since allocation may
    ///       fail.
    struct MemoryManager {
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
        void reset() noexcept;
    };

    template <typename T>
    T* MemoryManager::alloc(usize length) noexcept {
        T* const new_mem = allocator.alloc<T>(length);
        if (new_mem != nullptr) {
            ++allocation_count;
        }
        return new_mem;
    }

    template <typename T>
    T* MemoryManager::realloc(T* block, usize new_length) noexcept {
        T* const new_mem = allocator.realloc<T>(block, new_length);
        if (new_mem != block) {
            allocation_count += 1;
        }
        return new_mem;
    }

}  // namespace psh
