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
/// Description: Set type.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/algorithms.h>
#include <psh/arena.h>
#include <psh/dyn_array.h>
#include <initializer_list>

namespace psh {
    /// Collection of non-repeating elements.
    template <typename T>
    struct Set {
        DynArray<T> data;

        Set(FatPtr<T const> fptr, Arena* arena) noexcept : data{arena, fptr.size} {
            for (auto elem : fptr) {
                if (linear_search(fptr, elem).has_val) {
                    data.push(elem);
                }
            }
        }

        Set(std::initializer_list<T> elems, Arena* arena) noexcept : data{arena, elems.size} {
            FatPtr<T> fptr{elems.begin(), elems.size};
            for (auto elem : fptr) {
                if (linear_search(fptr, elem).has_val) {
                    data.push(elem);
                }
            }
        }
    };
}  // namespace psh
