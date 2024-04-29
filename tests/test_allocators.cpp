/// Tests for the stack memory allocator.
///
/// @author Luiz G. Mugnaini. A. <luizmugnaini@gmail.com>
#include <psh/arena.h>
#include <psh/assert.h>
#include <psh/mem_utils.h>
#include <psh/stack.h>
#include <psh/types.h>

#include <cstdio>
#include <cstdlib>

#include "utils.h"

using namespace psh;

struct Foo {
    f64 a;
    u32 b;
};

void arena_scratch() {
    constexpr StrPtr header   = "[arena_scratch]";
    constexpr usize  capacity = 1024;
    auto* const      buf      = reinterpret_cast<u8*>(std::malloc(capacity));
    Arena            arena{buf, capacity};
    constexpr usize  base_offset = 0;

    // Test 1
    //
    // arena -> s1 -> s2
    //          |     |
    //          |     s4 -> s5
    //          |     |
    //          |  -> s3
    //
    // Lifetimes:
    //     |-> arena creates `s1`
    //     |-> `s1` creates `s2`
    //         |-> `s1` creates `s3`
    //         |-> `s2` creates `s4`
    //            |-> `s4` creates `s5`
    //            |-> `s5` is destroyed, arena goes back to `s5_base_offset`
    //         |-> `s4` is destroyed, arena goes back to `s4_base_offset`
    //         |-> `s3` is destroyed, arena goes back to `s3_base_offset`
    //     |-> `s2` is destroyed, arena goes back to `s2_base_offset`
    //     |-> `s1` is destroyed, arena goes back to `s1_base_offset`
    {
        Arena::Scratch s1          = arena.make_scratch();
        usize          last_offset = base_offset;

        // Check consistency while allocating within a scratch arena.
        psh_assert(s1.arena() == &arena);
        psh_assert(s1.saved_offset() == base_offset);
        {
            constexpr usize a1_size = 32;
            u8*             a1      = s1.arena()->alloc<u8>(a1_size);
            psh_assert(a1 != nullptr);
            last_offset += a1_size;

            constexpr usize a2_size = 32;
            u8*             a2      = s1.arena()->alloc<u8>(a2_size);
            psh_assert(a2 != nullptr);
            last_offset += a2_size;

            // Throw away.
            psh_discard(a1);
            psh_discard(a2);
        }
        psh_assert(s1.saved_offset() == base_offset);
        psh_assert(s1.arena() == &arena);
        psh_assert(arena.offset == last_offset);

        // ---------------------------------------------------------------------------------
        // Create a new scratch arena from the previous one.
        // ---------------------------------------------------------------------------------
        usize const s2_base_offset = last_offset;
        {
            Arena::Scratch s2 = s1.decouple();

            // Check if this new scratch has a working allocation scheme.
            psh_assert(s2.arena() == &arena);
            psh_assert(s2.saved_offset() == s2_base_offset);
            {
                constexpr usize b1_size = 32;
                u8*             b1      = s2.arena()->alloc<u8>(b1_size);
                psh_assert(b1 != nullptr);
                last_offset += b1_size;

                constexpr usize b2_size = 64;
                u8*             b2      = s2.arena()->alloc<u8>(b2_size);
                psh_assert(b2 != nullptr);
                last_offset += b2_size;

                // Throw away.
                psh_discard(b1);
                psh_discard(b2);
            }
            psh_assert(s2.arena() == &arena);
            psh_assert(s2.saved_offset() == s2_base_offset);

            // ---------------------------------------------------------------------------------
            // Create another scratch arena from `s1` within the same lifetime of `s2`.
            // ---------------------------------------------------------------------------------

            Arena::Scratch s3 = s1.decouple();

            // Check if this new scratch has a working allocation scheme.
            psh_assert(s3.arena() == &arena);
            psh_assert(s3.saved_offset() == last_offset);
            usize const s3_base_offset = last_offset;
            {
                constexpr usize c1_size = 16;
                u8*             c1      = s3.arena()->alloc<u8>(c1_size);
                psh_assert(c1 != nullptr);
                last_offset += c1_size;

                constexpr usize c2_size = 128;
                u8*             c2      = s3.arena()->alloc<u8>(c2_size);
                psh_assert(c2 != nullptr);
                last_offset += c2_size;

                // Throw away.
                psh_discard(c1);
                psh_discard(c2);
            }
            psh_assert(s3.arena() == &arena);
            psh_assert(s3.saved_offset() == s3_base_offset);

            // ---------------------------------------------------------------------------------
            // Create another scratch arena from `s2` within the same lifetime of `s2` and `s3`.
            // ---------------------------------------------------------------------------------

            Arena::Scratch s4 = s2.decouple();

            // Check if this new scratch has a working allocation scheme.
            psh_assert(s4.arena() == &arena);
            psh_assert(s4.saved_offset() == last_offset);
            usize const s4_base_offset = last_offset;
            {
                constexpr usize c1_size = 16;
                u8*             c1      = s4.arena()->alloc<u8>(c1_size);
                psh_assert(c1 != nullptr);
                last_offset += c1_size;

                constexpr usize c2_size = 128;
                u8*             c2      = s4.arena()->alloc<u8>(c2_size);
                psh_assert(c2 != nullptr);
                last_offset += c2_size;

                // Throw away.
                psh_discard(c1);
                psh_discard(c2);
            }
            psh_assert(s4.arena() == &arena);
            psh_assert(s4.saved_offset() == s4_base_offset);

            // ---------------------------------------------------------------------------------
            // Create a scratch arena from `s4`
            // ---------------------------------------------------------------------------------

            usize const s5_base_offset = last_offset;
            {
                Arena::Scratch s5 = s4.decouple();

                // Check if this new scratch has a working allocation scheme.
                psh_assert(s5.arena() == &arena);
                psh_assert(s5.saved_offset() == last_offset);
                {
                    constexpr usize d1_size = 16;
                    u8*             d1      = s5.arena()->alloc<u8>(d1_size);
                    psh_assert(d1 != nullptr);
                    last_offset += d1_size;
                    psh_assert(s1.arena()->offset == last_offset);

                    constexpr usize c2_size = 128;
                    u8*             c2      = s5.arena()->alloc<u8>(c2_size);
                    psh_assert(c2 != nullptr);
                    last_offset += c2_size;
                    psh_assert(s1.arena()->offset == last_offset);

                    // Throw away.
                    psh_discard(d1);
                    psh_discard(c2);
                }
                psh_assert(s5.arena() == &arena);
                psh_assert(s5.saved_offset() == s5_base_offset);
            }

            // `s5` destroyed.
            psh_assert(arena.offset == s5_base_offset);
        }
        // `s4`, `s3`, `s2` destroyed.
        psh_assert(arena.offset == s2_base_offset);

        // Check the state of `s1`.
        psh_assert(s1.saved_offset() == base_offset);
        psh_assert(s1.arena() == &arena);
    }
    psh_assert(arena.offset == base_offset);

    std::free(buf);
    log_passed(header);
}

