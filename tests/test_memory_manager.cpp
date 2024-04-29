/// Tests for the memory manager system.
///
/// This test should be ran with sanitizer flags on in order to detect possible memory leaks that
/// may go unseen.
///
/// @author Luiz G. Mugnaini. A. <luizmugnaini@gmail.com>
#include <psh/arena.h>
#include <psh/assert.h>
#include <psh/dyn_array.h>
#include <psh/math.h>
#include <psh/mem_utils.h>
#include <psh/memory_manager.h>
#include <psh/stack.h>
#include <psh/types.h>

#include "utils.h"

using namespace psh;

void zeroed_at_initialization() {
    StrPtr const        header = "[zeroed_at_initialization]";
    MemoryManager const memory_manager{1024};

    // Check validity.
    void* const mem_ptr = memory_manager.raw_memory();
    psh_assert(mem_ptr != nullptr);

    psh_assert(memory_manager.used_memory == static_cast<i64>(0));
    psh_assert(memory_manager.allocation_count == 0ull);

    // Check allocator.
    psh_assert(memory_manager.allocator.memory == reinterpret_cast<u8*>(mem_ptr));
    psh_assert(memory_manager.allocator.offset == 0ull);
    psh_assert(memory_manager.allocator.previous_offset == 0ull);
    psh_assert(memory_manager.allocator.capacity == static_cast<usize>(1024));
    log_passed(header);
}

void initialization_and_shutdown() {
    StrPtr const  header                  = "[initialization_and_shutdown]";
    usize const   memory_manager_capacity = 2048;
    MemoryManager memory_manager{memory_manager_capacity};
    uptr const    mem_sys_alloc_mem_actual_addr =
        reinterpret_cast<uptr>(memory_manager.allocator.memory);

    // Acquire a block of memory and write to it.
    usize const block_length = 60;
    u64* const  block        = memory_manager.alloc<u64>(block_length);
    psh_assert(block != nullptr);

    // Write to the block.
    u64 const start_value = 1283;
    for (usize idx = 0; idx < block_length; ++idx) {
        block[idx] = static_cast<u64>(idx) * start_value;
    }

    // Ensure the addresses where correctly calculated.
    uptr const block_actual_addr = reinterpret_cast<uptr>(block);
    uptr const block_via_allocator_actual_addr =
        mem_sys_alloc_mem_actual_addr + memory_manager.allocator.previous_offset;
    uptr const block_expected_addr = mem_sys_alloc_mem_actual_addr + sizeof(StackHeader);
    psh_assert(block_actual_addr == block_expected_addr);
    psh_assert(block_via_allocator_actual_addr == block_expected_addr);

    psh_assert_msg(
        memory_manager.allocation_count == static_cast<usize>(1),
        "Expected memory stats to account for a single memory allocation count.");

    // Manually read each address to check if the data of block got written to the right location.
    usize const block_stride = sizeof(u64);
    for (usize idx = 0; idx < block_length; ++idx) {
        auto const       expected = static_cast<u64>(idx) * start_value;
        u64 const* const actual = reinterpret_cast<u64*>(block_expected_addr + idx * block_stride);
        psh_assert(*actual == expected);
    }

    log_passed(header);
}

