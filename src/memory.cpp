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
#include <psh/core.hpp>
#include <psh/debug.hpp>
#include <psh/math.hpp>

#if PSH_OS_WINDOWS
#    include <Windows.h>
#elif PSH_OS_UNIX
#    include <sys/mman.h>
#endif

#if PSH_ENABLE_ASSERT_NO_MEMORY_ERROR
#    define psh_impl_return_from_memory_error()                                                 \
        do {                                                                                    \
            psh_log_fatal("PSH_ENABLE_ASSERT_NO_MEMORY_ERROR active, aborting the program..."); \
            psh::abort_program();                                                               \
        } while (0)
#else
#    define psh_impl_return_from_memory_error() \
        return {}
#endif

namespace psh {
    // -------------------------------------------------------------------------------------------------
    // Virtual memory.
    // -------------------------------------------------------------------------------------------------

    // @TODO: We should round this up to a multiple of a page-size.
    psh_proc u8* memory_virtual_alloc(usize size_bytes) psh_no_except {
        u8* buf;
#if PSH_OS_WINDOWS
        buf = reinterpret_cast<u8*>(VirtualAlloc(nullptr, size_bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

#    if PSH_ENABLE_ASSERT_NO_MEMORY_ERROR
        psh_assert_fmt(buf != nullptr, "OS failed to allocate memory with error code: %lu", GetLastError());
#    endif
#elif PSH_OS_UNIX
        buf = reinterpret_cast<u8*>(mmap(nullptr, size_bytes, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));

#    if PSH_ENABLE_ASSERT_NO_MEMORY_ERROR
        psh_assert_fmt(reinterpret_cast<void*>(buf) != MAP_FAILED, "OS failed to allocate memory due to: %s", strerror(errno));
#    endif
#endif
        return buf;
    }

    psh_proc void memory_virtual_free(u8* memory, usize size_bytes) psh_no_except {
#if PSH_OS_WINDOWS
        psh_paranoid_validate_usage(psh_assert_not_null(memory));
        psh_discard_value(size_bytes);

        BOOL result = VirtualFree(memory, 0, MEM_RELEASE);
        if (psh_unlikely(result == FALSE)) {
            psh_log_error_fmt("Failed free memory with error code: %lu", GetLastError());
        }
#elif PSH_OS_UNIX
        psh_paranoid_validate_usage({
            psh_assert_not_null(memory);
            psh_assert(size_bytes > 0);
        });

        i32 result = munmap(memory, size_bytes);
        if (psh_unlikely(result == -1)) {
            psh_log_error_fmt("Failed to free memory due to: %s", strerror(errno));
        }
#endif
    }

    // -------------------------------------------------------------------------------------------------
    // Memory moves.
    // -------------------------------------------------------------------------------------------------

    psh_proc void memory_set(u8* memory, usize size_bytes, i32 fill) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(memory));

        if (psh_unlikely(size_bytes == 0)) {
            return;
        }

        psh_discard_value(memset(memory, fill, size_bytes));
    }

    psh_proc void memory_copy(u8* psh_no_alias dst, u8 const* psh_no_alias src, usize size_bytes) psh_no_except {
        psh_paranoid_validate_usage({
            psh_assert_not_null(dst);
            psh_assert_not_null(src);
            psh_assert_no_alias(src, dst);
        });

        if (psh_unlikely(size_bytes == 0)) {
            return;
        }

#if PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP
        psh_assert_msg(
            (dst + size_bytes > src) || (dst < src + size_bytes),
            "Source and destination overlap in copy region, which is undefined behaviour when using libc memcpy.");
#endif

        psh_discard_value(memcpy(dst, src, size_bytes));
    }

    psh_proc void memory_move(u8* psh_no_alias dst, u8 const* psh_no_alias src, usize size_bytes) psh_no_except {
        psh_paranoid_validate_usage({
            psh_assert_not_null(dst);
            psh_assert_not_null(src);
            psh_assert_no_alias(src, dst);
        });

        if (psh_unlikely(size_bytes == 0)) {
            return;
        }

        psh_discard_value(memmove(dst, src, size_bytes));
    }