void print_stack_info(Stack const& stack) {
    std::printf(
        "Stack {\n\tmemory: %zu,\n\tcapacity: %zu,\n\toffset: %zu,\n\tprevious_offset: %zu\n}\n",
        reinterpret_cast<uptr>(stack.memory),
        stack.capacity,
        stack.offset,
        stack.previous_offset);
}

void stack_allocation_with_default_alignment() {
    constexpr StrPtr header                   = "[stack_allocation_with_default_alignment]";
    usize            salloc_min_expected_size = 0;
    constexpr usize  expected_alloc_capacity  = 512;
    auto* const      buf = reinterpret_cast<u8*>(std::malloc(expected_alloc_capacity));
    Stack            salloc{buf, expected_alloc_capacity};

    u8 const    expected_u8_vec[5]       = {51, 102, 153, 204, 255};
    usize const expected_u8_vec_capacity = 5 * sizeof(u8);  // 5 bytes
    u8* const   test_vec_u8              = salloc.alloc<u8>(expected_u8_vec_capacity);
    psh_assert(test_vec_u8 != nullptr);
    for (usize i = 0; i < 5; ++i) {
        test_vec_u8[i] = expected_u8_vec[i];
    }
    salloc_min_expected_size += sizeof(StackHeader) + expected_u8_vec_capacity;

    // The first allocation has a zero alignment so we can check for equality.
    psh_assert(salloc.used() == salloc_min_expected_size);

    u32 const   expected_u32_vec[3]       = {1, 1024, 1073741824};
    usize const expected_u32_vec_capacity = 3 * sizeof(u32);  // 12 bytes
    u32* const  test_vec_u32              = salloc.alloc<u32>(3);
    psh_assert(test_vec_u32 != nullptr);
    for (usize i = 0; i < 3; ++i) {
        test_vec_u32[i] = expected_u32_vec[i];
    }
    salloc_min_expected_size += sizeof(StackHeader) + expected_u32_vec_capacity;

    psh_assert(salloc.used() >= salloc_min_expected_size);

    usize const capacity = salloc.capacity;
    psh_assert(capacity == expected_alloc_capacity);

    auto const* const th_u32 = salloc.top_header();
    psh_assert(th_u32 != nullptr);
    usize const actual_u32_vec_capacity = th_u32->capacity;
    psh_assert(actual_u32_vec_capacity == expected_u32_vec_capacity);

    auto* const top_u32 = salloc.top();
    psh_assert(top_u32 != nullptr);
    u32* actual_u32_vec = reinterpret_cast<u32*>(top_u32);
    for (usize i = 0; i < 3; ++i) {
        u32 const actual   = actual_u32_vec[i];
        u32 const expected = expected_u32_vec[i];
        psh_assert(actual == expected);
    }

    psh_assert_msg(
        salloc.pop(),
        "Expected StackAlloc to be able to pop the top memory in the stack");

    auto const* const th_u8 = salloc.top_header();
    psh_assert(th_u8 != nullptr);
    usize const actual_u8_vec_capacity = th_u8->capacity;
    psh_assert(actual_u8_vec_capacity == 5ull);

    u8* const actual_u8_vec = salloc.top();
    psh_assert(actual_u8_vec != nullptr);
    for (usize i = 0; i < 5; ++i) {
        u32 const actual   = actual_u8_vec[i];
        u32 const expected = expected_u8_vec[i];
        psh_assert(actual == expected);
    }

    psh_assert_msg(
        salloc.pop(),
        "Expected StackAlloc to be able to pop the top memory in the stack");

    psh_assert_msg(salloc.offset == 0ull, "Expected empty StackAlloc");
    psh_assert_msg(salloc.previous_offset == 0ull, "%s Expected empty StackAlloc");

    std::free(buf);
    log_passed(header);
}

