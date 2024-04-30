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
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/concepts.h>
#include <psh/fat_ptr.h>
#include <psh/option.h>

namespace psh {
    /// Try to find the index of the first match.
    template <typename T>
        requires Reflexive<T>
    Option<usize> linear_search(FatPtr<T const> fptr, T match) noexcept {
        Option<usize> match_idx{};
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
        if (fptr.is_empty()) return {};

        T* low = fptr.begin();
        T* hi  = fptr.end();
        T* mid = (low + hi) / 2;

        T mid_elem = *mid;
        if (mid_elem == match) {
            return Option{static_cast<usize>(mid - low)};
        } else if (mid_elem < match) {
            // Search the left side.
            T* new_low = mid + 1;
            return impl_binary_search(FatPtr{new_low, static_cast<usize>(hi - new_low)}, match);
        }
        // Search the right side.
        T* new_hi = mid - 1;
        return impl_binary_search(FatPtr{low, static_cast<usize>(new_hi - low)}, match);
    }
}  // namespace psh
