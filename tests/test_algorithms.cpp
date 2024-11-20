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
            Buffer<i64, 10>   buf  = {-192381, -281937123, 123, 0, 3, 90, 23198237, 127391723, 123769128, 5};
            FatPtr<i64 const> fptr = make_const_fat_ptr(buf);

            // Check exact buffer elements.
            psh_assert(psh::contains(fptr, static_cast<i64>(-192381)));
            psh_assert(psh::contains(fptr, static_cast<i64>(-281937123)));
            psh_assert(psh::contains(fptr, static_cast<i64>(123)));
            psh_assert(psh::contains(fptr, static_cast<i64>(0)));
            psh_assert(psh::contains(fptr, static_cast<i64>(3)));
            psh_assert(psh::contains(fptr, static_cast<i64>(90)));
            psh_assert(psh::contains(fptr, static_cast<i64>(23198237)));
            psh_assert(psh::contains(fptr, static_cast<i64>(127391723)));
            psh_assert(psh::contains(fptr, static_cast<i64>(123769128)));
            psh_assert(psh::contains(fptr, static_cast<i64>(5)));

            // Non-matching elements.
            psh_assert(!psh::contains(fptr, static_cast<i64>(192381)));
            psh_assert(!psh::contains(fptr, static_cast<i64>(-28193)));
            psh_assert(!psh::contains(fptr, static_cast<i64>(13)));
            psh_assert(!psh::contains(fptr, static_cast<i64>(1)));
            psh_assert(!psh::contains(fptr, static_cast<i64>(50850)));
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
            Buffer<u64, 11>   buf  = {192381, 281937123, 123, 0, 0, 3, 90, 23198237, 127391723, 123769128, 5};
            FatPtr<u64 const> fptr = make_const_fat_ptr(buf);

            // Exact elements.
            psh_assert(psh::linear_search(fptr, static_cast<u64>(192381)).demand() == 0);
            psh_assert(psh::linear_search(fptr, static_cast<u64>(281937123)).demand() == 1);
            psh_assert(psh::linear_search(fptr, static_cast<u64>(123)).demand() == 2);
            psh_assert(psh::linear_search(fptr, static_cast<u64>(0)).demand() == 3);
            psh_assert(psh::linear_search(fptr, static_cast<u64>(3)).demand() == 5);
            psh_assert(psh::linear_search(fptr, static_cast<u64>(90)).demand() == 6);
            psh_assert(psh::linear_search(fptr, static_cast<u64>(23198237)).demand() == 7);
            psh_assert(psh::linear_search(fptr, static_cast<u64>(127391723)).demand() == 8);
            psh_assert(psh::linear_search(fptr, static_cast<u64>(123769128)).demand() == 9);
            psh_assert(psh::linear_search(fptr, static_cast<u64>(5)).demand() == 10);

            // Non-existent elements.
            psh_assert(!psh::linear_search(fptr, static_cast<u64>(92381)).has_value);
            psh_assert(!psh::linear_search(fptr, static_cast<u64>(28197123)).has_value);
            psh_assert(!psh::linear_search(fptr, static_cast<u64>(12)).has_value);
            psh_assert(!psh::linear_search(fptr, static_cast<u64>(1)).has_value);
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
            Buffer<i64, 10>   buf  = {-12837, -123, -72, -1, 0, 0, 19, 192381, 281937123, 55518237198};
            FatPtr<i64 const> fptr = make_const_fat_ptr(buf);

            // Exact elements.
            psh_assert(psh::binary_search(fptr, static_cast<i64>(-12837)).demand() == 0);
            psh_assert(psh::binary_search(fptr, static_cast<i64>(-123)).demand() == 1);
            psh_assert(psh::binary_search(fptr, static_cast<i64>(-72)).demand() == 2);
            psh_assert(psh::binary_search(fptr, static_cast<i64>(-1)).demand() == 3);
            psh_assert(psh::binary_search(fptr, static_cast<i64>(0)).demand() == 4);
            psh_assert(psh::binary_search(fptr, static_cast<i64>(19)).demand() == 6);
            psh_assert(psh::binary_search(fptr, static_cast<i64>(192381)).demand() == 7);
            psh_assert(psh::binary_search(fptr, static_cast<i64>(281937123)).demand() == 8);
            psh_assert(psh::binary_search(fptr, static_cast<i64>(55518237198)).demand() == 9);

            // Non-existent elements.
            psh_assert(!psh::binary_search(fptr, static_cast<i64>(92381)).has_value);
            psh_assert(!psh::binary_search(fptr, static_cast<i64>(28197123)).has_value);
            psh_assert(!psh::binary_search(fptr, static_cast<i64>(12)).has_value);
            psh_assert(!psh::binary_search(fptr, static_cast<i64>(1)).has_value);
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
