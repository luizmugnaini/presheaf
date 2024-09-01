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

#include <psh/assert.hh>
#include <psh/core.hh>
#include <psh/dyn_array.hh>
#include <psh/memory_utils.hh>
#include <psh/memory_manager.hh>
#include "utils.hh"

struct Foo {
    i32 bar;
};

void test_dynarray_push_elements(psh::MemoryManager& mem_manager) {
    psh::Arena         arena = mem_manager.make_arena(sizeof(i32) * 1024).demand();
    psh::DynArray<i32> v{&arena};
    for (i32 i = 0; i < 100; ++i) {
        v.push(i);
        for (i32 j = 0; j < i; ++j) {
            usize uj = static_cast<usize>(j);
            psh_assert(v[uj] == j);
        }
    }

    mem_manager.pop();
    test_passed();
}

void test_dynarray_size_and_capacity(psh::MemoryManager& mem_manager) {
    psh::Arena         arena = mem_manager.make_arena(sizeof(Foo) * 100).demand();
    psh::DynArray<Foo> v{&arena};

    v.push(Foo{0});

    usize last_capacity = v.capacity;
    for (i32 i = 2; i < 50; ++i) {
        v.push(Foo{i});

        usize const size = v.size;
        psh_assert(size == static_cast<usize>(i));

        usize const current_capacity = v.capacity;
        if (i == static_cast<i32>(last_capacity + 1)) {
            usize const expected_capacity = 2 * last_capacity;
            psh_assert(current_capacity == expected_capacity);
        } else {
            psh_assert(current_capacity == last_capacity);
        }
        last_capacity = current_capacity;
    }

    mem_manager.pop();
    test_passed();
}

void test_dynarray_peek_and_pop(psh::MemoryManager& mem_manager) {
    psh::Arena         arena = mem_manager.make_arena(sizeof(i32) * 3).demand();
    psh::DynArray<i32> v{&arena, 3};
    v.push(4), v.push(5), v.push(6);
    i32* p = nullptr;

    p = v.peek();
    psh_assert((p != nullptr) && (*p == 6));
    psh_assert(v.pop() == psh::Status::OK);

    p = v.peek();
    psh_assert((p != nullptr) && (*p == 5));
    psh_assert(v.pop() == psh::Status::OK);

    p = v.peek();
    psh_assert((p != nullptr) && (*p == 4));
    psh_assert(v.pop() == psh::Status::OK);

    psh_assert(v.size == 0ull);

    mem_manager.pop();
    test_passed();
}

void test_dynarray_remove(psh::MemoryManager& mem_manager) {
    psh::Arena         arena = mem_manager.make_arena(sizeof(i32) * 5).demand();
    psh::DynArray<i32> v{&arena, 5};
    v.push(4), v.push(7), v.push(8), v.push(9), v.push(55);
    i32* p = nullptr;

    psh_assert(v.size == 5ull);
    psh_assert(v[0] == 4);
    psh_assert(v[1] == 7);
    psh_assert(v[2] == 8);
    psh_assert(v[3] == 9);
    psh_assert(v[4] == 55);
    p = v.peek();
    psh_assert((p != nullptr) && (*p == 55));

    psh_assert(v.remove(1) == psh::Status::OK);
    psh_assert(v.size == 4ull);
    psh_assert(v[0] == 4);
    psh_assert(v[1] == 8);
    psh_assert(v[2] == 9);
    psh_assert(v[3] == 55);
    p = v.peek();
    psh_assert((p != nullptr) && (*p == 55));

    psh_assert(v.remove(2) == psh::Status::OK);
    psh_assert(v.size == 3ull);
    psh_assert(v[0] == 4);
    psh_assert(v[1] == 8);
    psh_assert(v[2] == 55);
    p = v.peek();
    psh_assert((p != nullptr) && (*p == 55));

    psh_assert(v.remove(0) == psh::Status::OK);
    psh_assert(v.size == 2ull);
    psh_assert(v[0] == 8);
    psh_assert(v[1] == 55);
    p = v.peek();
    psh_assert((p != nullptr) && (*p == 55));

    psh_assert(v.remove(1) == psh::Status::OK);
    psh_assert(v.size == 1ull);
    psh_assert(v[0] == 8);
    p = v.peek();
    psh_assert((p != nullptr) && (*p == 8));

    psh_assert(v.remove(0) == psh::Status::OK);
    psh_assert(v.size == 0ull);

    mem_manager.pop();
    test_passed();
}

void test_dynarray_clear(psh::MemoryManager& mem_manager) {
    psh::Arena         arena = mem_manager.make_arena(sizeof(f32) * 4).demand();
    psh::DynArray<f32> v{&arena, 4};
    v.push(7.0f), v.push(4.8f), v.push(6.1f), v.push(3.14f);
    psh_assert_msg(v.size == static_cast<usize>(4), "Expected vector size to be 4");
    v.clear();
    psh_assert_msg(v.size == 0ull, "Expected vector size to be zero after clean");

    mem_manager.pop();
    test_passed();
}

void test_dynarray() {
    psh::MemoryManager mem_manager{10240};
    test_dynarray_push_elements(mem_manager);
    test_dynarray_size_and_capacity(mem_manager);
    test_dynarray_peek_and_pop(mem_manager);
    test_dynarray_remove(mem_manager);
    test_dynarray_clear(mem_manager);
}

#if !defined(NOMAIN)
int main() {
    test_dynarray();
    return 0;
}
#endif
