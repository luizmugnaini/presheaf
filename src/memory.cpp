///                             Presheaf library
/// Copyright (C) 2024 Luiz Gustavo Mugnaini Anselmo
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of
/// this software and associated documentation files (the “Software”), to deal in
/// the Software without restriction, including without limitation the rights to
/// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
/// of the Software, and to permit persons to whom the Software is furnished to do
/// so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///
/// Description: Implementation of the utilities for memory-related operations.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/memory.hpp>

#include <string.h>
#include <psh/assert.hpp>
#include <psh/core.hpp>
#include <psh/math.hpp>

#if defined(PSH_OS_WINDOWS)
#    include <Windows.h>
#elif defined(PSH_OS_UNIX)
#    include <sys/mman.h>
#endif

namespace psh {
    bool arch_is_little_endian() psh_noexcept {
        i32 integer = 1;
        return static_cast<bool>(*(reinterpret_cast<u8*>(&integer)));
    }

    bool arch_is_big_endian() psh_noexcept {
        i32 integer = 1;
        return static_cast<bool>(!*(reinterpret_cast<u8*>(&integer)));
    }

    FatPtr<u8> memory_virtual_alloc(usize size_bytes) psh_noexcept {
        FatPtr<u8> memory = {};

#if defined(PSH_OS_WINDOWS)
        memory.buf = reinterpret_cast<u8*>(VirtualAlloc(nullptr, size_bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
        if (psh_likely(memory.buf != nullptr)) {
            memory.count = size_bytes;
        } else {
#    if defined(PSH_ABORT_AT_MEMORY_ERROR)
            psh_log_fatal_fmt("OS failed to allocate memory with error code: %lu", GetLastError());
            psh_abort_program();
#    endif
        }
#elif defined(PSH_OS_UNIX)
        memory.buf = reinterpret_cast<u8*>(mmap(nullptr, size_bytes, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
        if (psh_likely(reinterpret_cast<void*>(memory.buf) != MAP_FAILED)) {
            memory.count = size_bytes;
        } else {
#    if defined(PSH_ABORT_AT_MEMORY_ERROR)
            psh_log_error_fmt("OS failed to allocate memory due to: %s", strerror(errno));
            psh_abort_program();
#    endif
        }
#endif

        return memory;
    }

    void memory_virtual_free(FatPtr<u8> memory) psh_noexcept {
#if defined(PSH_OS_WINDOWS)
        BOOL result = VirtualFree(memory.buf, 0, MEM_RELEASE);
        if (psh_unlikely(result == FALSE)) {
            psh_log_error_fmt("Failed free memory with error code: %lu", GetLastError());
        }
#elif defined(PSH_OS_UNIX)
        i32 result = munmap(memory.buf, memory.count);
        if (psh_unlikely(result == -1)) {
            psh_log_error_fmt("Failed to free memory due to: %s", strerror(errno));
        }
#endif
    }

    void memory_set(u8* memory, usize size_bytes, i32 fill) psh_noexcept {
        if (psh_unlikely(size_bytes == 0)) {
            return;
        }
        psh_assert(memory != nullptr);

        psh_discard_value(memset(memory, fill, size_bytes));
    }

    void memory_copy(u8* psh_no_alias dst, u8 const* psh_no_alias src, usize size_bytes) psh_noexcept {
        if (psh_unlikely(size_bytes == 0)) {
            return;
        }
        psh_assert(dst != nullptr);
        psh_assert(src != nullptr);

#if defined(PSH_CHECK_MEMCPY_OVERLAP)
        psh_assert_msg(
            (dst + size_bytes > src) || (dst < src + size_bytes),
            "Source and destination overlap in copy region (UB).");
#endif

        psh_discard_value(memcpy(dst, src, size_bytes));
    }

    void memory_move(u8* psh_no_alias dst, u8 const* psh_no_alias src, usize size_bytes) psh_noexcept {
        if (psh_unlikely(size_bytes == 0)) {
            return;
        }
        psh_assert(dst != nullptr);
        psh_assert(src != nullptr);

        psh_discard_value(memmove(dst, src, size_bytes));
    }

    usize padding_with_header(
        uptr  ptr_addr,
        usize alignment,
        usize header_size,
        usize header_alignment) psh_noexcept {
        psh_assert_fmt(
            psh_is_pow_of_two(alignment) && psh_is_pow_of_two(header_alignment),
            "Expected the alignments to be powers of two (alignment: %zu; header_alignment: %zu).",
            alignment,
            header_alignment);

        // Calculate the padding necessary for the alignment of the new block of memory.
        usize padding   = 0;
        usize mod_align = ptr_addr & (alignment - 1u);  // Same as `ptr_addr % alignment`.
        if (mod_align != 0) {
            padding += alignment - mod_align;
        }
        ptr_addr += padding;

        // Padding necessary for the header alignment.
        usize mod_header = ptr_addr & (header_alignment - 1u);  // Same as `ptr_addr % header_alignment`.
        if (mod_header != 0) {
            padding += header_alignment - mod_header;
        }

        // The padding should at least contain the header.
        padding += header_size;

        return padding;
    }

    usize align_forward(uptr ptr_addr, usize alignment) psh_noexcept {
        psh_assert_fmt(psh_is_pow_of_two(alignment), "Expected alignment (%zu) to be a power of two.", alignment);

        usize mod_align = ptr_addr & (alignment - 1u);
        if (mod_align != 0) {
            ptr_addr += alignment - mod_align;
        }

        return ptr_addr;
    }
}  // namespace psh
