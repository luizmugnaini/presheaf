/// Tests for the dynamic array type (vector).
///
/// @author Luiz G. Mugnaini. A. <luizmugnaini@gmail.com>
#include <psh/assert.h>
#include <psh/dyn_array.h>
#include <psh/mem_utils.h>
#include <psh/memory_manager.h>
#include <psh/types.h>

#include "utils.h"

using namespace psh;

struct Foo {
    i32 bar;
};

void push_elements(MemoryManager& mem_manager) {
    StrPtr const  header = "[push_elements]";
    auto          arena  = mem_manager.make_arena(sizeof(i32) * 1024).demand();
    DynArray<i32> v{&arena};
    for (i32 i = 0; i < 100; ++i) {
        v.push(i);
        for (i32 j = 0; j < i; ++j) {
            auto uj = static_cast<usize>(j);
            psh_assert(v[uj] == j);
        }
    }

    mem_manager.pop();
    log_passed(header);
}

void size_and_capacity(MemoryManager& mem_manager) {
    StrPtr const  header = "[size_and_capacity]";
    Arena         arena  = mem_manager.make_arena(sizeof(Foo) * 100).demand();
    DynArray<Foo> v{&arena};

    v.push(Foo{0});

    usize last_capacity = v.capacity();
    for (i32 i = 2; i < 50; ++i) {
        v.push(Foo{i});

        usize const size = v.size();
        psh_assert(size == static_cast<usize>(i));

        usize const current_capacity = v.capacity();
        if (i == static_cast<i32>(last_capacity + 1)) {
            usize const expected_capacity = 2 * last_capacity;
            psh_assert(current_capacity == expected_capacity);
        } else {
            psh_assert(current_capacity == last_capacity);
        }
        last_capacity = current_capacity;
    }

    mem_manager.pop();
    log_passed(header);
}

void peek_and_pop(MemoryManager& mem_manager) {
    StrPtr const header = "[peek_and_pop]";

    auto          arena = mem_manager.make_arena(sizeof(i32) * 3).demand();
    DynArray<i32> v{&arena, 3};
    v.push(4), v.push(5), v.push(6);
    i32* p = nullptr;

    p = v.peek();
    psh_assert((p != nullptr) && (*p == 6));
    psh_assert(v.pop());

    p = v.peek();
    psh_assert((p != nullptr) && (*p == 5));
    psh_assert(v.pop());

    p = v.peek();
    psh_assert((p != nullptr) && (*p == 4));
    psh_assert(v.pop());

    psh_assert(v.size() == 0ull);

    mem_manager.pop();
    log_passed(header);
}

void remove(MemoryManager& mem_manager) {
    StrPtr const header = "[remove]";

    auto          arena = mem_manager.make_arena(sizeof(i32) * 5).demand();
    DynArray<i32> v{&arena, 5};
    v.push(4), v.push(7), v.push(8), v.push(9), v.push(55);
    i32* p = nullptr;

    psh_assert(v.size() == 5ull);
    psh_assert(v[0] == 4);
    psh_assert(v[1] == 7);
    psh_assert(v[2] == 8);
    psh_assert(v[3] == 9);
    psh_assert(v[4] == 55);
    p = v.peek();
    psh_assert((p != nullptr) && (*p == 55));

    psh_assert(v.remove(1));
    psh_assert(v.size() == 4ull);
    psh_assert(v[0] == 4);
    psh_assert(v[1] == 8);
    psh_assert(v[2] == 9);
    psh_assert(v[3] == 55);
    p = v.peek();
    psh_assert((p != nullptr) && (*p == 55));

    psh_assert(v.remove(2));
    psh_assert(v.size() == 3ull);
    psh_assert(v[0] == 4);
    psh_assert(v[1] == 8);
    psh_assert(v[2] == 55);
    p = v.peek();
    psh_assert((p != nullptr) && (*p == 55));

    psh_assert(v.remove(0));
    psh_assert(v.size() == 2ull);
    psh_assert(v[0] == 8);
    psh_assert(v[1] == 55);
    p = v.peek();
    psh_assert((p != nullptr) && (*p == 55));

    psh_assert(v.remove(1));
    psh_assert(v.size() == 1ull);
    psh_assert(v[0] == 8);
    p = v.peek();
    psh_assert((p != nullptr) && (*p == 8));

    psh_assert(v.remove(0));
    psh_assert(v.size() == 0ull);

    mem_manager.pop();
    log_passed(header);
}

void clear(MemoryManager& mem_manager) {
    StrPtr const header = "[clear]";

    auto          arena = mem_manager.make_arena(sizeof(f32) * 4).demand();
    DynArray<f32> v{&arena, 4};
    v.push(7.0f), v.push(4.8f), v.push(6.1f), v.push(3.14f);
    psh_assert_msg(v.size() == static_cast<usize>(4), "Expected vector size to be 4");
    v.clear();
    psh_assert_msg(v.size() == 0ull, "Expected vector size to be zero after clean");

    mem_manager.pop();
    log_passed(header);
}

i32 main() {
    MemoryManager mem_manager{10240};
    push_elements(mem_manager);
    size_and_capacity(mem_manager);
    peek_and_pop(mem_manager);
    remove(mem_manager);
    clear(mem_manager);
    return 0;
}
