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
#include <psh/dyn_array.hpp>
#include <psh/memory.hpp>
#include <psh/memory_manager.hpp>
#include "utils.hpp"

namespace psh::test::dyn_array {
    struct Foo {
        i32 bar;
    };

    psh_internal void push_elements(MemoryManager& memory_manager) {
        Arena arena = memory_manager.make_arena(sizeof(i32) * 1024).demand();

        DynArray<i32> v{&arena};

        for (i32 i = 0; i < 100; ++i) {
            psh_assert(v.push(i));
            for (i32 j = 0; j < i; ++j) {
                usize uj = static_cast<usize>(j);
                psh_assert(v[uj] == j);
            }
        }

        memory_manager.pop();
        report_test_successful();
    }

    psh_internal void count_and_capacity(MemoryManager& memory_manager) {
        Arena arena = memory_manager.make_arena(sizeof(Foo) * 100).demand();

        DynArray<Foo> v{&arena};
        psh_assert(v.push(Foo{0}));

        usize last_capacity = v.capacity;
        for (i32 i = 2; i < 50; ++i) {
            psh_assert(v.push(Foo{i}));

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

    psh_internal void peek_and_pop(MemoryManager& memory_manager) {
        Arena arena = memory_manager.make_arena(sizeof(i32) * 3).demand();

        DynArray<i32> v{&arena, 3};
        {
            Buffer<i32, 2> elements = {4, 5};
            psh_assert(v.push(make_const_fat_ptr(elements)));
        }
        psh_assert(v.push(6));

        // Peek then pop 6.
        {
            i32* p = v.peek();
            psh_assert((p != nullptr) && (*p == 6));
            psh_assert(v.pop());
        }

        // Peek then pop 5;
        {
            i32* p = v.peek();
            psh_assert((p != nullptr) && (*p == 5));
            psh_assert(v.pop());
        }

        // Peek then pop 6.
        {
            i32* p = v.peek();
            psh_assert((p != nullptr) && (*p == 4));
            psh_assert(v.pop());
        }

        psh_assert(v.count == 0ull);

        memory_manager.pop();
        report_test_successful();
    }

    psh_internal void remove(MemoryManager& memory_manager) {
        Arena arena = memory_manager.make_arena(sizeof(i32) * 5).demand();

        DynArray<i32> v{&arena, 5};

        // Populate the array.
        psh_assert(v.push(4));
        psh_assert(v.push(7));
        {
            i32 last_two[3] = {8, 9, 55};
            psh_assert(v.push({last_two, 3}));
        }

        // Do nothing.
        {
            psh_assert(v.count == 5);
            psh_assert(v[0] == 4);
            psh_assert(v[1] == 7);
            psh_assert(v[2] == 8);
            psh_assert(v[3] == 9);
            psh_assert(v[4] == 55);
            i32* p = v.peek();
            psh_assert((p != nullptr) && (*p == 55));
        }

        // Remove at index 1.
        {
            psh_assert(v.remove(1));
            psh_assert(v.count == 4);
            psh_assert(v[0] == 4);
            psh_assert(v[1] == 8);
            psh_assert(v[2] == 9);
            psh_assert(v[3] == 55);
            i32* p = v.peek();
            psh_assert((p != nullptr) && (*p == 55));
        }

        // Remove at index 2.
        {
            psh_assert(v.remove(2));
            psh_assert(v.count == 3);
            psh_assert(v[0] == 4);
            psh_assert(v[1] == 8);
            psh_assert(v[2] == 55);
            i32* p = v.peek();
            psh_assert((p != nullptr) && (*p == 55));
        }

        // Remove at index 0.
        {
            psh_assert(v.remove(0));
            psh_assert(v.count == 2);
            psh_assert(v[0] == 8);
            psh_assert(v[1] == 55);
            i32* p = v.peek();
            psh_assert((p != nullptr) && (*p == 55));
        }

        // Remove at index 1.
        {
            psh_assert(v.remove(1));
            psh_assert(v.count == 1);
            psh_assert(v[0] == 8);
            i32* p = v.peek();
            psh_assert((p != nullptr) && (*p == 8));
        }

        // Remove at index 0.
        {
            psh_assert(v.remove(0));
            psh_assert(v.count == 0);
        }

        memory_manager.pop();
        report_test_successful();
    }

    psh_internal void clear(MemoryManager& memory_manager) {
        Arena arena = memory_manager.make_arena(sizeof(f32) * 4).demand();

        DynArray<f32> v{&arena, 4};
        {
            f32 elements[4] = {7.0f, 4.8f, 6.1f, 3.14f};
            psh_assert(v.push({elements, 4}));
        }

        psh_assert(v.count == static_cast<usize>(4));

        v.clear();
        psh_assert(v.count == 0);
        psh_assert(v.capacity == 4);

        memory_manager.pop();
        report_test_successful();
    }

    psh_internal void run_all() {
        MemoryManager memory_manager;
        memory_manager.init(10240);

        push_elements(memory_manager);
        count_and_capacity(memory_manager);
        peek_and_pop(memory_manager);
        remove(memory_manager);
        clear(memory_manager);
    }
}  // namespace psh::test::dyn_array

#if !defined(PSH_TEST_NOMAIN)
int main() {
    psh::test::dyn_array::run_all();
    return 0;
}
#endif