void stack_offsets_reads_and_writes() {
    constexpr StrPtr header = "[stack_offsets_reads_and_writes]";

    constexpr usize capacity = 1024;
    u8* const       buf      = reinterpret_cast<u8*>(std::malloc(capacity));
    Stack           stack{buf, capacity};

    uptr const buf_start_addr = reinterpret_cast<uptr>(buf);

    // Create an array of `uint64_t`.
    usize const array1_len       = 70;
    usize const array1_size      = array1_len * sizeof(u64);
    usize const array1_alignment = sizeof(u64);
    u64* const  array1           = stack.alloc<u64>(array1_len);
    psh_assert(array1 != nullptr);

    // Write to `array1`.
    for (u32 idx = 0; idx < array1_len; ++idx) {
        array1[idx] = 64 * static_cast<u64>(idx);
    }

    // Check correctness of the `array1` offset.
    constexpr usize array1_expected_padding = sizeof(StackHeader);
    constexpr usize array1_expected_offset  = array1_expected_padding;
    psh_assert(stack.previous_offset == array1_expected_offset);

    uptr const array1_addr = buf_start_addr + array1_expected_offset;

    // Check the correctness of the `array1` header.
    auto const* const array1_header =
        reinterpret_cast<StackHeader const*>(array1_addr - sizeof(StackHeader));
    usize const array1_actual_padding         = array1_header->padding;
    usize const array1_actual_previous_offset = array1_header->previous_offset;
    psh_assert(array1_actual_padding == array1_expected_padding);
    psh_assert_msg(
        array1_actual_previous_offset == 0ull,
        "Expected `array1` header to have a previous offset of zero");

    // Manually read from `stack.buf` checking for the values of `array1`.
    for (usize idx = 0; idx < array1_len; ++idx) {
        u64 const         expected = 64 * idx;
        auto const* const actual   = reinterpret_cast<u64*>(array1_addr + idx * array1_alignment);
        psh_assert(actual != nullptr);
        psh_assert(*actual == expected);
    }

    // Check offset to the free memory in the stack.
    constexpr usize after_array1_expected_offset = array1_expected_offset + array1_size;
    psh_assert(stack.offset == after_array1_expected_offset);

    // Create an array of strings.
    constexpr usize array2_len       = 30;
    constexpr usize array2_size      = array2_len * sizeof(i32);
    constexpr usize array2_alignment = sizeof(i32);
    i32* const      array2           = stack.alloc<i32>(array2_len);
    psh_assert(array2 != nullptr);

    // Write to `array2`.
    int const array2_constant_value = 123456;
    for (usize idx = 0; idx < array2_len; ++idx) {
        array2[idx] = array2_constant_value;
    }

    // Check correctness of the `array2` offset.
    usize const array2_alignment_modifier = after_array1_expected_offset % array2_alignment;
    usize const array2_expected_padding =
        (array2_alignment_modifier == 0)
            ? sizeof(StackHeader)
            : array2_alignment - array2_alignment_modifier + sizeof(StackHeader);
    usize const array2_expected_offset = after_array1_expected_offset + array2_expected_padding;
    psh_assert(stack.previous_offset == array2_expected_offset);

    uptr const array2_addr = buf_start_addr + array2_expected_offset;

    // Check the correctness of the `array2` header.
    auto const* const array2_header =
        reinterpret_cast<StackHeader const*>(array2_addr - sizeof(StackHeader));
    usize const array2_actual_padding = array2_header->padding;
    psh_assert(array2_actual_padding == array2_expected_padding);
    auto const  array2_expected_previous_offset = static_cast<usize>(array1_addr - buf_start_addr);
    usize const array2_actual_previous_offset   = array2_header->previous_offset;
    psh_assert(array2_actual_previous_offset == array2_expected_previous_offset);

    // Manually read from `stack.buf` checking for the values of `array2`.
    for (usize idx = 0; idx < array2_len; ++idx) {
        int const* const actual =
            reinterpret_cast<int const*>(array2_addr + idx * array2_alignment);
        psh_assert(actual != nullptr);
        psh_assert(*actual == array2_constant_value);
    }

    // Check offset to the free memory in the stack.
    usize const after_array2_expected_offset = array2_addr + array2_size - buf_start_addr;
    psh_assert(stack.offset == after_array2_expected_offset);

    free(buf);
    log_passed(header);
}

