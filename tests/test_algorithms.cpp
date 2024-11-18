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

#include <stdlib.h>  // For rand.
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

    template <typename T>
    psh_internal bool test_sort(FatPtr<T> fptr) {
        bool sorted = true;
        for (usize idx = 0; idx < fptr.count - 1; ++idx) {
            sorted &= (fptr[idx] <= fptr[idx + 1]);
        }
        return sorted;
    }

    psh_internal void insertion_sort() {
        // Already sorted.
        {
            Buffer<f32, 9> buf  = {1.0f, 1.2f, 3.0f, 3.3f, 3.3f, 99.04f, 100.0f, 155.56f, 779.189f};
            FatPtr<f32>    fptr = psh::make_fat_ptr(buf);

            psh::insertion_sort(fptr);
            psh_assert(test_sort(fptr));
        }

        // Reverse sorted.
        {
            Buffer<f32, 12> buf  = {779.189f, 155.56f, 100.0f, 99.04f, 3.3f, 3.3f, 3.0f, 1.2f, 1.0f, -3.56f, -60.005f};
            FatPtr<f32>     fptr = psh::make_fat_ptr(buf);

            psh::insertion_sort(fptr);
            psh_assert(test_sort(fptr));
        }

        // Random.
        {
            Buffer<int32_t, 256> buf;
            FatPtr<i32>          fptr = psh::make_fat_ptr(buf);
            for (u32 iter = 0; iter < 20; ++iter) {
                for (i32& v : buf) {
                    v = rand();
                }
                psh::insertion_sort(fptr);
                psh_assert(test_sort(fptr));
            }
        }

        report_test_successful();
    }

    psh_internal void quick_sort() {
        // Already sorted.
        {
            Buffer<f32, 9> buf  = {1.0f, 1.2f, 3.0f, 3.3f, 3.3f, 99.04f, 100.0f, 155.56f, 779.189f};
            FatPtr<f32>    fptr = psh::make_fat_ptr(buf);

            psh::quick_sort(fptr);
            psh_assert(test_sort(fptr));
        }

        // Reverse sorted.
        {
            Buffer<f32, 12> buf  = {779.189f, 155.56f, 100.0f, 99.04f, 3.3f, 3.3f, 3.0f, 1.2f, 1.0f, -3.56f, -60.005f};
            FatPtr<f32>     fptr = psh::make_fat_ptr(buf);

            psh::quick_sort(fptr);
            psh_assert(test_sort(fptr));
        }

        // Random.
        {
            Buffer<int32_t, 256> buf;
            FatPtr<i32>          fptr = psh::make_fat_ptr(buf);
            for (u32 iter = 0; iter < 20; ++iter) {
                for (i32& v : buf) {
                    v = rand();
                }
                psh::quick_sort(fptr);
                psh_assert(test_sort(fptr));
            }
        }

        report_test_successful();
    }

    psh_internal void contains() {
        // Default matcher.
        {
            Buffer<i64, 10>   buf  = {-192381LL, -281937123LL, 123LL, 0LL, 3LL, 90LL, 23198237LL, 127391723LL, 123769128LL, 5LL};
            FatPtr<i64 const> fptr = make_const_fat_ptr(buf);

            // Check exact buffer elements.
            psh_assert(psh::contains(fptr, -192381LL));
            psh_assert(psh::contains(fptr, -281937123LL));
            psh_assert(psh::contains(fptr, 123LL));
            psh_assert(psh::contains(fptr, 0LL));
            psh_assert(psh::contains(fptr, 3LL));
            psh_assert(psh::contains(fptr, 90LL));
            psh_assert(psh::contains(fptr, 23198237LL));
            psh_assert(psh::contains(fptr, 127391723LL));
            psh_assert(psh::contains(fptr, 123769128LL));
            psh_assert(psh::contains(fptr, 5LL));

            // Non-matching elements.
            psh_assert(!psh::contains(fptr, 192381LL));
            psh_assert(!psh::contains(fptr, -28193LL));
            psh_assert(!psh::contains(fptr, 13LL));
            psh_assert(!psh::contains(fptr, 1LL));
            psh_assert(!psh::contains(fptr, 50850LL));
        }

        // Custom function.
        {
            Buffer<Fraction, 4>    buf  = {Fraction{3, 3}, Fraction{12, 3}, Fraction{2, 4}, Fraction{60, 8}};
            FatPtr<Fraction const> fptr = make_const_fat_ptr(buf);

            // Check the exact fractions in buf.
            psh_assert(psh::contains(fptr, Fraction{3, 3}, equal_fractions));
            psh_assert(psh::contains(fptr, Fraction{12, 3}, equal_fractions));
            psh_assert(psh::contains(fptr, Fraction{2, 4}, equal_fractions));
            psh_assert(psh::contains(fptr, Fraction{60, 8}, equal_fractions));

            // Check equivalent fractions.
            psh_assert(psh::contains(fptr, Fraction{1, 1}, equal_fractions));   //  3 / 3
            psh_assert(psh::contains(fptr, Fraction{4, 1}, equal_fractions));   // 12 / 3
            psh_assert(psh::contains(fptr, Fraction{1, 2}, equal_fractions));   //  2 / 4
            psh_assert(psh::contains(fptr, Fraction{30, 4}, equal_fractions));  // 60 / 8
            psh_assert(psh::contains(fptr, Fraction{15, 2}, equal_fractions));  // 60 / 8

            // Non-matching fractions.
            psh_assert(!psh::contains(fptr, Fraction{2, 1}, equal_fractions));
            psh_assert(!psh::contains(fptr, Fraction{4, 3}, equal_fractions));
            psh_assert(!psh::contains(fptr, Fraction{1, 4}, equal_fractions));
            psh_assert(!psh::contains(fptr, Fraction{27, 5}, equal_fractions));
            psh_assert(!psh::contains(fptr, Fraction{0, 2}, equal_fractions));
        }

        report_test_successful();
    }

    psh_internal void linear_search() {
        // Default matcher.
        {
            Buffer<u64, 11>   buf  = {192381ull, 281937123ull, 123ull, 0ull, 0ull, 3ull, 90ull, 23198237ull, 127391723ull, 123769128ull, 5ull};
            FatPtr<u64 const> fptr = make_const_fat_ptr(buf);

            // Exact elements.
            psh_assert(psh::linear_search(fptr, 192381ull).demand() == 0);
            psh_assert(psh::linear_search(fptr, 281937123ull).demand() == 1);
            psh_assert(psh::linear_search(fptr, 123ull).demand() == 2);
            psh_assert(psh::linear_search(fptr, 0ull).demand() == 3);
            psh_assert(psh::linear_search(fptr, 3ull).demand() == 5);
            psh_assert(psh::linear_search(fptr, 90ull).demand() == 6);
            psh_assert(psh::linear_search(fptr, 23198237ull).demand() == 7);
            psh_assert(psh::linear_search(fptr, 127391723ull).demand() == 8);
            psh_assert(psh::linear_search(fptr, 123769128ull).demand() == 9);
            psh_assert(psh::linear_search(fptr, 5ull).demand() == 10);

            // Non-existent elements.
            psh_assert(!psh::linear_search(fptr, 92381ull).has_value);
            psh_assert(!psh::linear_search(fptr, 28197123ull).has_value);
            psh_assert(!psh::linear_search(fptr, 12ull).has_value);
            psh_assert(!psh::linear_search(fptr, 1ull).has_value);
        }

        // Custom function.
        {
            Buffer<Fraction, 4>    buf  = {Fraction{3, 3}, Fraction{12, 3}, Fraction{2, 4}, Fraction{60, 8}};
            FatPtr<Fraction const> fptr = make_const_fat_ptr(buf);

            // Check the exact fractions in buf.
            psh_assert(psh::linear_search(fptr, Fraction{3, 3}, equal_fractions).demand() == 0);
            psh_assert(psh::linear_search(fptr, Fraction{12, 3}, equal_fractions).demand() == 1);
            psh_assert(psh::linear_search(fptr, Fraction{2, 4}, equal_fractions).demand() == 2);
            psh_assert(psh::linear_search(fptr, Fraction{60, 8}, equal_fractions).demand() == 3);

            // Check equivalent fractions.
            psh_assert(psh::linear_search(fptr, Fraction{1, 1}, equal_fractions).demand() == 0);   //  3 / 3
            psh_assert(psh::linear_search(fptr, Fraction{4, 1}, equal_fractions).demand() == 1);   // 12 / 3
            psh_assert(psh::linear_search(fptr, Fraction{1, 2}, equal_fractions).demand() == 2);   //  2 / 4
            psh_assert(psh::linear_search(fptr, Fraction{30, 4}, equal_fractions).demand() == 3);  // 60 / 8
            psh_assert(psh::linear_search(fptr, Fraction{15, 2}, equal_fractions).demand() == 3);  // 60 / 8

            // Non-matching fractions.
            psh_assert(!psh::linear_search(fptr, Fraction{2, 1}, equal_fractions).has_value);
            psh_assert(!psh::linear_search(fptr, Fraction{4, 3}, equal_fractions).has_value);
            psh_assert(!psh::linear_search(fptr, Fraction{1, 4}, equal_fractions).has_value);
            psh_assert(!psh::linear_search(fptr, Fraction{27, 5}, equal_fractions).has_value);
            psh_assert(!psh::linear_search(fptr, Fraction{0, 2}, equal_fractions).has_value);
        }
        report_test_successful();
    }

    psh_internal void binary_search() {
        // Basic test.
        {
            Buffer<i64, 10>   buf  = {-12837LL, -123LL, -72LL, -1LL, 0LL, 0LL, 19LL, 192381LL, 281937123LL, 55518237198LL};
            FatPtr<i64 const> fptr = make_const_fat_ptr(buf);

            // Exact elements.
            psh_assert(psh::binary_search(fptr, -12837LL).demand() == 0);
            psh_assert(psh::binary_search(fptr, -123LL).demand() == 1);
            psh_assert(psh::binary_search(fptr, -72LL).demand() == 2);
            psh_assert(psh::binary_search(fptr, -1LL).demand() == 3);
            psh_assert(psh::binary_search(fptr, 0LL).demand() == 4);
            psh_assert(psh::binary_search(fptr, 19LL).demand() == 6);
            psh_assert(psh::binary_search(fptr, 192381LL).demand() == 7);
            psh_assert(psh::binary_search(fptr, 281937123LL).demand() == 8);
            psh_assert(psh::binary_search(fptr, 55518237198LL).demand() == 9);

            // Non-existent elements.
            psh_assert(!psh::binary_search(fptr, 92381LL).has_value);
            psh_assert(!psh::binary_search(fptr, 28197123LL).has_value);
            psh_assert(!psh::binary_search(fptr, 12LL).has_value);
            psh_assert(!psh::binary_search(fptr, 1LL).has_value);
        }

        // Random test.
        {
            Buffer<i32, 256> buf;
            for (u32 iter = 0; iter < 20; ++iter) {
                for (i32& v : buf) {
                    v = rand();
                }
                psh::quick_sort(make_fat_ptr(buf));

                for (i32 v : buf) {
                    psh_assert(psh::binary_search(make_const_fat_ptr(buf), v).has_value);
                }
            }
        }

        report_test_successful();
    }

    psh_internal void run_all() {
        psh::test::algorithms::insertion_sort();
        psh::test::algorithms::quick_sort();
        psh::test::algorithms::contains();
        psh::test::algorithms::linear_search();
        psh::test::algorithms::binary_search();
    }
}  // namespace psh::test::algorithms

#if !defined(PSH_TEST_NOMAIN)
int main() {
    psh::test::algorithms::run_all();
    return 0;
}
#endif
