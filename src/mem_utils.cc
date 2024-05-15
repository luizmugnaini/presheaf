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
/// Description: Implementation of the utilities for memory-related operations.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#include <psh/mem_utils.h>

#include <psh/assert.h>
#include <psh/intrinsics.h>
#include <psh/math.h>
#include <psh/types.h>

#include <cstring>

namespace psh {
    bool arch_is_little_endian() noexcept {
        i32 integer = 1;
        return static_cast<bool>(*(reinterpret_cast<u8*>(&integer)));
    }

    bool arch_is_bit_endian() noexcept {
        i32 integer = 1;
        return static_cast<bool>(!*(reinterpret_cast<u8*>(&integer)));
    }

    void memory_set(FatPtr<u8> fptr, i32 fill) noexcept {
        if (psh_unlikely(fptr.buf == nullptr)) return;
        psh_discard(std::memset(fptr.buf, fill, fptr.size));
    }

    void memory_copy(u8* dest, u8 const* src, usize size) noexcept {
        if (psh_unlikely(dest == nullptr || src == nullptr)) {
            return;
        }

#if defined(PSH_DEBUG) || defined(PSH_CHECK_MEMCPY_OVERLAP)
        auto const dest_addr = reinterpret_cast<uptr>(dest);
        auto const src_addr  = reinterpret_cast<uptr>(src);
        psh_assert_msg(
            (dest_addr + size > src_addr) || (dest_addr < src_addr + size),
            "memcpy called but source and destination overlap, which produces UB");
#endif

        psh_discard(std::memcpy(dest, src, size));
    }

    void memory_move(u8* dest, u8 const* src, usize size) noexcept {
        if (psh_unlikely(dest == nullptr || src == nullptr)) return;
        psh_discard(std::memmove(dest, src, size));
    }

    usize padding_with_header(
        uptr  ptr,
        usize alignment,
        usize header_size,
        usize header_alignment) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_ALIGNMENT)
        psh_assert_msg(
            psh_is_pow_of_two(alignment) && psh_is_pow_of_two(header_alignment),
            "padding_with_header expected the alignments to be powers of two");
#endif

        uptr padding = 0;

        // Padding necessary for the alignment of the new block of memory.
        uptr const align = static_cast<uptr>(alignment);
        uptr const mod_align =
            ptr & (align - 1);  // Same as `ptr % align` since `align` is a power of two.
        if (mod_align != 0) {
            padding += align - mod_align;
        }
        ptr += padding;

        // Padding necessary for the header alignment.
        uptr const align_header = static_cast<uptr>(header_alignment);
        uptr const mod_header   = ptr & (align_header - 1);  // Same as `ptr % align_header`.
        if (mod_header != 0) {
            padding += align_header - mod_header;
        }

        // The padding should at least contain the header.
        padding += header_size;

        return static_cast<usize>(padding);
    }

    usize align_forward(uptr ptr, usize alignment) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_ALIGNMENT)
        psh_assert_msg(
            psh_is_pow_of_two(alignment),
            "align_forward expected the alignment to be a power of two");
#endif

        uptr const align     = static_cast<uptr>(alignment);
        uptr const mod_align = ptr & (align - 1);
        if (mod_align != 0) {
            ptr += align - mod_align;
        }

        return ptr;
    }
}  // namespace psh
