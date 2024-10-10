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
/// Description: Tests for the fundamental types.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/assert.hpp>
#include <psh/core.hpp>
#include "utils.hpp"

void test_types() {
    psh_assert(sizeof(u8) == 1);
    psh_assert(sizeof(u16) == 2);
    psh_assert(sizeof(u32) == 4);
    psh_assert(sizeof(u64) == 8);
    psh_assert(sizeof(usize) == 8);
    psh_assert(sizeof(i8) == 1);
    psh_assert(sizeof(i16) == 2);
    psh_assert(sizeof(i32) == 4);
    psh_assert(sizeof(i64) == 8);
    psh_assert(sizeof(f32) == 4);
    psh_assert(sizeof(f64) == 8);
    psh_assert(sizeof(uptr) == 8);
    psh_assert(sizeof(iptr) == 8);
    test_passed();
}

#if !defined(NOMAIN)
int main() {
    test_types();
    return 0;
}
#endif
