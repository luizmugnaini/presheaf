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
/// Description: Tests for the memory manager system.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
///
/// This test should be ran with sanitizer flags on in order to detect possible memory leaks that
/// may go unseen.

#include <psh/memory_manager.h>

#include <psh/arena.h>
#include <psh/assert.h>
#include <psh/dyn_array.h>
#include <psh/intrinsics.h>
#include <psh/mem_utils.h>
#include <psh/stack.h>
#include <psh/types.h>
#include "utils.h"

using namespace psh;

void zeroed_at_initialization() {
    MemoryManager memory_manager{1024};

    // Check validity.
    void* mem_ptr = memory_manager.allocator.buf;
    psh_assert(mem_ptr != nullptr);

    psh_assert(memory_manager.allocator.offset == static_cast<i64>(0));
    psh_assert(memory_manager.allocation_count == 0ull);

    // Check allocator.
    psh_assert(memory_manager.allocator.buf == reinterpret_cast<u8*>(mem_ptr));
    psh_assert(memory_manager.allocator.offset == 0ull);
    psh_assert(memory_manager.allocator.previous_offset == 0ull);
    psh_assert(memory_manager.allocator.capacity == static_cast<usize>(1024));
    test_passed();
}

void initialization_and_shutdown() {
    usize         memory_manager_capacity = 2048;
    MemoryManager memory_manager{memory_manager_capacity};
    u8 const*     mem_sys_alloc_mem_actual_addr = memory_manager.allocator.buf;

    // Acquire a block of memory and write to it.
    usize block_length = 60;
    u64*  block        = memory_manager.alloc<u64>(block_length);
    psh_assert(block != nullptr);

    // Write to the block.
    u64 start_value = 1283;
    for (usize idx = 0; idx < block_length; ++idx) {
        block[idx] = idx * start_value;
    }

    // Ensure the addresses where correctly calculated.
    u64 const* block_expected_addr =
        reinterpret_cast<u64 const*>(mem_sys_alloc_mem_actual_addr + sizeof(StackHeader));
    psh_assert(block == block_expected_addr);
    u64 const* block_via_allocator_actual_addr = reinterpret_cast<u64 const*>(
        mem_sys_alloc_mem_actual_addr + memory_manager.allocator.previous_offset);
    psh_assert(block_via_allocator_actual_addr == block_expected_addr);

    psh_assert_msg(
        memory_manager.allocation_count == 1,
        "Expected memory stats to account for a single memory allocation count.");

    // Manually read each address to check if the data of block got written to the right location.
    usize block_stride = sizeof(u64);
    for (usize idx = 0; idx < block_length; ++idx) {
        u64        expected = idx * start_value;
        u64 const* actual   = reinterpret_cast<u64 const*>(
            reinterpret_cast<u8 const*>(block_expected_addr) + idx * block_stride);
        psh_assert(*actual == expected);
    }

    test_passed();
}

void memory_statistics() {
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
    usize used = memory_manager.allocator.offset;
    psh_assert(used == expected_arena);
    psh_assert(memory_manager.allocation_count == static_cast<usize>(1));

    strptr a = memory_manager.alloc<char>(40);
    psh_assert(a != nullptr);
    usize a_size = wrap_sub(memory_manager.allocator.offset, used);
    used         = memory_manager.allocator.offset;
    psh_assert(memory_manager.allocation_count == static_cast<usize>(2));

    DynArray<usize> b{&arena, 33};
    psh_discard(b);
    constexpr usize b_size = sizeof(usize) * 33;
    // The manager shouldn't have a bump in its usage.
    psh_assert(memory_manager.allocator.offset == used);
    // Expect no alignment from first allocation
    psh_assert(arena.offset == b_size);
    psh_assert(memory_manager.allocation_count == static_cast<usize>(2));

    strptr c = memory_manager.alloc<char>(34);
    psh_assert(c != nullptr);
    usize c_size = wrap_sub(memory_manager.allocator.offset, used);
    used         = memory_manager.allocator.offset;
    psh_assert(memory_manager.allocation_count == static_cast<usize>(3));

    DynArray<f32> d{&arena, 45};
    psh_discard(d);
    constexpr usize d_size = sizeof(f32) * 45;
    // Expect no change in the manager.
    psh_assert(memory_manager.allocator.offset == used);
    // Alignment may have happened
    psh_assert(arena.offset >= b_size + d_size);
    psh_assert(memory_manager.allocation_count == static_cast<usize>(3));

    strptr e = memory_manager.alloc<char>(90);
    psh_assert(e != nullptr);
    usize e_size = wrap_sub(memory_manager.allocator.offset, used);
    used         = memory_manager.allocator.offset;
    psh_assert(memory_manager.allocation_count == static_cast<usize>(4));

    strptr f = memory_manager.alloc<char>(55);
    psh_assert(f != nullptr);
    usize f_size = wrap_sub(memory_manager.allocator.offset, used);
    used         = memory_manager.allocator.offset;
    psh_assert(memory_manager.allocation_count == static_cast<usize>(5));

    u64* g = memory_manager.alloc<u64>(72);
    psh_assert(g != nullptr);
    usize g_size = wrap_sub(memory_manager.allocator.offset, used);
    psh_assert(memory_manager.allocation_count == static_cast<usize>(6));

    psh_assert(memory_manager.allocator.offset >= static_cast<i64>(expected_total_at_least));

    //
    // Free memory and check if the memory statistics is correctly updated.
    //

    usize actual_used_memory = memory_manager.allocator.offset;

    // Free both `f` and `g`.
    memory_manager.clear_until(reinterpret_cast<u8 const*>(f));
    psh_assert(memory_manager.allocation_count == expected_allocation_count - 2);
    psh_assert(memory_manager.allocator.offset == wrap_sub(actual_used_memory, f_size + g_size));

    // Pop `e`.
    memory_manager.pop();
    psh_assert(memory_manager.allocation_count == expected_allocation_count - 3);
    psh_assert(
        memory_manager.allocator.offset == wrap_sub(actual_used_memory, f_size + g_size + e_size));

    // Pop `c`.
    memory_manager.pop();
    psh_assert(memory_manager.allocation_count == expected_allocation_count - 4);
    psh_assert(
        memory_manager.allocator.offset ==
        wrap_sub(actual_used_memory, f_size + g_size + e_size + c_size));

    // Free `a`.
    memory_manager.clear_until(reinterpret_cast<u8 const*>(a));
    psh_assert(memory_manager.allocation_count == expected_allocation_count - 5);
    psh_assert(
        memory_manager.allocator.offset ==
        wrap_sub(actual_used_memory, f_size + g_size + e_size + c_size + a_size));

    // Assert that the only thing left is the arena.
    psh_assert(memory_manager.allocator.offset == expected_arena);

    // Pop `arena`
    memory_manager.pop();
    psh_assert(memory_manager.allocation_count == 0ull);
    psh_assert(memory_manager.allocator.offset == 0ull);

    test_passed();
}

int main() {
    zeroed_at_initialization();
    initialization_and_shutdown();
    memory_statistics();
    return 0;
}
