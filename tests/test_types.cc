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
/// Description: Tests for the fundamental types.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/types.h>

#include <psh/assert.h>
#include "utils.h"

using namespace psh;

void expected_size_types() {
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

int main() {
    expected_size_types();
    return 0;
}
