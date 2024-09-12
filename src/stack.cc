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

#include <psh/stack.h>

namespace psh {
    void Stack::init(u8* _buf, usize _size) noexcept {
        this->buf  = _buf;
        this->size = _size;

        if (this->size > 0) {
            psh_assert_msg(
                this->buf != nullptr,
                "Stack initialized with non-zero capacity but an empty buffer");
        }
    }

    Stack::Stack(u8* _buf, usize _size) noexcept {
        this->init(_buf, _size);
    }

    u8* Stack::alloc_align(usize size_bytes, u32 alignment) noexcept {
        if (psh_unlikely(size_bytes == 0)) {
            return nullptr;
        }

        u8* const   free_mem = this->buf + this->offset;
        usize const padding  = padding_with_header(
            reinterpret_cast<uptr>(free_mem),
            alignment,
            sizeof(StackHeader),
            alignof(StackHeader));
        usize const required = padding + size_bytes;

        if (psh_unlikely(required > this->size - this->offset)) {
            psh_error_fmt(
                "StackAlloc::alloc unable to allocate %zu bytes of memory (%zu bytes required "
                "due to alignment and padding). The stack allocator has only %zu bytes "
                "remaining.",
                size_bytes,
                required,
                this->size - this->offset);
            psh_impl_return_from_memory_error();
        }

        // Address to the start of the new block of memory.
        u8* new_block = free_mem + padding;

        // Write to the header associated with the new block of memory.
        StackHeader* new_header     = reinterpret_cast<StackHeader*>(new_block - sizeof(StackHeader));
        new_header->padding         = padding;
        new_header->size            = size_bytes;
        new_header->previous_offset = this->previous_offset;

        // Update the stack offsets.
        this->previous_offset = this->offset + padding;
        this->offset += padding + size_bytes;

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
        if (psh_unlikely((block < this->buf) || (block >= this->buf + this->size))) {
            psh_error("StackAlloc::realloc called with a pointer outside of the memory region "
                      "managed by the stack allocator.");
            psh_impl_return_from_memory_error();
        }

        // Check if the address is already free.
        if (psh_unlikely(block >= this->buf + this->offset)) {
            psh_error("StackAlloc::realloc called with a free block of memory (use-after-free error).");
            psh_impl_return_from_memory_error();
        }

        StackHeader const* header = reinterpret_cast<StackHeader const*>(block - sizeof(StackHeader));

        // Check memory availability.
        if (psh_unlikely(new_size_bytes > this->size - this->offset)) {
            psh_error_fmt(
                "StackAlloc::realloc cannot reallocate memory from size %zu to %zu. Only %zu "
                "bytes of memory remaining.",
                header->size,
                new_size_bytes,
                this->size - this->offset);
            psh_impl_return_from_memory_error();
        }

        u8* new_mem = this->alloc_align(new_size_bytes, alignment);

        usize const copy_size = psh_min_val(header->size, new_size_bytes);
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
        return (header == nullptr) ? 0 : header->size;
    }

    usize Stack::top_previous_offset() const noexcept {
        StackHeader const* header = this->top_header();
        return (header == nullptr) ? 0 : header->previous_offset;
    }

    StackHeader const* Stack::header_of(u8 const* block) const noexcept {
        u8 const* block_header = block + sizeof(StackHeader);
        bool      valid        = true;

        if (psh_unlikely(block == nullptr)) {
            valid = false;
        }
        if (psh_unlikely((block < this->buf) || (block >= this->buf + this->size))) {
            psh_error("StackAlloc::header_of called with a pointer to a block of memory "
                      "outside of the stack allocator scope.");
            valid = false;
        }
        if (psh_unlikely(block > this->buf + this->previous_offset)) {
            psh_error("StackAlloc::header_of called with a pointer to a freed block of memory.");
            valid = false;
        }
        if (psh_unlikely(block_header < this->buf)) {
            psh_error("StackAlloc::header_of expected the memory block header to be contained "
                      "in the stack allocator scope.");
            valid = false;
        }

        return (!valid) ? nullptr : reinterpret_cast<StackHeader const*>(block_header);
    }

    usize Stack::size_of(u8 const* mem) const noexcept {
        StackHeader const* header = this->header_of(mem);
        return (header == nullptr) ? 0 : header->size;
    }

    usize Stack::previous_offset_of(u8 const* mem) const noexcept {
        auto* const header = this->header_of(mem);
        return (header == nullptr) ? 0 : header->previous_offset;
    }

    Status Stack::pop() noexcept {
        if (psh_unlikely(this->previous_offset == 0)) {
            return Status::FAILED;
        }

        u8 const*          top        = this->buf + this->previous_offset;
        StackHeader const* top_header = reinterpret_cast<StackHeader const*>(top - sizeof(StackHeader));

        this->offset          = this->previous_offset - top_header->padding;
        this->previous_offset = top_header->previous_offset;
        return Status::OK;
    }

    Status Stack::clear_at(u8 const* block) noexcept {
        if (psh_unlikely(block == nullptr)) {
            return Status::FAILED;
        }

        // Check if the block is within the allocator's memory.
        if (psh_unlikely((block < this->buf) || (block > this->buf + this->previous_offset))) {
            strptr fail_reason =
                (block > this->buf + this->size)
                    ? "StackAlloc::free_at called with a pointer outside of the stack allocator memory region."
                    : "StackAlloc::free_at called with a pointer to an already free region of the stack allocator memory.";
            psh_error(fail_reason);

            psh_impl_return_from_memory_error();
        }

        StackHeader const* header = reinterpret_cast<StackHeader const*>(block - sizeof(StackHeader));

        this->offset = wrap_sub(
            wrap_sub(reinterpret_cast<uptr>(block), header->padding),
            reinterpret_cast<uptr>(this->buf));
        this->previous_offset = header->previous_offset;

        return Status::OK;
    }

    /// Reset the allocator's offset.
    void Stack::clear() noexcept {
        this->offset          = 0;
        this->previous_offset = 0;
    }
}  // namespace psh
