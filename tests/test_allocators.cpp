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
/// Description: Tests for the stack memory allocator.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <stdlib.h>
#include <psh_memory.hpp>
#include "utils.hpp"

namespace psh::test::allocators {
    struct FooBar {
        f64 a;
        u32 b;
    };

    psh_internal void scratch_arena_basic() {
        Arena arena = make_owned_arena(1024);
        psh_defer(destroy_owned_arena(&arena));

        usize base_offset = 0;

        // Test 1
        //
        // arena -> s1 -> s2
        //          |     |
        //          |     s4 -> s5
        //          |     |
        //          |  -> s3
        //
        // Lifetimes:
        //     |-> arena creates s1
        //     |-> s1 creates s2
        //         |-> s1 creates s3
        //         |-> s2 creates s4
        //            |-> s4 creates s5
        //            |-> s5 is destroyed, arena goes back to s5_base_offset
        //         |-> s4 is destroyed, arena goes back to s4_base_offset
        //         |-> s3 is destroyed, arena goes back to s3_base_offset
        //     |-> s2 is destroyed, arena goes back to s2_base_offset
        //     |-> s1 is destroyed, arena goes back to s1_base_offset
        {
            ScratchArena s1{&arena};
            usize        last_offset = base_offset;

            // Check consistency while allocating within a scratch arena.
            psh_assert(s1.arena == &arena);
            psh_assert(s1.saved_offset == base_offset);
            {
                usize a1_size = 32;
                u8*   a1      = memory_alloc<u8>(s1.arena, a1_size);
                psh_assert_not_null(a1);
                last_offset += a1_size;

                usize a2_size = 32;
                u8*   a2      = memory_alloc<u8>(s1.arena, a2_size);
                psh_assert_not_null(a2);
                last_offset += a2_size;

                // Throw away.
                psh_discard_value(a1);
                psh_discard_value(a2);
            }
            psh_assert(s1.saved_offset == base_offset);
            psh_assert(s1.arena == &arena);
            psh_assert(arena.offset == last_offset);

            // -----------------------------------------------------------------------------------------------------
            // Create a new scratch arena from the previous one.
            // -----------------------------------------------------------------------------------------------------
            usize s2_base_offset = last_offset;
            {
                ScratchArena s2{s1.arena};

                // Check if this new scratch has a working allocation scheme.
                psh_assert(s2.arena == &arena);
                psh_assert(s2.saved_offset == s2_base_offset);
                {
                    usize b1_size = 32;
                    u8*   b1      = memory_alloc<u8>(s2.arena, b1_size);
                    psh_assert_not_null(b1);
                    last_offset += b1_size;

                    usize b2_size = 64;
                    u8*   b2      = memory_alloc<u8>(s2.arena, b2_size);
                    psh_assert_not_null(b2);
                    last_offset += b2_size;

                    // Throw away.
                    psh_discard_value(b1);
                    psh_discard_value(b2);
                }
                psh_assert(s2.arena == &arena);
                psh_assert(s2.saved_offset == s2_base_offset);

                // -----------------------------------------------------------------------------------------------------
                // Create another scratch arena from s1 within the same lifetime of s2.
                // -----------------------------------------------------------------------------------------------------

                ScratchArena s3{s1.arena};

                // Check if this new scratch has a working allocation scheme.
                psh_assert(s3.arena == &arena);
                psh_assert(s3.saved_offset == last_offset);
                usize s3_base_offset = last_offset;
                {
                    usize c1_size = 16;
                    u8*   c1      = memory_alloc<u8>(s3.arena, c1_size);
                    psh_assert_not_null(c1);
                    last_offset += c1_size;

                    usize c2_size = 128;
                    u8*   c2      = memory_alloc<u8>(s3.arena, c2_size);
                    psh_assert_not_null(c2);
                    last_offset += c2_size;

                    // Throw away.
                    psh_discard_value(c1);
                    psh_discard_value(c2);
                }
                psh_assert(s3.arena == &arena);
                psh_assert(s3.saved_offset == s3_base_offset);

                // -----------------------------------------------------------------------------------------------------
                // Create another scratch arena from s2 within the same lifetime of s2 and s3.
                // -----------------------------------------------------------------------------------------------------

                ScratchArena s4{s2.arena};

                // Check if this new scratch has a working allocation scheme.
                psh_assert(s4.arena == &arena);
                psh_assert(s4.saved_offset == last_offset);
                usize s4_base_offset = last_offset;
                {
                    usize c1_size = 16;
                    u8*   c1      = memory_alloc<u8>(s4.arena, c1_size);
                    psh_assert_not_null(c1);
                    last_offset += c1_size;

                    usize c2_size = 128;
                    u8*   c2      = memory_alloc<u8>(s4.arena, c2_size);
                    psh_assert_not_null(c2);
                    last_offset += c2_size;

                    // Throw away.
                    psh_discard_value(c1);
                    psh_discard_value(c2);
                }
                psh_assert(s4.arena == &arena);
                psh_assert(s4.saved_offset == s4_base_offset);

                // -----------------------------------------------------------------------------------------------------
                // Create a scratch arena from s4
                // -----------------------------------------------------------------------------------------------------

                usize s5_base_offset = last_offset;
                {
                    ScratchArena s5{s4.arena};

                    // Check if this new scratch has a working allocation scheme.
                    psh_assert(s5.arena == &arena);
                    psh_assert(s5.saved_offset == last_offset);
                    {
                        usize d1_size = 16;
                        u8*   d1      = memory_alloc<u8>(s5.arena, d1_size);
                        psh_assert_not_null(d1);
                        last_offset += d1_size;
                        psh_assert(s1.arena->offset == last_offset);

                        usize c2_size = 128;
                        u8*   c2      = memory_alloc<u8>(s5.arena, c2_size);
                        psh_assert_not_null(c2);
                        last_offset += c2_size;
                        psh_assert(s1.arena->offset == last_offset);

                        // Throw away.
                        psh_discard_value(d1);
                        psh_discard_value(c2);
                    }
                    psh_assert(s5.arena == &arena);
                    psh_assert(s5.saved_offset == s5_base_offset);
                }

                // s5 destroyed.
                psh_assert(arena.offset == s5_base_offset);
            }
            // s4, s3, s2 destroyed.
            psh_assert(arena.offset == s2_base_offset);

            // Check the state of s1.
            psh_assert(s1.saved_offset == base_offset);
            psh_assert(s1.arena == &arena);
        }
        psh_assert(arena.offset == base_offset);

        report_test_successful();
    }

