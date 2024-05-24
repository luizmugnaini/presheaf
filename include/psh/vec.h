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

#include <psh/types.h>

namespace psh {
    struct Vec2 {
        f32 x = 0.0f;
        f32 y = 0.0f;

        Vec2& operator+=(Vec2 const& other) noexcept;
    };

    struct Vec3 {
        f32 x = 0.0f;
        f32 y = 0.0f;
        f32 z = 0.0f;

        Vec3& operator+=(Vec3 const& other) noexcept;
        Vec3& operator-=(Vec3 const& other) noexcept;
        Vec3& operator*=(Vec3 const& other) noexcept;
        Vec3& operator*=(f32 scalar) noexcept;
        Vec3  operator+(Vec3 const& other) const noexcept;
        Vec3  operator-(Vec3 const& other) const noexcept;
        Vec3  operator-() const noexcept;
        Vec3  operator*(Vec3 const& other) const noexcept;
        Vec3  operator*(f32 scalar) const noexcept;

        Vec3 normalized() const noexcept;
        f32  dot(Vec3 const& other) const noexcept;
        Vec3 cross(Vec3 const& other) const noexcept;
    };

    template <typename T>
    struct Vec4 {
        f32 x = 0.0f;
        f32 y = 0.0f;
        f32 z = 0.0f;
        f32 w = 0.0f;
    };

    struct Mat4 {
        f32 data[16] = {0.0f};

        f32& at(u32 r, u32 c) noexcept;

        static Mat4 id() noexcept;
        static Mat4 translation(f32 dx, f32 dy, f32 dz) noexcept;
    };
}  // namespace psh
