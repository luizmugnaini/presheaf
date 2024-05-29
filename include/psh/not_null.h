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
/// Description: Simple wrapper around pointers that ensures that the internal pointer is non-null.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/type_utils.h>

namespace psh {
    /// Type holding an immutable pointer that can never be null.
    template <typename T>
        requires NotPointer<T>
    struct NotNull {
        T* const ptr;

        NotNull(T* _ptr) noexcept : ptr{_ptr} {
            psh_assert_msg(ptr != nullptr, "NotNull created with a null pointer");
        }

        T& operator*() noexcept {
            return *ptr;
        }

        T* operator->() noexcept {
            return ptr;
        }

        NotNull() = delete;
    };
}  // namespace psh