    // -------------------------------------------------------------------------------------------------
    // Memory alignment.
    // -------------------------------------------------------------------------------------------------

    psh_proc usize padding_with_header(
        uptr  ptr_addr,
        usize alignment,
        usize header_size,
        usize header_alignment) psh_no_except {
        psh_validate_usage({
            psh_assert_fmt(
                psh_is_pow_of_two(alignment),
                "Expected the element alignment to be a power of two (got %zu).",
                alignment);
            psh_assert_fmt(
                psh_is_pow_of_two(header_alignment),
                "Expected the header alignment to be a power of two (got %zu).",
                header_alignment);
        });

        // Calculate the padding necessary for the alignment of the new block of memory.
        usize padding   = 0;
        usize mod_align = ptr_addr & (alignment - 1u);  // Same as ptr_addr % alignment.
        if (mod_align != 0) {
            padding += alignment - mod_align;
        }
        ptr_addr += padding;

        // Padding necessary for the header alignment.
        usize mod_header = ptr_addr & (header_alignment - 1u);  // Same as ptr_addr % header_alignment.
        if (mod_header != 0) {
            padding += header_alignment - mod_header;
        }

        // The padding should at least contain the header.
        padding += header_size;

        return padding;
    }

    psh_proc usize align_forward(uptr ptr_addr, usize alignment) psh_no_except {
        psh_validate_usage(psh_assert_fmt(psh_is_pow_of_two(alignment), "Expected alignment (%zu) to be a power of two.", alignment));

        usize mod_align = ptr_addr & (alignment - 1u);
        if (mod_align != 0) {
            ptr_addr += alignment - mod_align;
        }

        return ptr_addr;
    }

    // -------------------------------------------------------------------------------------------------
    // Stack memory allocator implementation.
    // -------------------------------------------------------------------------------------------------

    u8* Stack::top() psh_no_except {
        psh_paranoid_validate_usage(psh_assert_msg(this->buf != nullptr, "Stack uninitialised."));

        return this->buf + this->previous_offset;
    }

    StackHeader const* Stack::top_header() const psh_no_except {
        psh_paranoid_validate_usage(psh_assert_msg(this->buf != nullptr, "Stack uninitialised."));

        return reinterpret_cast<StackHeader const*>(pointer_add_bytes(this->buf, this->previous_offset - psh_usize_of(StackHeader)));
    }

    usize Stack::top_size() const psh_no_except {
        StackHeader const* header = this->top_header();
        return (header == nullptr) ? 0 : header->capacity;
    }

    usize Stack::top_previous_offset() const psh_no_except {
        StackHeader const* header = this->top_header();
        return (header == nullptr) ? 0 : header->previous_offset;
    }

    StackHeader const* Stack::header_of(u8 const* block) const psh_no_except {
        psh_validate_usage(psh_assert_not_null(block));
        psh_paranoid_validate_usage(psh_assert_msg(this->buf != nullptr, "Stack uninitialised."));

        u8 const* memory_start = this->buf;
        u8 const* block_header = block + sizeof(StackHeader);

        bool valid = (block >= memory_start)
                     && (block <= memory_start + this->capacity)
                     && (block <= memory_start + this->previous_offset)
                     && (block_header >= memory_start);

        return valid ? reinterpret_cast<StackHeader const*>(block_header) : nullptr;
    }

    usize Stack::size_of(u8 const* block) const psh_no_except {
        StackHeader const* header = this->header_of(block);
        return (header == nullptr) ? 0 : header->capacity;
    }

    usize Stack::previous_offset_of(u8 const* block) const psh_no_except {
        StackHeader const* header = this->header_of(block);
        return (header == nullptr) ? 0 : header->previous_offset;
    }

    Status Stack::pop() psh_no_except {
        if (psh_unlikely(this->previous_offset == 0)) {
            return STATUS_FAILED;
        }

        u8 const*          top        = pointer_add_bytes(this->buf, this->previous_offset);
        StackHeader const* top_header = reinterpret_cast<StackHeader const*>(pointer_const_subtract_bytes(top, psh_isize_of(StackHeader)));

        this->offset          = this->previous_offset - top_header->padding;
        this->previous_offset = top_header->previous_offset;
        return STATUS_OK;
    }

