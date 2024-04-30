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
/// Description: Vector types. Just like in linear algebra, they have a fixed size.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>
#pragma once

namespace psh {
    template <typename T>
    struct Vec2 {
        T x = 0;
        T y = 0;

        constexpr Vec2& operator+=(Vec2 const& other) noexcept {
            x += other.x;
            y += other.y;
            return *this;
        }
    };

    template <typename T>
    struct Vec3 {
        T x = 0;
        T y = 0;
        T z = 0;

        constexpr Vec3& operator+=(Vec3 const& other) noexcept {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }
    };
}  // namespace psh
