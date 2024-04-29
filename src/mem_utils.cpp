/// Implementation of the mem_utils.h header file.
///
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#include <psh/mem_utils.h>

#include <compile_options.h>
#include <psh/assert.h>
#include <psh/math.h>

#include <cstring>

namespace psh {
    void memory_set(FatPtr<u8> fat_ptr, i32 fill) noexcept {
        if (fat_ptr.buf() == nullptr) {
            return;
        }
        psh_discard(std::memset(fat_ptr.buf(), fill, fat_ptr.size()));
    }

    void memory_copy(u8* dest, u8 const* src, usize size) noexcept {
        if (dest == nullptr || src == nullptr) {
            return;
        }

        if constexpr (CHECK_MEMCPY_OVERLAP) {
            auto const dest_addr = reinterpret_cast<uptr>(dest);
            auto const src_addr  = reinterpret_cast<uptr>(src);
            psh_assert_msg(
                (dest_addr + size > src_addr) || (dest_addr < src_addr + size),
                "memcpy called but source and destination overlap, which produces UB");
        }

        psh_discard(std::memcpy(dest, src, size));
    }

    void memory_move(u8* dest, u8 const* src, usize size) noexcept {
        if (dest == nullptr || src == nullptr) {
            return;
        }
        psh_discard(std::memmove(dest, src, size));
    }

    usize padding_with_header(
        uptr  ptr,
        usize alignment,
        usize header_size,
        usize header_alignment) noexcept {
        if constexpr (CHECK_ALIGNMENT) {
            psh_assert_msg(
                is_power_of_two(alignment) && is_power_of_two(header_alignment),
                "padding_with_header expected the alignments to be powers of two");
        }

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
        if constexpr (CHECK_ALIGNMENT) {
            psh_assert_msg(
                is_power_of_two(alignment),
                "align_forward expected the alignment to be a power of two");
        }

        uptr const align     = static_cast<uptr>(alignment);
        uptr const mod_align = ptr & (align - 1);
        if (mod_align != 0) {
            ptr += align - mod_align;
        }

        return ptr;
    }
}  // namespace psh
