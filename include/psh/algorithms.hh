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

#pragma once

#include <psh/core.hh>
#include <psh/fat_ptr.hh>
#include <psh/option.hh>
#include <psh/type_utils.hh>

namespace psh {
    // -----------------------------------------------------------------------------
    // - Search algorithms -
    // -----------------------------------------------------------------------------

    template <typename T>
    using MatchFn = bool(T lhs, T rhs);

    /// Check if a range given by a fat pointer contains a given `match` element.
    template <typename T>
        requires IsObject<T> && TriviallyCopyable<T>
    bool contains(T match, FatPtr<T const> container, MatchFn<T>* match_fn) {
        psh_assert_msg(match_fn != nullptr, "contains expected a valid match function");
        bool found = false;
        for (auto const& m : container) {
            if (match_fn(match, m)) {
                found = true;
                break;
            }
        }
        return found;
    }

    /// Check if a range given by a fat pointer contains a given `match` element.
    template <typename T>
        requires IsObject<T> && TriviallyCopyable<T> && Reflexive<T>
    bool contains(T match, FatPtr<T const> container) {
        bool found = false;
        for (auto const& m : container) {
            if (match == m) {
                found = true;
                break;
            }
        }
        return found;
    }

    /// Try to find the index of the first match.
    template <typename T>
        requires Reflexive<T>
    Option<usize> linear_search(FatPtr<T const> fptr, T match) noexcept {
        Option<usize> match_idx   = {};
        usize         search_size = fptr.size;
        for (usize idx = 0; idx < search_size; ++idx) {
            if (fptr[idx] == match) {
                match_idx = idx;
                break;
            }
        }
        return match_idx;
    }

    /// Try to find the index of the first match.
    ///
    /// Note: We assume that the buffer of data is ordered.
    template <typename T>
        requires PartiallyOrdered<T> && TriviallyCopyable<T>
    Option<usize> binary_search(FatPtr<T const> fptr, T match) noexcept {
        if (psh_unlikely(fptr.is_empty())) {
            return {};
        }

        T* low      = fptr.begin();
        T* hi       = fptr.end();
        T* mid      = (low + hi) / 2;
        T  mid_elem = *mid;

        // Found.
        if (mid_elem == match) {
            return Option{static_cast<usize>(mid - low)};
        }

        // Search the left side.
        if (mid_elem < match) {
            T* new_low = mid + 1;
            return impl_binary_search(FatPtr{new_low, static_cast<usize>(hi - new_low)}, match);
        }

        // Search the right side.
        T* new_hi = mid - 1;
        return impl_binary_search(FatPtr{low, static_cast<usize>(new_hi - low)}, match);
    }

    // -----------------------------------------------------------------------------
    // - Write-based algorithms -
    // -----------------------------------------------------------------------------

    /// Override the contents of a fat pointer with a given element.
    ///
    /// This is the virtually same as `memory_set` but can copy elements of any type. However it
    /// will be slower.
    template <typename T>
        requires TriviallyCopyable<T>
    void fill(FatPtr<T> fat_ptr, T _fill) noexcept {
        for (T& elem : fat_ptr) {
            elem = _fill;
        }
    }

}  // namespace psh
