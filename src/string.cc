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
/// Description: Implementation of the string related types and utility functions.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#include <psh/string.h>

#include <cstring>

namespace psh {
    StrCmpResult str_cmp(strptr lhs, strptr rhs) {
        i32 const cmp = std::strcmp(lhs, rhs);

        StrCmpResult res;
        if (cmp == 0) {
            res = StrCmpResult::Equal;
        } else if (cmp < 0) {
            res = StrCmpResult::LessThan;
        } else {
            res = StrCmpResult::GreaterThan;
        }

        return res;
    }

    bool str_equal(strptr lhs, strptr rhs) {
        return (std::strcmp(lhs, rhs) == 0);
    }
}  // namespace psh
