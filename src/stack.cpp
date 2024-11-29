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
/// Description: Implementation of the stack memory allocator.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/stack.hpp>

namespace psh {
    u8* Stack::alloc_align(usize size_bytes, u32 alignment) noexcept {
        usize current_capacity = this->capacity;
        usize current_offset   = this->offset;

        if (psh_unlikely(current_capacity == 0 || size_bytes == 0)) {
            return nullptr;
        }

        u8*   free_memory = this->buf + current_offset;
        usize padding     = padding_with_header(
            reinterpret_cast<uptr>(free_memory),
            alignment,
            sizeof(StackHeader),
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
        u8* new_block = free_memory + padding;

        // Write to the header associated with the new block of memory.
        StackHeader* new_header     = reinterpret_cast<StackHeader*>(new_block - sizeof(StackHeader));
        new_header->padding         = padding;
        new_header->capacity        = size_bytes;
        new_header->previous_offset = this->previous_offset;

        // Update the stack offsets.
        this->previous_offset = current_offset + padding;
        this->offset          = current_offset + padding + size_bytes;

        memory_set(new_block, size_bytes, 0);
        return new_block;
    }

    u8* Stack::realloc_align(u8* block, usize new_size_bytes, u32 alignment) noexcept {
        if (psh_unlikely(new_size_bytes == 0)) {
            this->clear_at(block);
            return nullptr;
        }

        // If `ptr` is the last allocated block, just adjust the offsets.
        if (block == this->top()) {
            this->offset = this->previous_offset + new_size_bytes;
            return block;
        }

        // Check if the address is within the allocator's memory.
        if (psh_unlikely((block < this->buf) || (block >= this->buf + this->capacity))) {
            psh_log_error("Pointer outside of the memory region managed by the stack allocator.");
            psh_impl_return_from_memory_error();
        }

        // Check if the address is already free.
        if (psh_unlikely(block >= this->buf + this->offset)) {
            psh_log_error("Called with a free block of memory (use-after-free error).");
            psh_impl_return_from_memory_error();
        }

        StackHeader const* header = reinterpret_cast<StackHeader const*>(block - sizeof(StackHeader));

        // Check memory availability.
        if (psh_unlikely(new_size_bytes > this->capacity - this->offset)) {
            psh_log_error_fmt(
                "Cannot reallocate memory from size %zu to %zu. Only %zu bytes of memory remaining.",
                header->capacity,
                new_size_bytes,
                this->capacity - this->offset);
            psh_impl_return_from_memory_error();
        }

        u8* new_mem = this->alloc_align(new_size_bytes, alignment);

        usize const copy_size = psh_min_value(header->capacity, new_size_bytes);
        memory_copy(reinterpret_cast<u8*>(new_mem), reinterpret_cast<u8 const*>(block), copy_size);

        return new_mem;
    }

    usize Stack::used() const noexcept {
        return this->offset;
    }

    u8* Stack::top() const noexcept {
        return psh_ptr_add(this->buf, this->previous_offset);
    }

    StackHeader const* Stack::top_header() const noexcept {
        return reinterpret_cast<StackHeader const*>(
            psh_ptr_add(this->buf, this->previous_offset - sizeof(StackHeader)));
    }

    usize Stack::top_size() const noexcept {
        StackHeader const* header = this->top_header();
        return (header == nullptr) ? 0 : header->capacity;
    }

    usize Stack::top_previous_offset() const noexcept {
        StackHeader const* header = this->top_header();
        return (header == nullptr) ? 0 : header->previous_offset;
    }

    StackHeader const* Stack::header_of(u8 const* block) const noexcept {
        u8 const* memory_start = this->buf;

        bool valid = true;
        valid &= (block != nullptr);
        valid &= (block >= memory_start);
        valid &= (block <= memory_start + this->capacity);
        valid &= (block <= memory_start + this->previous_offset);

        u8 const* block_header = block + sizeof(StackHeader);
        valid &= (block_header >= memory_start);

        return valid ? reinterpret_cast<StackHeader const*>(block_header) : nullptr;
    }

    usize Stack::size_of(u8 const* block) const noexcept {
        StackHeader const* header = this->header_of(block);
        return (header == nullptr) ? 0 : header->capacity;
    }

    usize Stack::previous_offset_of(u8 const* block) const noexcept {
        StackHeader const* header = this->header_of(block);
        return (header == nullptr) ? 0 : header->previous_offset;
    }

    Status Stack::pop() noexcept {
        if (psh_unlikely(this->previous_offset == 0)) {
            return STATUS_FAILED;
        }

        u8 const*          top        = this->buf + this->previous_offset;
        StackHeader const* top_header = reinterpret_cast<StackHeader const*>(top - sizeof(StackHeader));

        this->offset          = this->previous_offset - top_header->padding;
        this->previous_offset = top_header->previous_offset;
        return STATUS_OK;
    }

    Status Stack::clear_at(u8 const* block) noexcept {
        if (psh_unlikely(block == nullptr)) {
            return STATUS_FAILED;
        }

        // Check if the block is within the allocator's memory.
        if (psh_unlikely((block < this->buf) || (block > this->buf + this->previous_offset))) {
            strptr fail_reason =
                (block > this->buf + this->capacity)
                    ? "Pointer outside of the stack allocator memory region."
                    : "Pointer to an already free region of the stack allocator memory.";
            psh_log_error(fail_reason);

            psh_impl_return_from_memory_error();
        }

        StackHeader const* header = reinterpret_cast<StackHeader const*>(block - sizeof(StackHeader));

        this->offset = no_wrap_sub(
            no_wrap_sub(reinterpret_cast<uptr>(block), header->padding),
            reinterpret_cast<uptr>(this->buf));
        this->previous_offset = header->previous_offset;

        return STATUS_OK;
    }
}  // namespace psh
