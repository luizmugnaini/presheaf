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
/// Description: Tests for the memory manager system.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
///
/// This test should be ran with sanitizer flags on in order to detect possible memory leaks that
/// may go unseen.

#include <psh/memory.hpp>
#include "utils.hpp"

namespace psh::test::memory_manager {
    psh_internal void zeroed_at_initialisation() {
        MemoryManager memory_manager;
        memory_manager.init(1024);

        // Check validity.
        void* mem_ptr = memory_manager.allocator.buf;
        psh_assert_not_null(mem_ptr);

        psh_assert(memory_manager.allocator.offset == static_cast<i64>(0));
        psh_assert(memory_manager.allocation_count == 0ull);

        // Check allocator.
        psh_assert(memory_manager.allocator.buf == reinterpret_cast<u8*>(mem_ptr));
        psh_assert(memory_manager.allocator.offset == 0ull);
        psh_assert(memory_manager.allocator.previous_offset == 0ull);
        psh_assert(memory_manager.allocator.capacity == static_cast<usize>(1024));
        report_test_successful();
    }

    psh_internal void initialisation_and_shutdown() {
        usize         memory_manager_capacity = 2048;
        MemoryManager memory_manager;
        memory_manager.init(memory_manager_capacity);

        u8 const* mem_sys_alloc_mem_actual_addr = memory_manager.allocator.buf;

        // Acquire a block of memory and write to it.
        usize block_length = 60;
        u64*  block        = memory_alloc<u64>(&memory_manager, block_length);
        psh_assert_not_null(block);

        // Write to the block.
        u64 start_value = 1283;
        for (usize idx = 0; idx < block_length; ++idx) {
            block[idx] = idx * start_value;
        }

        // Ensure the addresses where correctly calculated.
        u64 const* block_expected_addr = reinterpret_cast<u64 const*>(mem_sys_alloc_mem_actual_addr + sizeof(StackHeader));
        psh_assert(block == block_expected_addr);
        u64 const* block_via_allocator_actual_addr = reinterpret_cast<u64 const*>(mem_sys_alloc_mem_actual_addr + memory_manager.allocator.previous_offset);
        psh_assert(block_via_allocator_actual_addr == block_expected_addr);

        psh_assert_msg(memory_manager.allocation_count == 1, "Expected memory stats to account for a single memory allocation count.");

        // Manually read each address to check if the data of block got written to the right location.
        usize block_stride = sizeof(u64);
        for (usize idx = 0; idx < block_length; ++idx) {
            u64        expected = idx * start_value;
            u64 const* actual   = reinterpret_cast<u64 const*>(reinterpret_cast<u8 const*>(block_expected_addr) + idx * block_stride);
            psh_assert(*actual == expected);
        }

        report_test_successful();
    }

