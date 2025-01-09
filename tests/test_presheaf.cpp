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

// Prevent tests from defining a main() function.
#define PSH_TEST_NOMAIN

// -------------------------------------------------------------------------------------------------
// Single compilation unit comprising the whole library implementation.
// -------------------------------------------------------------------------------------------------

#include "../src/presheaf.cpp"

// -------------------------------------------------------------------------------------------------
// Invoke all test.
// -------------------------------------------------------------------------------------------------

// Include all tests.
// clang-format off
#include "test_types.cpp"
#include "test_bit.cpp"
#include "test_repr.cpp"
#include "test_vec.cpp"
#include "test_allocators.cpp"
#include "test_memory_manager.cpp"
#include "test_containers.cpp"
#include "test_string.cpp"
#include "test_algorithms.cpp"
#include "test_time.cpp"
#include "test_logging.cpp"
// clang-format on

int main() {
    psh::test::types::run_all();
    psh::test::bit::run_all();
    psh::test::repr::run_all();
    psh::test::vec::run_all();
    psh::test::allocators::run_all();
    psh::test::memory_manager::run_all();
    psh::test::containers::run_all();
    psh::test::string::run_all();
    psh::test::algorithms::run_all();
    psh::test::time::run_all();
    psh::test::logging::run_all();
    return 0;
}
