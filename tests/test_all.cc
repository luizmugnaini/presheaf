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
/// Description: Single compilation unit containing all Presheaf library tests.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

// Prevent tests from defining a `main()` function.
#define NOMAIN

// -----------------------------------------------------------------------------
// - Single compilation unit comprising the whole library implementation -
// -----------------------------------------------------------------------------

#include "../src/all.cc"

// -----------------------------------------------------------------------------
// - Invoke all test -
// -----------------------------------------------------------------------------

// Include all tests.
// clang-format off
#include "test_types.cc"
#include "test_bit.cc"
#include "test_repr.cc"
#include "test_vec.cc"
#include "test_allocators.cc"
#include "test_memory_manager.cc"
#include "test_dynarray.cc"
#include "test_string.cc"
// clang-format on

int main() {
    test_types();
    test_bit();
    test_repr();
    test_vec();
    test_allocators();
    test_memory_manager();
    test_dynarray();
    test_string();
    return 0;
}