    psh_internal u8* fn_scratch_as_ref(ScratchArena& s, usize size) {
        return memory_alloc<u8>(s.arena, size);
    }

    psh_internal void scratch_arena_passed_as_reference() {
        Arena ar = make_owned_arena(1024);
        psh_defer(destroy_owned_arena(&ar));

        usize expected_offset = 0;
        psh_discard_value(memory_alloc<u8>(&ar, 32));
        expected_offset += 32;
        {
            ScratchArena s{&ar};
            psh_discard_value(fn_scratch_as_ref(s, 64));
            expected_offset += 64;
            psh_assert(ar.offset == expected_offset);
        }
        expected_offset -= 64;
        psh_assert(ar.offset == expected_offset);

        report_test_successful();
    }

    psh_internal void stack_allocation_with_default_alignment() {
        usize stack_min_expected_size = 0;
        usize expected_alloc_size     = 512;
        u8*   buf                     = reinterpret_cast<u8*>(malloc(expected_alloc_size));
        Stack stack{.buf = buf, .capacity = expected_alloc_size};

        u8    expected_u8_vec[5]   = {51, 102, 153, 204, 255};
        usize expected_u8_vec_size = 5 * psh_usize_of(u8);  // 5 bytes
        u8*   test_vec_u8          = memory_alloc<u8>(&stack, expected_u8_vec_size);
        psh_assert_not_null(test_vec_u8);
        for (usize i = 0; i < 5; ++i) {
            test_vec_u8[i] = expected_u8_vec[i];
        }
        stack_min_expected_size += psh_usize_of(StackHeader) + expected_u8_vec_size;

        // The first allocation has a zero alignment so we can check for equality.
        psh_assert(stack.offset == stack_min_expected_size);

        u32   expected_u32_vec[3]   = {1, 1024, 1073741824};
        usize expected_u32_vec_size = 3 * psh_usize_of(u32);  // 12 bytes
        u32*  test_vec_u32          = memory_alloc<u32>(&stack, 3);
        psh_assert_not_null(test_vec_u32);
        for (usize i = 0; i < 3; ++i) {
            test_vec_u32[i] = expected_u32_vec[i];
        }
        stack_min_expected_size += psh_usize_of(StackHeader) + expected_u32_vec_size;

        psh_assert(stack.offset >= stack_min_expected_size);

        usize size = stack.capacity;
        psh_assert(size == expected_alloc_size);

        StackHeader const* th_u32 = stack.top_header();
        psh_assert_not_null(th_u32);
        usize actual_u32_vec_size = th_u32->capacity;
        psh_assert(actual_u32_vec_size == expected_u32_vec_size);

        u8* top_u32 = stack.top();
        psh_assert_not_null(top_u32);
        u32* actual_u32_vec = reinterpret_cast<u32*>(top_u32);
        for (usize i = 0; i < 3; ++i) {
            u32 actual   = actual_u32_vec[i];
            u32 expected = expected_u32_vec[i];
            psh_assert(actual == expected);
        }

        psh_assert(stack.pop());

        StackHeader const* th_u8 = stack.top_header();
        psh_assert_not_null(th_u8);
        usize actual_u8_vec_size = th_u8->capacity;
        psh_assert(actual_u8_vec_size == 5ull);

        u8* actual_u8_vec = stack.top();
        psh_assert_not_null(actual_u8_vec);
        for (usize i = 0; i < 5; ++i) {
            u32 actual   = actual_u8_vec[i];
            u32 expected = expected_u8_vec[i];
            psh_assert(actual == expected);
        }

        psh_assert(stack.pop());

        psh_assert(stack.offset == 0ull);
        psh_assert(stack.previous_offset == 0ull);

        free(buf);
        report_test_successful();
    }

