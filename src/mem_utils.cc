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
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/mem_utils.h>

#include <psh/assert.h>
#include <psh/core.h>
#include <psh/math.h>
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
        if (psh_unlikely((fptr.size == 0) || (fptr.buf == nullptr))) {
            return;
        }
        psh_discard(std::memset(fptr.buf, fill, fptr.size));
    }

    void memory_copy(void* dest, void const* src, usize size) noexcept {
        if (psh_unlikely(dest == nullptr || src == nullptr)) {
            return;
        }

#if defined(PSH_DEBUG) || defined(PSH_CHECK_MEMCPY_OVERLAP)
        uptr dest_addr = reinterpret_cast<uptr>(dest);
        uptr src_addr  = reinterpret_cast<uptr>(src);
        psh_assert_msg(
            (dest_addr + size > src_addr) || (dest_addr < src_addr + size),
            "memcpy called but source and destination overlap, which produces UB");
#endif

        psh_discard(std::memcpy(dest, src, size));
    }

    void memory_move(void* dest, void const* src, usize size) noexcept {
        if (psh_unlikely(dest == nullptr || src == nullptr)) {
            return;
        }
        psh_discard(std::memmove(dest, src, size));
    }

    usize padding_with_header(
        uptr  ptr,
        usize alignment,
        usize header_size,
        usize header_alignment) noexcept {
        psh_assert_msg(
            psh_is_pow_of_two(alignment) && psh_is_pow_of_two(header_alignment),
            "padding_with_header expected the alignments to be powers of two");

        // Calculate the padding necessary for the alignment of the new block of memory.
        uptr padding = 0;
        uptr align   = static_cast<uptr>(alignment);
        uptr mod_align =
            ptr & (align - 1);  // Same as `ptr % align` since `align` is a power of two.
        if (mod_align != 0) {
            padding += align - mod_align;
        }
        ptr += padding;

        // Padding necessary for the header alignment.
        uptr align_header = static_cast<uptr>(header_alignment);
        uptr mod_header   = ptr & (align_header - 1);  // Same as `ptr % align_header`.
        if (mod_header != 0) {
            padding += align_header - mod_header;
        }

        // The padding should at least contain the header.
        padding += header_size;

        return static_cast<usize>(padding);
    }

    usize align_forward(uptr ptr, usize alignment) noexcept {
        psh_assert_msg(
            psh_is_pow_of_two(alignment),
            "align_forward expected the alignment to be a power of two");

        uptr align     = static_cast<uptr>(alignment);
        uptr mod_align = ptr & (align - 1);
        if (mod_align != 0) {
            ptr += align - mod_align;
        }

        return ptr;
    }
}  // namespace psh
