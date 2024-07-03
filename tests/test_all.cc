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
#include "test_vec.cc"
#include "test_allocators.cc"
#include "test_memory_manager.cc"
#include "test_dynarray.cc"
#include "test_string.cc"
// clang-format on

int main() {
    test_types();
    test_bit();
    test_vec();
    test_allocators();
    test_memory_manager();
    test_dynarray();
    test_string();
    return 0;
}