    Status Stack::clear_at(u8 const* block) psh_no_except {
        u8* this_buf = this->buf;

        // Check if the block is within the allocator's memory.
        bool is_within_region = (block >= this_buf) || (block < pointer_add_bytes(this_buf, this->previous_offset));
        if (psh_unlikely((block == nullptr) || !is_within_region)) {
            return STATUS_FAILED;
        }

        StackHeader const* header = reinterpret_cast<StackHeader const*>(block - psh_usize_of(StackHeader));

        this->previous_offset = header->previous_offset;
        this->offset          = no_wrap_sub(
            no_wrap_sub(reinterpret_cast<uptr>(block), header->padding),
            reinterpret_cast<uptr>(this_buf));

        return STATUS_OK;
    }

    // -------------------------------------------------------------------------------------------------
    // Memory manager implementation.
    // -------------------------------------------------------------------------------------------------

    void MemoryManager::init(usize capacity_bytes) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_msg(this->allocation_count == 0, "MemoryManager already initialised."));

        this->allocator.init(memory_virtual_alloc(capacity_bytes), capacity_bytes);
    }

    void MemoryManager::destroy() psh_no_except {
        memory_virtual_free(this->allocator.buf, this->allocator.capacity);
    }

    Status MemoryManager::pop() psh_no_except {
        Status st = this->allocator.pop();
        if (psh_likely(st == STATUS_OK)) {
            --this->allocation_count;
        }
        return st;
    }

    Status MemoryManager::clear_until(u8 const* block) psh_no_except {
        psh_validate_usage(psh_assert_not_null(block));

        u8 const* memory_start = this->allocator.buf;

        // Check if the block lies within the allocator's memory.
        if (psh_unlikely((block < memory_start) || (block > memory_start + this->allocator.previous_offset))) {
            cstring fail_reason = (block > memory_start + this->allocator.capacity)
                                      ? "MemoryManager::clear_until called with a pointer outside of the stack "
                                        "memory_start region."
                                      : "MemoryManager::clear_until called with a pointer to an already free region "
                                        "of the stack memory_start.";
            psh_log_error(fail_reason);
            return STATUS_FAILED;
        }

        // Pop the top memory block until popping block or reaching the end of the allocator.
        //
        // @NOTE: If we were given the incorrect address, we end up clearing the whole memory.
        for (;;) {
            u8 const* top_block = this->allocator.top();
            if (psh_unlikely(top_block == memory_start)) {
                break;
            }
            if (psh_likely(this->allocator.pop() == STATUS_OK)) {
                --this->allocation_count;
            }
            if (psh_unlikely(top_block == block)) {
                break;
            }
        }

        return STATUS_OK;
    }

    void MemoryManager::clear() psh_no_except {
        this->allocation_count = 0;
        this->allocator.clear();
    }
    // -------------------------------------------------------------------------------------------------
    // Memory manipulation procedures.
    // -------------------------------------------------------------------------------------------------

    psh_proc void raw_unordered_remove(FatPtr<u8> fptr, u8* element_ptr, usize element_size) psh_no_except {
        u8 const* buf_end = pointer_const_add_bytes(fptr.buf, fptr.count);
        psh_validate_usage(psh_assert_bounds_check(reinterpret_cast<uptr>(element_ptr), reinterpret_cast<uptr>(buf_end)));

        // Only effectively remove if the element isn't the last one.
        u8 const* last_element_ptr = pointer_const_subtract_bytes(buf_end, static_cast<isize>(element_size));
        if (element_ptr != last_element_ptr) {
            memory_move(element_ptr, last_element_ptr, element_size);
        }
    }

    psh_proc void raw_ordered_remove(FatPtr<u8> fptr, u8* element_ptr, usize element_size) psh_no_except {
        u8 const* buf_end = pointer_const_add_bytes(fptr.buf, fptr.count);
        psh_validate_usage(psh_assert_bounds_check(reinterpret_cast<uptr>(element_ptr), reinterpret_cast<uptr>(buf_end)));

        // Only effectively remove if the element isn't the last one.
        u8 const* last_element_ptr = pointer_const_subtract_bytes(buf_end, static_cast<isize>(element_size));
        if (element_ptr != last_element_ptr) {
            u8 const* next_element_ptr = pointer_const_add_bytes(element_ptr, element_size);
            memory_move(element_ptr, next_element_ptr, static_cast<usize>(pointer_offset(next_element_ptr, buf_end)));
        }
    }

    // @TODO: integrate this with the PSH_ENABLE_ASSERT_NO_MEMORY_ERROR