    psh_internal void memory_statistics() {
        constexpr usize expected_string_at_least  = sizeof(char) * (40 + 34 + 55) + 3 * sizeof(StackHeader);
        constexpr usize expected_misc_at_least    = sizeof(char) * 90 + sizeof(u64) * 72 + 2 * sizeof(StackHeader);
        constexpr usize arena_data_size           = sizeof(usize) * 33 + sizeof(f32) * 45;
        constexpr usize expected_arena            = sizeof(StackHeader) + arena_data_size;
        constexpr usize expected_allocation_count = 6;  // NOTE: 3 strings, 1 arena, 2 misc.
        constexpr usize expected_total_at_least   = expected_arena + expected_string_at_least + expected_misc_at_least;

        MemoryManager memory_manager;
        memory_manager.init(2048);

        // Allocate memory.
        //
        // Expected final layout of the stack allocator:
        //
        // | hdr + (b  and  d) | hdr + a | hdr + c | hdr + e | hdr + f | hdr + g |
        // ^                   ^
        // |      arena        |
        //
        // where "hdr" stands for the associated StackAllocHeader instance.

        Arena arena;
        init_arena(&arena, memory_alloc<u8>(&memory_manager, arena_data_size), arena_data_size);
        usize used = memory_manager.allocator.offset;
        psh_assert(used == expected_arena);
        psh_assert(memory_manager.allocation_count == 1);

        cstring a = memory_alloc<char>(&memory_manager, 40);
        psh_assert_not_null(a);
        usize a_size = no_wrap_sub(memory_manager.allocator.offset, used);
        used         = memory_manager.allocator.offset;
        psh_assert(memory_manager.allocation_count == 2);

        DynamicArray<usize> b;
        init_dynamic_array(&b, &arena, 33);
        psh_discard_value(b);
        constexpr usize b_size = sizeof(usize) * 33;
        // The manager shouldn't have a bump in its usage.
        psh_assert(memory_manager.allocator.offset == used);
        // Expect no alignment from first allocation
        psh_assert(arena.offset == b_size);
        psh_assert(memory_manager.allocation_count == 2);

        cstring c = memory_alloc<char>(&memory_manager, 34);
        psh_assert_not_null(c);
        usize c_size = no_wrap_sub(memory_manager.allocator.offset, used);
        used         = memory_manager.allocator.offset;
        psh_assert(memory_manager.allocation_count == 3);

        DynamicArray<f32> d;
        init_dynamic_array(&d, &arena, 45);
        psh_discard_value(d);
        constexpr usize d_size = sizeof(f32) * 45;
        // Expect no change in the manager.
        psh_assert(memory_manager.allocator.offset == used);
        // Alignment may have happened
        psh_assert(arena.offset >= b_size + d_size);
        psh_assert(memory_manager.allocation_count == 3);

        cstring e = memory_alloc<char>(&memory_manager, 90);
        psh_assert_not_null(e);
        usize e_size = no_wrap_sub(memory_manager.allocator.offset, used);
        used         = memory_manager.allocator.offset;
        psh_assert(memory_manager.allocation_count == 4);

        cstring f = memory_alloc<char>(&memory_manager, 55);
        psh_assert_not_null(f);
        usize f_size = no_wrap_sub(memory_manager.allocator.offset, used);
        used         = memory_manager.allocator.offset;
        psh_assert(memory_manager.allocation_count == 5);

        u64* g = memory_alloc<u64>(&memory_manager, 72);
        psh_assert_not_null(g);
        usize g_size = no_wrap_sub(memory_manager.allocator.offset, used);
        psh_assert(memory_manager.allocation_count == 6);

        psh_assert(memory_manager.allocator.offset >= static_cast<i64>(expected_total_at_least));

        //
        // Free memory and check if the memory statistics is correctly updated.
        //

        usize actual_used_memory = memory_manager.allocator.offset;

        // Free both f and g.
        memory_manager.clear_until(reinterpret_cast<u8 const*>(f));
        psh_assert(memory_manager.allocation_count == expected_allocation_count - 2);
        psh_assert(memory_manager.allocator.offset == no_wrap_sub(actual_used_memory, f_size + g_size));

        // Pop e.
        memory_manager.pop();
        psh_assert(memory_manager.allocation_count == expected_allocation_count - 3);
        psh_assert(memory_manager.allocator.offset == no_wrap_sub(actual_used_memory, f_size + g_size + e_size));

        // Pop c.
        memory_manager.pop();
        psh_assert(memory_manager.allocation_count == expected_allocation_count - 4);
        psh_assert(memory_manager.allocator.offset == no_wrap_sub(actual_used_memory, f_size + g_size + e_size + c_size));

        // Free a.
        memory_manager.clear_until(reinterpret_cast<u8 const*>(a));
        psh_assert(memory_manager.allocation_count == expected_allocation_count - 5);
        psh_assert(memory_manager.allocator.offset == no_wrap_sub(actual_used_memory, f_size + g_size + e_size + c_size + a_size));

        // Assert that the only thing left is the arena.
        psh_assert(memory_manager.allocator.offset == expected_arena);

        // Pop arena
        memory_manager.pop();
        psh_assert(memory_manager.allocation_count == 0ull);
        psh_assert(memory_manager.allocator.offset == 0ull);

        report_test_successful();
    }

    psh_internal void run_all() {
        zeroed_at_initialisation();
        initialisation_and_shutdown();
        memory_statistics();
    }
}  // namespace psh::test::memory_manager

#if !defined(PSH_TEST_NOMAIN)
int main() {
    psh::test::memory_manager::run_all();
    return 0;
}
#endif
