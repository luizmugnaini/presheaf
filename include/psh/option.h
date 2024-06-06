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
/// Description: Optional type.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/assert.h>
#include <psh/type_utils.h>

namespace psh {
    enum struct Status : bool {
        OK     = true,
        FAILED = false,
    };

    /// Option type.
    template <typename T>
        requires NotPointer<T>
    struct Option {
        T    val{};
        bool has_val = false;

        constexpr Option() noexcept = default;

        constexpr Option(T _val) noexcept : val{_val}, has_val{true} {}

        constexpr Option& operator=(T _val) noexcept {
            val     = _val;
            has_val = true;
            return *this;
        }

        T const& val_or(T const& default_val = {}) const noexcept {
            return has_val ? val : default_val;
        }

        T const& demand(strptr msg = "Option::demand failed") const noexcept {
            psh_assert_msg(has_val, msg);
            return val;
        }
    };
}  // namespace psh
