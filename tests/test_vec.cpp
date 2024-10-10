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
/// Description: Tests for the linear algebra types.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/assert.hpp>
#include <psh/vec.hpp>
#include "utils.hpp"

void test_matrix_indexed_access() {
    psh::Mat3 mat3 = psh::Mat3::id();
    for (u32 row = 0; row < 3; ++row) {
        for (u32 column = 0; column < 3; ++column) {
            if (row != column) {
                psh_assert(psh::f32_approx_equal(mat3.at(row, column), 0.0f));
            } else {
                psh_assert(psh::f32_approx_equal(mat3.at(row, column), 1.0f));
            }
        }
    }

    psh::ColMat4 cmat4 = psh::ColMat4::id();
    for (u32 row = 0; row < 4; ++row) {
        for (u32 column = 0; column < 4; ++column) {
            if (row != column) {
                psh_assert(psh::f32_approx_equal(cmat4.at(row, column), 0.0f));
            } else {
                psh_assert(psh::f32_approx_equal(cmat4.at(row, column), 1.0f));
            }
        }
    }

    test_passed();
}

void test_vec() {
    test_matrix_indexed_access();
}

#if !defined(NOMAIN)
int main() {
    test_vec();
    return 0;
}
#endif