void memory_statistics() {
    StrPtr const header = "[memory_statistics]";

    /** Expected statistics. **/

    constexpr usize expected_string_at_least =
        sizeof(char) * (40 + 34 + 55) + 3 * sizeof(StackHeader);
    constexpr usize expected_misc_at_least =
        sizeof(char) * 90 + sizeof(u64) * 72 + 2 * sizeof(StackHeader);
    constexpr usize arena_data_size           = sizeof(usize) * 33 + sizeof(f32) * 45;
    constexpr usize expected_arena            = sizeof(StackHeader) + arena_data_size;
    constexpr usize expected_allocation_count = 6;  // NOTE: 3 strings, 1 arena, 2 misc.
    constexpr usize expected_total_at_least =
        expected_arena + expected_string_at_least + expected_misc_at_least;

    /** Create the manager. **/

    MemoryManager memory_manager{2048};

    /**
     * Allocate memory.
     *
     * Expected final layout of the stack allocator:
     *
     * | hdr + (b  and  d) | hdr + a | hdr + c | hdr + e | hdr + f | hdr + g |
     * ^                   ^
     * |      arena        |
     *
     * where "hdr" stands for the associated `StackAllocHeader` instance.
     */

    Arena arena{memory_manager.alloc<u8>(arena_data_size), arena_data_size};
    usize used = memory_manager.used_memory;
    psh_assert(used == expected_arena);
    psh_assert(memory_manager.allocation_count == static_cast<usize>(1));

    StrPtr const a = memory_manager.alloc<char>(40);
    psh_assert(a != nullptr);
    usize const a_size = wrap_sub(memory_manager.used_memory, used);
    used               = memory_manager.used_memory;
    psh_assert(memory_manager.allocation_count == static_cast<usize>(2));

    auto const b = DynArray<usize>(&arena, 33);
    psh_discard(b);
    constexpr usize b_size = sizeof(usize) * 33;
    psh_assert(
        memory_manager.used_memory == used);  // The manager shouldn't have a bump in its usage.
    psh_assert(arena.offset == b_size);       // Expect no alignment from first allocation
    psh_assert(memory_manager.allocation_count == static_cast<usize>(2));

    StrPtr const c = memory_manager.alloc<char>(34);
    psh_assert(c != nullptr);
    usize const c_size = wrap_sub(memory_manager.used_memory, used);
    used               = memory_manager.used_memory;
    psh_assert(memory_manager.allocation_count == static_cast<usize>(3));

    auto const d = DynArray<f32>(&arena, 45);
    psh_discard(d);
    constexpr usize d_size = sizeof(f32) * 45;
    psh_assert(memory_manager.used_memory == used);  // Expect no change in the manager.
    psh_assert(arena.offset >= b_size + d_size);     // Alignment may have happened
    psh_assert(memory_manager.allocation_count == static_cast<usize>(3));

    StrPtr const e = memory_manager.alloc<char>(90);
    psh_assert(e != nullptr);
    usize const e_size = wrap_sub(memory_manager.used_memory, used);
    used               = memory_manager.used_memory;
    psh_assert(memory_manager.allocation_count == static_cast<usize>(4));

    StrPtr f = memory_manager.alloc<char>(55);
    psh_assert(f != nullptr);
    usize const f_size = wrap_sub(memory_manager.used_memory, used);
    used               = memory_manager.used_memory;
    psh_assert(memory_manager.allocation_count == static_cast<usize>(5));

    u64* const g = memory_manager.alloc<u64>(72);
    psh_assert(g != nullptr);
    usize const g_size = wrap_sub(memory_manager.used_memory, used);
    psh_assert(memory_manager.allocation_count == static_cast<usize>(6));

    psh_assert(memory_manager.used_memory >= static_cast<i64>(expected_total_at_least));

    /**
     * Free memory and check if the memory statistics is correctly updated.
     */

    usize const actual_used_memory = memory_manager.used_memory;

    // Free both `f` and `g`.
    memory_manager.clear_until(reinterpret_cast<u8 const*>(f));
    psh_assert(memory_manager.allocation_count == expected_allocation_count - 2);
    psh_assert(memory_manager.used_memory == wrap_sub(actual_used_memory, f_size + g_size));

    // Pop `e`.
    memory_manager.pop();
    psh_assert(memory_manager.allocation_count == expected_allocation_count - 3);
    psh_assert(
        memory_manager.used_memory == wrap_sub(actual_used_memory, f_size + g_size + e_size));

    // Pop `c`.
    memory_manager.pop();
    psh_assert(memory_manager.allocation_count == expected_allocation_count - 4);
    psh_assert(
        memory_manager.used_memory ==
        wrap_sub(actual_used_memory, f_size + g_size + e_size + c_size));

    // Free `a`.
    memory_manager.clear_until(reinterpret_cast<u8 const*>(a));
    psh_assert(memory_manager.allocation_count == expected_allocation_count - 5);
    psh_assert(
        memory_manager.used_memory ==
        wrap_sub(actual_used_memory, f_size + g_size + e_size + c_size + a_size));

    // Assert that the only thing left is the arena.
    psh_assert(memory_manager.used_memory == expected_arena);

    // Pop `arena`
    memory_manager.pop();
    psh_assert(memory_manager.allocation_count == 0ull);
    psh_assert(memory_manager.used_memory == 0ull);

    // TODO: in the future, check that the memory from the arena is correctly handled by the manager
    // when freeing `b` and `d`.

    log_passed(header);
}

int main() {
    zeroed_at_initialization();
    initialization_and_shutdown();
    memory_statistics();
    return 0;
}
