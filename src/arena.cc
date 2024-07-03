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
/// Description: Implementation of the arena memory allocator.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/arena.h>

namespace psh {
    // -----------------------------------------------------------------------------
    // - Arena implementation -
    // -----------------------------------------------------------------------------

    void Arena::clear() noexcept {
        offset = 0;
    }

    ScratchArena Arena::make_scratch() noexcept {
        return ScratchArena{this};
    }

    // -----------------------------------------------------------------------------
    // - Scratch arena implementation -
    // -----------------------------------------------------------------------------

    ScratchArena::ScratchArena(Arena* parent) noexcept
        : arena{parent}, saved_offset{parent->offset} {}

    ScratchArena::~ScratchArena() noexcept {
        arena->offset = saved_offset;
    }

    ScratchArena ScratchArena::decouple() const noexcept {
        return ScratchArena{arena};
    }
}  // namespace psh
