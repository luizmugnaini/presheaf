/// Implementation of the memory_manager.h header file.
///
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#include <psh/memory_manager.h>

#include <psh/arena.h>
#include <psh/stack.h>
#include <psh/types.h>

#include <cstdlib>

namespace psh {
    MemoryManager::MemoryManager(usize _capacity) noexcept
        : capacity{_capacity},
          allocator{reinterpret_cast<u8*>(std::malloc(_capacity)), _capacity} {}

    MemoryManager::~MemoryManager() noexcept {
        std::free(allocator.memory);
    }

    void MemoryManager::init(usize _capacity) noexcept {
        allocator.init(reinterpret_cast<u8*>(std::malloc(_capacity)), _capacity);
    }

    Option<Arena> MemoryManager::make_arena(usize size) noexcept {
        u8* mem = alloc<u8>(size);
        if (mem == nullptr) {
            return {};
        }
        return Option<Arena>{Arena{mem, size}};
    }

    bool MemoryManager::pop() noexcept {
        usize const previous_offset = allocator.offset;
        if (!allocator.pop()) {
            return false;
        }

        used_memory = wrap_sub(used_memory, wrap_sub(previous_offset, allocator.offset));
        --allocation_count;

        return true;
    }

    bool MemoryManager::clear_until(u8 const* block) noexcept {
        // Check if the block lies within the allocator's memory.
        u8 const* const memory = allocator.memory;
        if (block < memory || block > memory + allocator.previous_offset) {
            if (block > memory + allocator.capacity) {
                log(LogLevel::Error,
                    "MemoryManager::clear_until called with a pointer outside of the stack "
                    "allocator memory region.");
                return false;
            }

            log(LogLevel::Error,
                "MemoryManager::clear_until called with a pointer to an already free region "
                "of the stack allocator memory.");
            return false;
        }

        // Pop the top memory block until popping `block` or reaching the end of the allocator.
        for (;;) {
            u8 const* top_block = allocator.top();
            if (top_block == nullptr) {
                break;
            }

            usize const last_offset = allocator.offset;
            psh_discard(allocator.pop());

            used_memory = wrap_sub(used_memory, wrap_sub(last_offset, allocator.offset));
            --allocation_count;

            // This is never true if the user gives an incorrect address.
            if (top_block == block) {
                break;
            }
        }

        return true;
    }

    void MemoryManager::reset() noexcept {
        // Zero all but `allocator.memory`.
        capacity                  = 0;
        allocation_count          = 0;
        used_memory               = 0;
        allocator.capacity        = 0;
        allocator.offset          = 0;
        allocator.previous_offset = 0;
    }
}  // namespace psh
