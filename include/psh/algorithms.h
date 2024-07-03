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
/// Description: Fat pointer type.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/fat_ptr.h>
#include <psh/intrinsics.h>
#include <psh/option.h>
#include <psh/type_utils.h>

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