    psh_internal void stack_offsets_reads_and_writes() {
        usize size = 1024;
        u8*   buf  = reinterpret_cast<u8*>(malloc(size));
        Stack stack{.buf = buf, .capacity = size};

        u8* buf_start = buf;

        // Create an array of uint64_t.
        usize array1_len       = 70;
        usize array1_size      = array1_len * psh_usize_of(u64);
        usize array1_alignment = psh_usize_of(u64);
        u64*  array1           = memory_alloc<u64>(&stack, array1_len);
        psh_assert_not_null(array1);

        // Write to array1.
        for (u32 idx = 0; idx < array1_len; ++idx) {
            array1[idx] = 64 * static_cast<u64>(idx);
        }

        // Check correctness of the array1 offset.
        usize array1_expected_padding = psh_usize_of(StackHeader);
        usize array1_expected_offset  = array1_expected_padding;
        psh_assert(stack.previous_offset == array1_expected_offset);

        u8* array1_addr = buf_start + array1_expected_offset;

        // Check the correctness of the array1 header.
        StackHeader const* array1_header                 = reinterpret_cast<StackHeader const*>(array1_addr - psh_usize_of(StackHeader));
        usize              array1_actual_padding         = array1_header->padding;
        usize              array1_actual_previous_offset = array1_header->previous_offset;
        psh_assert(array1_actual_padding == array1_expected_padding);
        psh_assert(array1_actual_previous_offset == 0ull);

        // Manually read from stack.buf checking for the values of array1.
        for (usize idx = 0; idx < array1_len; ++idx) {
            u64        expected = 64 * idx;
            u64 const* actual   = reinterpret_cast<u64*>(array1_addr + idx * array1_alignment);
            psh_assert_not_null(actual);
            psh_assert(*actual == expected);
        }

        // Check offset to the free memory in the stack.
        usize after_array1_expected_offset = array1_expected_offset + array1_size;
        psh_assert(stack.offset == after_array1_expected_offset);

        // Create an array of strings.
        usize array2_len       = 30;
        usize array2_size      = array2_len * psh_usize_of(i32);
        usize array2_alignment = psh_usize_of(i32);
        i32*  array2           = memory_alloc<i32>(&stack, array2_len);
        psh_assert_not_null(array2);

        // Write to array2.
        int array2_constant_value = 123456;
        for (usize idx = 0; idx < array2_len; ++idx) {
            array2[idx] = array2_constant_value;
        }

        // Check correctness of the array2 offset.
        usize array2_alignment_modifier = after_array1_expected_offset % array2_alignment;
        usize array2_expected_padding   = (array2_alignment_modifier == 0)
                                              ? psh_usize_of(StackHeader)
                                              : array2_alignment - array2_alignment_modifier + psh_usize_of(StackHeader);
        usize array2_expected_offset    = after_array1_expected_offset + array2_expected_padding;
        psh_assert(stack.previous_offset == array2_expected_offset);

        u8* array2_addr = buf_start + array2_expected_offset;

        // Check the correctness of the array2 header.
        StackHeader const* array2_header         = reinterpret_cast<StackHeader const*>(array2_addr - psh_usize_of(StackHeader));
        usize              array2_actual_padding = array2_header->padding;
        psh_assert(array2_actual_padding == array2_expected_padding);
        usize array2_expected_previous_offset = static_cast<usize>(array1_addr - buf_start);
        usize array2_actual_previous_offset   = array2_header->previous_offset;
        psh_assert(array2_actual_previous_offset == array2_expected_previous_offset);

        // Manually read from stack.buf checking for the values of array2.
        for (usize idx = 0; idx < array2_len; ++idx) {
            int const* actual = reinterpret_cast<int const*>(array2_addr + idx * array2_alignment);
            psh_assert_not_null(actual);
            psh_assert(*actual == array2_constant_value);
        }

        // Check offset to the free memory in the stack.
        iptr after_array2_expected_offset = reinterpret_cast<iptr>(array2_addr) + static_cast<iptr>(array2_size) - reinterpret_cast<iptr>(buf_start);
        psh_assert(static_cast<iptr>(stack.offset) == after_array2_expected_offset);

        free(buf);
        report_test_successful();
    }