void stack_memory_stress_and_free() {
    constexpr StrPtr header = "[stack_memory_stress_and_free]";

    constexpr usize capacity = 2048;
    u8* const       buf      = reinterpret_cast<u8*>(std::malloc(capacity));
    Stack           stack{buf, capacity};

    iptr const  stack_buf_diff = reinterpret_cast<iptr>(stack.memory);
    usize const zero           = 0ull;

    constexpr usize a1_alignment = sizeof(StrPtr);
    StrPtr          a1           = stack.alloc<char>(50);
    psh_assert(a1 != nullptr);
    psh_assert((stack.previous_offset - sizeof(StackHeader)) % alignof(StackHeader) == zero);
    psh_assert(stack.previous_offset % a1_alignment == zero);
    psh_assert(
        static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a1) - stack_buf_diff);

    constexpr usize a2_alignment = sizeof(i32);
    i32* const      a2           = stack.alloc<i32>(100);
    psh_assert(a2 != nullptr);
    psh_assert((stack.previous_offset - sizeof(StackHeader)) % alignof(StackHeader) == zero);
    psh_assert(stack.previous_offset % a2_alignment == zero);
    psh_assert(
        static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a2) - stack_buf_diff);

    constexpr usize a3_alignment = sizeof(u64);
    u64* const      a3           = stack.alloc<u64>(33);
    psh_assert(a3 != nullptr);
    psh_assert((stack.previous_offset - sizeof(StackHeader)) % alignof(StackHeader) == zero);
    psh_assert(stack.previous_offset % a3_alignment == zero);
    psh_assert(
        static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a3) - stack_buf_diff);

    constexpr usize a4_alignment = sizeof(u8);
    u8* const       a4           = stack.alloc<u8>(49);
    psh_assert(a4 != nullptr);
    psh_assert((stack.previous_offset - sizeof(StackHeader)) % alignof(StackHeader) == zero);
    psh_assert(stack.previous_offset % a4_alignment == zero);
    psh_assert(
        static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a4) - stack_buf_diff);

    constexpr usize a5_alignment = sizeof(u32);
    u32* const      a5           = stack.alloc<u32>(8);
    psh_assert(a5 != nullptr);
    psh_assert((stack.previous_offset - sizeof(StackHeader)) % alignof(StackHeader) == zero);
    psh_assert(stack.previous_offset % a5_alignment == zero);
    psh_assert(
        static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a5) - stack_buf_diff);

    constexpr usize a6_alignment = sizeof(StrPtr);
    StrPtr const    a6           = stack.alloc<char>(14);
    psh_assert(a6 != nullptr);
    psh_assert((stack.previous_offset - sizeof(StackHeader)) % alignof(StackHeader) == zero);
    psh_assert(stack.previous_offset % a6_alignment == zero);
    psh_assert(
        static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a6) - stack_buf_diff);

    // Free a6.
    stack.pop();  // NOTE: a6 is now dangling.
    psh_assert(
        static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a5) - stack_buf_diff);

    // Free a3, a4, and a5 (they should all be dangling after the free).
    psh_assert(stack.clear_at(reinterpret_cast<u8*>(a3)));
    psh_assert(
        static_cast<iptr>(stack.previous_offset) == reinterpret_cast<iptr>(a2) - stack_buf_diff);

    // Free both a2 and a1.
    stack.clear();
    psh_assert(stack.previous_offset == zero);
    psh_assert(stack.offset == zero);
    psh_assert(stack.memory && (stack.capacity != 0));  // The memory should still be available.

    // Ensure we can allocate after freeing all blocks.
    i32* const  b1 = stack.alloc<int>(80);
    auto* const b2 = stack.alloc<double>(80);
    psh_assert(b1 != nullptr);
    psh_assert(b2 != nullptr);

    std::free(buf);
    log_passed(header);
}

