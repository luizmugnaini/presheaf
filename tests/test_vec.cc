///                          Presheaf Library
///    Copyright (C) 2024 Luiz Gustavo Mugnaini Anselmo
///
///    This program is free software; you can redistribute it and/or modify
///    it under the terms of the GNU General Public License as published by
///    the Free Software Foundation; either version 2 of the License, or
///    (at your option) any later version.
///
///    This program is distributed in the hope that it will be useful,
///    but WITHOUT ANY WARRANTY; without even the implied warranty of
///    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///    GNU General Public License for more details.
///
///    You should have received a copy of the GNU General Public License along
///    with this program; if not, write to the Free Software Foundation, Inc.,
///    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
///
/// Description: Tests for the linear algebra types.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/assert.h>
#include <psh/vec.h>
#include "utils.h"

using namespace psh;

void test_matrix_indexed_access() {
    Mat3 mat3 = Mat3::id();
    for (u32 row = 0; row < 3; ++row) {
        for (u32 column = 0; column < 3; ++column) {
            if (row != column) {
                psh_assert(float_equal(mat3.at(row, column), 0.0f));
            } else {
                psh_assert(float_equal(mat3.at(row, column), 1.0f));
            }
        }
    }

    ColMat4 cmat4 = ColMat4::id();
    for (u32 row = 0; row < 4; ++row) {
        for (u32 column = 0; column < 4; ++column) {
            if (row != column) {
                psh_assert(float_equal(cmat4.at(row, column), 0.0f));
            } else {
                psh_assert(float_equal(cmat4.at(row, column), 1.0f));
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