    psh_internal void stack_memory_stress_and_free() {
        usize size = 2048;
        u8*   buf  = reinterpret_cast<u8*>(malloc(size));
        Stack stack{.buf = buf, .capacity = size};

        iptr  stack_buf_diff = reinterpret_cast<iptr>(stack.buf);
        usize zero           = 0;

        usize   a1_alignment = psh_usize_of(cstring);
        cstring a1           = memory_alloc<char>(&stack, 50);
        psh_assert_not_null(a1);
        psh_assert((stack.previous_offset - psh_usize_of(StackHeader)) % alignof(StackHeader) == zero);
        psh_assert(stack.previous_offset % a1_alignment == zero);
        psh_assert(static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a1) - stack_buf_diff);

        usize a2_alignment = psh_usize_of(i32);
        i32*  a2           = memory_alloc<i32>(&stack, 100);
        psh_assert_not_null(a2);
        psh_assert((stack.previous_offset - psh_usize_of(StackHeader)) % alignof(StackHeader) == zero);
        psh_assert(stack.previous_offset % a2_alignment == zero);
        psh_assert(static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a2) - stack_buf_diff);

        usize a3_alignment = psh_usize_of(u64);
        u64*  a3           = memory_alloc<u64>(&stack, 33);
        psh_assert_not_null(a3);
        psh_assert((stack.previous_offset - psh_usize_of(StackHeader)) % alignof(StackHeader) == zero);
        psh_assert(stack.previous_offset % a3_alignment == zero);
        psh_assert(static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a3) - stack_buf_diff);

        usize a4_alignment = psh_usize_of(u8);
        u8*   a4           = memory_alloc<u8>(&stack, 49);
        psh_assert_not_null(a4);
        psh_assert((stack.previous_offset - psh_usize_of(StackHeader)) % alignof(StackHeader) == zero);
        psh_assert(stack.previous_offset % a4_alignment == zero);
        psh_assert(static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a4) - stack_buf_diff);

        usize a5_alignment = psh_usize_of(u32);
        u32*  a5           = memory_alloc<u32>(&stack, 8);
        psh_assert_not_null(a5);
        psh_assert((stack.previous_offset - psh_usize_of(StackHeader)) % alignof(StackHeader) == zero);
        psh_assert(stack.previous_offset % a5_alignment == zero);
        psh_assert(static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a5) - stack_buf_diff);

        usize   a6_alignment = psh_usize_of(cstring);
        cstring a6           = memory_alloc<char>(&stack, 14);
        psh_assert_not_null(a6);
        psh_assert((stack.previous_offset - psh_usize_of(StackHeader)) % alignof(StackHeader) == zero);
        psh_assert(stack.previous_offset % a6_alignment == zero);
        psh_assert(static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a6) - stack_buf_diff);

        // Free a6.
        stack.pop();  // NOTE: a6 is now dangling.
        psh_assert(static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a5) - stack_buf_diff);

        // Free a3, a4, and a5 (they should all be dangling after the free).
        psh_assert(stack.clear_at(reinterpret_cast<u8*>(a3)));
        psh_assert(static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a2) - stack_buf_diff);

        // Free both a2 and a1.
        stack.clear();
        psh_assert(stack.previous_offset == zero);
        psh_assert(stack.offset == zero);
        psh_assert(stack.buf && (stack.capacity != 0));  // The memory should still be available.

        // Ensure we can allocate after freeing all blocks.
        i32* b1 = memory_alloc<int>(&stack, 80);
        f64* b2 = memory_alloc<double>(&stack, 80);
        psh_assert_not_null(b1);
        psh_assert_not_null(b2);

        free(buf);
        report_test_successful();
    }

    psh_internal void stack_free_all() {
        usize     size = 512;
        u8* const buf  = reinterpret_cast<u8*>(malloc(size));
        Stack     stack{.buf = buf, .capacity = size};

        usize expected_min_size = 0;

        usize fib_size  = 30 * psh_usize_of(u64);
        u64*  fibonacci = memory_alloc<u64>(&stack, 30);
        psh_assert_not_null(fibonacci);
        expected_min_size += fib_size;
        fibonacci[0] = 1;
        fibonacci[1] = 1;
        for (u64 idx = 2; idx < 30; ++idx) {
            fibonacci[idx] = fibonacci[idx - 1u] + fibonacci[idx - 2u];
        }

        psh_assert(stack.offset >= expected_min_size);

        usize   foos_size = 10 * psh_usize_of(FooBar);
        FooBar* foos      = memory_alloc<FooBar>(&stack, 10);
        psh_assert_not_null(foos);
        expected_min_size += foos_size;
        for (u32 idx = 0; idx < 10; ++idx) {
            foos[idx] = FooBar{static_cast<f64>(idx) / 2.0, idx};
        }

        psh_assert(stack.offset >= expected_min_size);

        stack.clear();
        psh_assert(stack.offset == 0ull);

        psh_assert(stack.offset == 0ull);
        psh_assert(stack.previous_offset == 0ull);

        free(buf);
        report_test_successful();
    }

    psh_internal void run_all() {
        scratch_arena_basic();
        scratch_arena_passed_as_reference();
        stack_allocation_with_default_alignment();
        stack_offsets_reads_and_writes();
        stack_memory_stress_and_free();
        stack_free_all();
    }
}  // namespace psh::test::allocators

#if !defined(PSH_TEST_NOMAIN)
int main() {
    psh::test::allocators::run_all();
    return 0;
}
#endif