void free_all() {
    constexpr StrPtr header = "[free_all]";

    usize const capacity = 512;
    u8* const   buf      = reinterpret_cast<u8*>(std::malloc(capacity));
    Stack       salloc{buf, capacity};

    usize expected_min_size = 0;

    usize      fib_size  = 30 * sizeof(u64);
    u64* const fibonacci = salloc.alloc<u64>(30);
    psh_assert(fibonacci != nullptr);
    expected_min_size += fib_size;
    fibonacci[0] = 1;
    fibonacci[1] = 1;
    for (u64 idx = 2; idx < 30; ++idx) {
        fibonacci[idx] = fibonacci[idx - 1] + fibonacci[idx - 2];
    }

    psh_assert(salloc.used() >= expected_min_size);

    usize      foos_size = 10 * sizeof(Foo);
    Foo* const foos      = salloc.alloc<Foo>(10);
    psh_assert(foos != nullptr);
    expected_min_size += foos_size;
    for (u32 idx = 0; idx < 10; ++idx) {
        foos[idx] = Foo{static_cast<f64>(idx) / 2.0, idx};
    }

    psh_assert(salloc.used() >= expected_min_size);

    salloc.clear();
    psh_assert(salloc.used() == 0ull);

    psh_assert_msg(salloc.offset == 0ull, "Expected empty StackAlloc");
    psh_assert_msg(salloc.previous_offset == 0ull, "Expected empty StackAlloc");

    std::free(buf);
    log_passed(header);
}

int main() {
    arena_scratch();
    stack_allocation_with_default_alignment();
    stack_offsets_reads_and_writes();
    stack_memory_stress_and_free();
    free_all();
    return 0;
}
