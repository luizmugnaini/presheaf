/// Tests for the internal types.
///
/// Tests for the expected sizes of the type definitions used by the internals.
///
/// @author Luiz G. Mugnaini. A. <luizmugnaini@gmail.com>
#include <psh/assert.h>
#include <psh/types.h>

#include "utils.h"

using namespace psh;

void expected_size_types() {
    StrPtr const header = "[expected_size_types]";
    psh_assert(sizeof(u8) == static_cast<usize>(1));
    psh_assert(sizeof(u16) == static_cast<usize>(2));
    psh_assert(sizeof(u32) == static_cast<usize>(4));
    psh_assert(sizeof(u64) == static_cast<usize>(8));

    psh_assert(sizeof(usize) == static_cast<usize>(8));

    psh_assert(sizeof(i8) == static_cast<usize>(1));
    psh_assert(sizeof(i16) == static_cast<usize>(2));
    psh_assert(sizeof(i32) == static_cast<usize>(4));
    psh_assert(sizeof(i64) == static_cast<usize>(8));

    psh_assert(sizeof(f32) == static_cast<usize>(4));
    psh_assert(sizeof(f64) == static_cast<usize>(8));

    psh_assert(sizeof(uptr) == static_cast<usize>(8));
    psh_assert(sizeof(iptr) == static_cast<usize>(8));

    log_passed(header);
}

int main() {
    expected_size_types();
    return 0;
}
