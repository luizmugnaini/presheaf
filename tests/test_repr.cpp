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
/// Description: Tests for number representation conversions utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/assert.hpp>
#include <psh/core.hpp>
#include <psh/repr.hpp>
#include "utils.hpp"

#include <stdlib.h>

namespace psh::test::repr {
    psh_internal void binary_representation() {
        u8*   memory = reinterpret_cast<u8*>(malloc(256));
        Arena arena{memory, 256};
        {
            String repr0 = binary_repr(&arena, 0b0);
            psh_assert(string_equal(make_string_view(repr0), "0b0"));

            String repr1 = binary_repr(&arena, 0b010);
            psh_assert(string_equal(make_string_view(repr1), "0b10"));

            String repr2 = binary_repr(&arena, 0b11010);
            psh_assert(string_equal(make_string_view(repr2), "0b11010"));

            String repr3 = binary_repr(&arena, 0b000111110101010101011);
            psh_assert(string_equal(make_string_view(repr3), "0b111110101010101011"));
        }
        free(memory);

        report_test_successful();
    }

    psh_internal void run_all() {
        binary_representation();
    }
}  // namespace psh::test::repr

#if !defined(PSH_TEST_NOMAIN)
int main() {
    psh::test::repr::run_all();
    return 0;
}
#endif
