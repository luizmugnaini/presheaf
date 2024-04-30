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
        usize capacity         = 0;
        usize allocation_count = 0;
        usize used_memory      = 0;
        Stack allocator{};

        /// Make a default empty memory manager.
        explicit constexpr MemoryManager() noexcept = default;

        /// Make and initialize a memory manager.
        explicit MemoryManager(usize capacity) noexcept;

        ~MemoryManager() noexcept;

        /// Initialize an empty memory manager.
        void init(usize capacity) noexcept;

        /// Get a raw pointer to the block of memory administrated by the memory manager.
        constexpr u8* raw_memory() const noexcept {
            return allocator.memory;
        }

        /// Make a new arena allocator with a given size.
        Option<Arena> make_arena(usize size) noexcept;

        /// Request a region of memory of a given type `T`.
        ///
        /// Parameters:
        ///     * `length`: Number of entities of type `T` that should fit in the requested memory
        ///                 region.
        template <typename T>
        T* alloc(usize length) noexcept {
            if (length == 0) {
                return nullptr;
            }

            usize const previous_offset = allocator.offset;
            T* const    new_mem         = allocator.alloc<T>(length);
            if (new_mem == nullptr) {
                log_fmt(
                    LogLevel::Error,
                    "MemoryManager::alloc unable to allocate %zu bytes.",
                    sizeof(T) * length);
                return nullptr;
            }

            ++allocation_count;
            used_memory += wrap_sub(allocator.offset, previous_offset);

            return new_mem;
        }

        /// Reallocate a region of memory created by the manager.
        ///
        /// Parameters:
        ///     * `block`: Pointer to the memory block that should be reallocated.
        ///     * `new_length`: The new length that the memory block should have. (i.e. the
        ///                     new memory block should be able to contain `new_length` entities of
        ///                     type `T`).
        template <typename T>
        T* realloc(T* block, usize new_length) noexcept {
            usize const last_offset = allocator.offset;

            T* const new_mem = allocator.realloc<T>(block, new_length);
            if (new_mem == nullptr) {
                log_fmt(
                    LogLevel::Error,
                    "MemoryManager::realloc unable to allocate %zu bytes.",
                    sizeof(T) * new_length);
                return nullptr;
            }

            used_memory += wrap_sub(allocator.offset, last_offset);

            // A new allocation occurred only if the pointer's address differs.
            if (new_mem != block) {
                allocation_count += 1;
            }

            return new_mem;
        }

        /// Try to free the last allocated block of memory.
        bool pop() noexcept;

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
        bool clear_until(u8 const* block) noexcept;

        /// Resets the manager by zeroing the memory offset and statistics.
        void reset() noexcept;
    };
}  // namespace psh
