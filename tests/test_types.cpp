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

#include "utils.hpp"

namespace psh::test::types {
    psh_internal void sizes() {
        psh_assert_constexpr(psh_usize_of(u8) == 1);
        psh_assert_constexpr(psh_usize_of(u16) == 2);
        psh_assert_constexpr(psh_usize_of(u32) == 4);
        psh_assert_constexpr(psh_usize_of(u64) == 8);
        psh_assert_constexpr(psh_usize_of(usize) == 8);
        psh_assert_constexpr(psh_usize_of(i8) == 1);
        psh_assert_constexpr(psh_usize_of(i16) == 2);
        psh_assert_constexpr(psh_usize_of(i32) == 4);
        psh_assert_constexpr(psh_usize_of(i64) == 8);
        psh_assert_constexpr(psh_usize_of(f32) == 4);
        psh_assert_constexpr(psh_usize_of(f64) == 8);
        psh_assert_constexpr(psh_usize_of(uptr) == 8);
        psh_assert_constexpr(psh_usize_of(iptr) == 8);

        report_test_successful();
    }

    psh_internal void run_all() {
        sizes();
    }
}  // namespace psh::test::types

#if !defined(PSH_TEST_NOMAIN)
int main() {
    psh::test::types::run_all();
    return 0;
}
#endif
