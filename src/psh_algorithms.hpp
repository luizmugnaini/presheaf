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
/// Description: Commonly used algorithms.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

// TODO: Implement the cache-oblivious Funnelsort https://en.wikipedia.org/wiki/Funnelsort

#pragma once

#include "psh_core.hpp"
#include "psh_memory.hpp"

/// Range size threshold for the quick sort algorithm to fallback to the insertion sort algorithm.
#ifndef QUICK_SORT_CUTOFF_TO_INSERTION_SORT
#    define QUICK_SORT_CUTOFF_TO_INSERTION_SORT 10u
#endif

namespace psh {
    // -------------------------------------------------------------------------------------------------
    // Search algorithms.
    // -------------------------------------------------------------------------------------------------

    template <typename T>
    using MatchFn = bool(T lhs, T rhs);

    /// Check if a range given by a fat pointer contains a given match element.
    template <typename T>
    psh_proc bool contains(FatPtr<T const> fptr, T match) psh_no_except {
        bool found = false;
        for (usize idx = 0; idx < fptr.count; ++idx) {
            if (match == fptr[idx]) {
                found = true;
                break;
            }
        }
        return found;
    }

    /// Check if a range given by a fat pointer contains a given match element.
    template <typename T>
    psh_proc bool contains(FatPtr<T const> fptr, T match, MatchFn<T>* match_fn) psh_no_except {
        psh_assert_not_null(match_fn);

        bool found = false;
        for (usize idx = 0; idx < fptr.count; ++idx) {
            if (match_fn(fptr[idx], match)) {
                found = true;
                break;
            }
        }
        return found;
    }

    /// Try to find the index of the first match.
    template <typename T>
    psh_proc isize linear_search(FatPtr<T const> fptr, T match) psh_no_except {
        isize match_idx = -1;
        for (usize idx = 0; idx < fptr.count; ++idx) {
            if (fptr[idx] == match) {
                match_idx = static_cast<isize>(idx);
                break;
            }
        }
        return match_idx;
    }

    /// Try to find the index of the first match.
    template <typename T>
    psh_proc isize linear_search(FatPtr<T const> fptr, T match, MatchFn<T>* match_fn) psh_no_except {
        isize match_idx = -1;
        for (usize idx = 0; idx < fptr.count; ++idx) {
            if (match_fn(fptr[idx], match)) {
                match_idx = static_cast<isize>(idx);
                break;
            }
        }
        return match_idx;
    }

    /// Try to find the index of the first match.
    ///
    /// Note: We assume that the buffer of data is ordered.
    template <typename T>
    psh_proc isize binary_search(FatPtr<T const> fptr, T match) psh_no_except {
        return binary_search_range(fptr, match, 0, fptr.count - 1u);
    }

    template <typename T>
    psh_proc isize binary_search_range(FatPtr<T const> fptr, T match, usize low, usize high) psh_no_except {
        if (psh_unlikely(high < low)) {
            return -1;
        }

        usize mid      = (low + high) / 2;
        T     mid_elem = fptr[mid];

        // Found.
        if (mid_elem == match) {
            return static_cast<isize>(mid);
        }

        // Search the left side.
        if (mid_elem < match) {
            return binary_search_range(fptr, match, mid + 1u, high);
        }

        // Search the right side.
        return binary_search_range(fptr, match, low, mid - 1u);
    }

    // -------------------------------------------------------------------------------------------------
    // Sorting algorithms.
    // -------------------------------------------------------------------------------------------------

    template <typename T>
    psh_proc void swap_elements(T* data, usize lhs_idx, usize rhs_idx) psh_no_except {
        T tmp         = data[lhs_idx];
        data[lhs_idx] = data[rhs_idx];
        data[rhs_idx] = tmp;
    }

    template <typename T>
    psh_proc void swap_elements(FatPtr<T> data, usize lhs_idx, usize rhs_idx) psh_no_except {
        T tmp         = data[lhs_idx];
        data[lhs_idx] = data[rhs_idx];
        data[rhs_idx] = tmp;
    }

    template <typename T>
    psh_proc void insertion_sort(FatPtr<T> data) psh_no_except {
        for (usize end = 1; end < data.count; ++end) {
            for (usize idx = end; (idx > 0) && (data[idx - 1u] > data[idx]); --idx) {
                swap_elements(data.buf, idx, idx - 1u);
            }
        }
    }

    template <typename T>
    psh_proc void quick_sort(FatPtr<T> data) psh_no_except {
        quick_sort_range(data, 0, data.count - 1u);
    }

    template <typename T>
    psh_proc void quick_sort_range(FatPtr<T> data, usize low, usize high) psh_no_except {
        if (high <= low + QUICK_SORT_CUTOFF_TO_INSERTION_SORT) {
            insertion_sort(make_slice(&data, low, (high + 1u) - low));
            return;
        }

        usize left_scan  = low + 1u;
        usize right_scan = high;
        for (;;) {
            while (data[left_scan] <= data[low]) {
                if (left_scan == high) {
                    break;
                }
                ++left_scan;
            }

            while (data[low] <= data[right_scan]) {
                if (right_scan == low) {
                    break;
                }
                right_scan = psh_nowrap_unsigned_dec(right_scan);
            }

            if (right_scan <= left_scan) {
                break;
            }

            swap_elements(data.buf, left_scan, right_scan);
        }
        swap_elements(data.buf, low, right_scan);

        quick_sort_range(data, low, psh_nowrap_unsigned_dec(right_scan));
        quick_sort_range(data, right_scan + 1u, high);
    }

    // -------------------------------------------------------------------------------------------------
    // Write-based algorithms.
    // -------------------------------------------------------------------------------------------------

    /// Override the contents of a fat pointer with a given element.
    ///
    /// This is the virtually same as memory_set but can copy elements of any type. However it
    /// will be slower.
    template <typename T>
    psh_proc void fill(FatPtr<T> fptr, T value) psh_no_except {
        for (usize idx = 0; idx < fptr.count; ++idx) {
            fptr[idx] = value;
        }
    }
}  // namespace psh
