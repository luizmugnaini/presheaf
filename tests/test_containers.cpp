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
/// Description: Tests for the dynamic array.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/assert.hpp>
#include <psh/core.hpp>
#include <psh/memory.hpp>
#include "utils.hpp"

namespace psh::test::containers {
    struct Foo {
        i32 bar;
    };

    psh_internal void c_array_count() {
        u32 values[] = {0, 0, 1, 1, 1};
        psh_assert(count_of(values) == 5);

        char const str_array[]    = "this is a string";
        usize      expected_count = cstring_length(str_array) + 1;  // count_of will account for the zero-terminator.
        psh_assert(count_of(str_array) == expected_count);
        psh_assert(count_of("this is a string") == expected_count);
        psh_assert(count_of("this is a string") == psh_usize_of("this is a string"));
    }

    psh_internal void dynarray_push_elements(MemoryManager& memory_manager) {
        Arena arena;
        {
            usize arena_capacity = sizeof(i32) * 1024;
            arena_init(&arena, memory_alloc<u8>(&memory_manager, arena_capacity), arena_capacity);
        }

        DynArray<i32> v = make_dynarray<i32>(&arena);

        for (i32 i = 0; i < 100; ++i) {
            psh_assert(dynarray_push(&v, i));
            for (i32 j = 0; j < i; ++j) {
                psh_assert(v[static_cast<usize>(j)] == j);
            }
        }

        memory_manager.pop();
        report_test_successful();
    }

    psh_internal void dynarray_count_and_capacity(MemoryManager& memory_manager) {
        Arena arena;
        {
            usize arena_capacity = sizeof(Foo) * 100;
            arena_init(&arena, memory_alloc<u8>(&memory_manager, arena_capacity), arena_capacity);
        }

        DynArray<Foo> v = make_dynarray<Foo>(&arena);
        psh_assert(dynarray_push(&v, Foo{0}));

        usize last_capacity = v.capacity;
        for (i32 i = 2; i < 50; ++i) {
            psh_assert(dynarray_push(&v, Foo{i}));

            usize const count = v.count;
            psh_assert(count == static_cast<usize>(i));

            usize const current_capacity = v.capacity;
            if (i == static_cast<i32>(last_capacity + 1)) {
                usize const expected_capacity = 2 * last_capacity;
                psh_assert(current_capacity == expected_capacity);
            } else {
                psh_assert(current_capacity == last_capacity);
            }
            last_capacity = current_capacity;
        }

        memory_manager.pop();
        report_test_successful();
    }

    psh_internal void dynarray_peek_and_pop(MemoryManager& memory_manager) {
        Arena arena;
        {
            usize arena_capacity = sizeof(i32) * 3;
            arena_init(&arena, memory_alloc<u8>(&memory_manager, arena_capacity), arena_capacity);
        }

        DynArray<i32> v = make_dynarray<i32>(&arena, 3);
        {
            Buffer<i32, 2> elements = {4, 5};
            psh_assert(dynarray_push(&v, make_const_fat_ptr(elements)));
        }
        psh_assert(dynarray_push(&v, 6));

        // Peek then pop 6.
        {
            psh_assert(v.count == 3);
            psh_assert(v[2] == 6);
            psh_assert(dynarray_pop(&v));
        }

        // Peek then pop 5;
        {
            psh_assert(v.count == 2);
            psh_assert(v[1] == 5);
            psh_assert(dynarray_pop(&v));
        }

        // Peek then pop 6.
        {
            psh_assert(v.count == 1);
            psh_assert(v[0] == 4);
            psh_assert(dynarray_pop(&v));
        }

        psh_assert(v.count == 0);

        memory_manager.pop();
        report_test_successful();
    }

    psh_internal void dynarray_remove(MemoryManager& memory_manager) {
        Arena arena;
        {
            usize arena_capacity = sizeof(i32) * 5;
            arena_init(&arena, memory_alloc<u8>(&memory_manager, arena_capacity), arena_capacity);
        }

        DynArray<i32> v = make_dynarray<i32>(&arena, 5);

        // Populate the array.
        psh_assert(dynarray_push(&v, 4));
        psh_assert(dynarray_push(&v, 7));
        {
            i32 last_two[3] = {8, 9, 55};
            psh_assert(dynarray_push(&v, {last_two, count_of(last_two)}));
        }

        // Do nothing.
        {
            psh_assert(v.count == 5);
            psh_assert(v[0] == 4);
            psh_assert(v[1] == 7);
            psh_assert(v[2] == 8);
            psh_assert(v[3] == 9);
            psh_assert(v[4] == 55);
        }

        // Remove at index 1.
        {
            ordered_remove(&v, 1);
            psh_assert(v.count == 4);
            psh_assert(v[0] == 4);
            psh_assert(v[1] == 8);
            psh_assert(v[2] == 9);
            psh_assert(v[3] == 55);
        }

        // Remove at index 2.
        {
            ordered_remove(&v, 2);
            psh_assert(v.count == 3);
            psh_assert(v[0] == 4);
            psh_assert(v[1] == 8);
            psh_assert(v[2] == 55);
        }

        // Remove at index 0.
        {
            ordered_remove(&v, 0);
            psh_assert(v.count == 2);
            psh_assert(v[0] == 8);
            psh_assert(v[1] == 55);
        }

        // Remove at index 1.
        {
            ordered_remove(&v, 1);
            psh_assert(v.count == 1);
            psh_assert(v[0] == 8);
        }

        // Remove at index 0.
        {
            ordered_remove(&v, 0);
            psh_assert(v.count == 0);
        }

        memory_manager.pop();
        report_test_successful();
    }

    psh_internal void dynarray_clear(MemoryManager& memory_manager) {
        Arena arena;
        {
            usize arena_capacity = sizeof(f32) * 4;
            arena_init(&arena, memory_alloc<u8>(&memory_manager, arena_capacity), arena_capacity);
        }

        DynArray<f32> v = make_dynarray<f32>(&arena, 4);
        {
            f32 elements[4] = {7.0f, 4.8f, 6.1f, 3.14f};
            psh_assert(dynarray_push(&v, {elements, count_of(elements)}));
        }

        psh_assert(v.count == 4);

        dynarray_clear(&v);
        psh_assert(v.count == 0);
        psh_assert(v.capacity == 4);

        memory_manager.pop();
        report_test_successful();
    }

    psh_internal void run_all() {
        MemoryManager memory_manager;
        memory_manager.init(10240);

        c_array_count();
        dynarray_push_elements(memory_manager);
        dynarray_count_and_capacity(memory_manager);
        dynarray_peek_and_pop(memory_manager);
        dynarray_remove(memory_manager);
        dynarray_clear(memory_manager);
    }
}  // namespace psh::test::containers

#if !defined(PSH_TEST_NOMAIN)
int main() {
    psh::test::containers::run_all();
    return 0;
}
#endif