#define psh_impl_arena_report_out_of_memory(arena, requested_size, requested_alignment)  \
    do {                                                                                 \
        psh_log_error_fmt(                                                               \
            "Arena unable to allocate %zu bytes (with %u bytes of alignment) of memory." \
            " The allocator has only %zu bytes remaining.",                              \
            requested_size,                                                              \
            requested_alignment,                                                         \
            arena->capacity - arena->offset);                                            \
    } while (0)

    // @TODO: should we really do this? kinda cringe
#define psh_impl_arena_is_empty(arena) (((arena)->capacity == 0) || ((arena)->buf == nullptr))

    psh_proc u8* memory_alloc_align(Arena* arena, usize size_bytes, u32 alignment) psh_no_except {
        psh_validate_usage(psh_assert_not_null(arena));

        if (psh_unlikely(size_bytes == 0)) {
            return nullptr;
        }

        // Check if there is enough memory.
        uptr memory_addr    = reinterpret_cast<uptr>(arena->buf);
        uptr new_block_addr = align_forward(memory_addr + arena->offset, alignment);
        if (psh_unlikely(new_block_addr + size_bytes > arena->capacity + memory_addr)) {
            psh_impl_arena_report_out_of_memory(arena, size_bytes, alignment);
            psh_impl_return_from_memory_error();
        }

        // Commit the new block of memory.
        arena->offset = static_cast<usize>(size_bytes + new_block_addr - memory_addr);

        u8* new_block = reinterpret_cast<u8*>(new_block_addr);
        memory_set(new_block, size_bytes, 0);
        return new_block;
    }

    //
    // @TODO: When asan is available, poison the non-allocated memory regions!!
    //

    psh_proc u8* memory_alloc_align(Stack* stack, usize size_bytes, u32 alignment) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(stack));

        if (psh_unlikely(size_bytes == 0)) {
            return nullptr;
        }

        usize current_capacity = stack->capacity;
        usize current_offset   = stack->offset;
        u8*   free_memory      = pointer_add_bytes(stack->buf, current_offset);

        usize padding = padding_with_header(
            reinterpret_cast<uptr>(free_memory),
            alignment,
            psh_usize_of(StackHeader),
            alignof(StackHeader));
        usize required_bytes = padding + size_bytes;

        if (psh_unlikely(required_bytes > current_capacity - current_offset)) {
            psh_log_error_fmt(
                "Unable to allocate %zu bytes of memory (%zu bytes required due to alignment and padding)."
                " The stack allocator has only %zu bytes remaining.",
                size_bytes,
                required_bytes,
                current_capacity - current_offset);
            psh_impl_return_from_memory_error();
        }

        // Address to the start of the new block of memory.
        u8* new_block = pointer_add_bytes(free_memory, padding);

        // Write to the header associated with the new block of memory.
        StackHeader* new_header     = reinterpret_cast<StackHeader*>(pointer_subtract_bytes(new_block, psh_isize_of(StackHeader)));
        new_header->padding         = padding;
        new_header->capacity        = size_bytes;
        new_header->previous_offset = stack->previous_offset;

        // Update the stack offsets.
        stack->previous_offset = current_offset + padding;
        stack->offset          = current_offset + padding + size_bytes;

        memory_set(new_block, size_bytes, 0);
        return new_block;
    }

    psh_proc u8* memory_realloc_align(
        Arena* arena,
        u8*    block,
        usize  current_size_bytes,
        usize  new_size_bytes,
        u32    alignment) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(arena));
        psh_validate_usage({
            psh_assert_msg(block != nullptr, "Don't use realloc to allocate new memory.");
            psh_assert_msg(current_size_bytes != 0, "Don't use realloc to allocate new memory.");
            psh_assert_msg(new_size_bytes != 0, "Don't use realloc to free blocks of memory.");
        });

        uptr  memory_addr      = reinterpret_cast<uptr>(arena->buf);
        uptr  memory_end       = memory_addr + arena->capacity;
        usize memory_offset    = arena->offset;
        uptr  free_memory_addr = memory_addr + memory_offset;

        uptr block_addr = reinterpret_cast<uptr>(block);

        // Check if the block lies within the allocator's memory.
        if (psh_unlikely((block_addr < memory_addr) || (block_addr >= memory_end))) {
            psh_log_error("Pointer outside of the arena memory region.");
            psh_impl_return_from_memory_error();
        }

        // Check if the block is already free.
        if (psh_unlikely(block_addr >= free_memory_addr)) {
            psh_log_error("Pointer to a free address of the arena memory region.");
            psh_impl_return_from_memory_error();
        }

        if (psh_unlikely(current_size_bytes > memory_offset)) {
            psh_log_error_fmt(
                "current_block_size (%zu) surpasses the current offset (%zu) of the arena, which isn't allowed.",
                current_size_bytes,
                memory_offset);
            psh_impl_return_from_memory_error();
        }

        // If the block is the last allocated, just bump the offset.
        if (block_addr == free_memory_addr - current_size_bytes) {
            // Check if there is enough space.
            if (psh_unlikely(block_addr + new_size_bytes > memory_end)) {
                psh_log_error_fmt(
                    "Unable to reallocate block from %zu bytes to %zu bytes.",
                    current_size_bytes,
                    new_size_bytes);
                psh_impl_return_from_memory_error();
            }

            arena->offset = static_cast<usize>(
                static_cast<isize>(memory_offset) + static_cast<isize>(new_size_bytes - current_size_bytes));
            return block;
        }

        // Allocate a new block and copy old memory.
        u8* new_block = memory_alloc_align(arena, new_size_bytes, alignment);
        memory_move(new_block, block, psh_min_value(current_size_bytes, new_size_bytes));

        return new_block;
    }

    psh_proc u8* memory_realloc_align(Stack* stack, u8* block, usize new_size_bytes, u32 alignment) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(stack));
        psh_validate_usage({
            psh_assert_msg(stack->buf != nullptr, "Stack uninitialised.");
            psh_assert_msg(block != nullptr, "Don't use realloc to allocate new memory.");
            psh_assert_msg(new_size_bytes != 0, "Don't use realloc to free existing memory blocks.");
        });

        // If ptr is the last allocated block, just adjust the offsets.
        if (block == stack->top()) {
            stack->offset = stack->previous_offset + new_size_bytes;
            return block;
        }

        // Check if the address is within the allocator's memory.
        if (psh_unlikely((block < stack->buf) || (block >= stack->buf + stack->capacity))) {
            psh_log_error("Pointer outside of the memory region managed by the stack allocator.");
            psh_impl_return_from_memory_error();
        }

        // Check if the address is already free.
        if (psh_unlikely(block >= stack->buf + stack->offset)) {
            psh_log_error("Called with a free block of memory (use-after-free error).");
            psh_impl_return_from_memory_error();
        }

        StackHeader const* header = reinterpret_cast<StackHeader const*>(block + psh_usize_of(StackHeader));

        // Check memory availability.
        if (psh_unlikely(new_size_bytes > stack->capacity - stack->offset)) {
            psh_log_error_fmt(
                "Cannot reallocate memory from size %zu to %zu. Only %zu bytes of memory remaining.",
                header->capacity,
                new_size_bytes,
                stack->capacity - stack->offset);
            psh_impl_return_from_memory_error();
        }

        u8* new_block = memory_alloc_align(stack, new_size_bytes, alignment);

        usize const copy_size = psh_min_value(header->capacity, new_size_bytes);
        memory_copy(reinterpret_cast<u8*>(new_block), reinterpret_cast<u8 const*>(block), copy_size);

        return new_block;
    }
}  // namespace psh
