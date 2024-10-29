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
/// Description: Tests for the bit twiddling utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/algorithms.hpp>
#include <psh/assert.hpp>
#include "utils.hpp"

namespace psh::test::algorithms {
    struct Fraction {
        i32 x;
        i32 y;
    };

    psh_internal bool equal_fractions(Fraction lhs, Fraction rhs) {
        return (lhs.x * rhs.y == lhs.y * rhs.x);
    }

    psh_internal void contains() {
        Buffer<Fraction, 4>    buf      = {Fraction{3, 3}, Fraction{12, 3}, Fraction{2, 4}, Fraction{60, 8}};
        FatPtr<Fraction const> buf_fptr = const_fat_ptr(buf);

        psh_assert(contains(Fraction{4, 1}, buf_fptr, equal_fractions));
        psh_assert(contains(Fraction{1, 1}, buf_fptr, equal_fractions));
        psh_assert(contains(Fraction{1, 2}, buf_fptr, equal_fractions));
        psh_assert(contains(Fraction{30, 4}, buf_fptr, equal_fractions));
        psh_assert(contains(Fraction{15, 2}, buf_fptr, equal_fractions));

        report_test_successful();
    }

    psh_internal void run_all() {
        psh::test::algorithms::contains();
    }

}  // namespace psh::test::algorithms

#if !defined(PSH_TEST_NOMAIN)
int main() {
    psh::test::algorithms::run_all();
    return 0;
}
#endif
