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
/// Description: Implementation of the stack memory allocator.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/stack.h>

namespace psh {
    void Stack::init(u8* _buf, usize _capacity) noexcept {
        this->buf      = _buf;
        this->capacity = _capacity;
        if (psh_likely(this->capacity != 0)) {
            psh_assert_msg(
                this->buf != nullptr,
                "Stack::init called with non-zero capacity but null memory");
        }
    }
    Stack::Stack(u8* _buf, usize _capacity) noexcept {
        this->init(_buf, _capacity);
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

    usize Stack::top_capacity() const noexcept {
        StackHeader const* header = this->top_header();
        return (header == nullptr) ? 0 : header->capacity;
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
        if (psh_unlikely((block < this->buf) || (block >= this->buf + this->capacity))) {
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

    usize Stack::capacity_of(u8 const* mem) const noexcept {
        StackHeader const* header = this->header_of(mem);
        return (header == nullptr) ? 0 : header->capacity;
    }

    usize Stack::previous_offset_of(u8 const* mem) const noexcept {
        auto* const header = this->header_of(mem);
        return (header == nullptr) ? 0 : header->previous_offset;
    }

    Status Stack::pop() noexcept {
        if (psh_unlikely(this->previous_offset == 0)) {
            return Status::FAILED;
        }

        u8 const*          top = this->buf + this->previous_offset;
        StackHeader const* top_header =
            reinterpret_cast<StackHeader const*>(top - sizeof(StackHeader));

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
                (block > this->buf + this->capacity)
                    ? "StackAlloc::free_at called with a pointer outside of the stack "
                      "allocator memory region."
                    : "StackAlloc::free_at called with a pointer to an already free region of "
                      "the stack allocator memory.";
            psh_error(fail_reason);
            return Status::FAILED;
        }

        StackHeader const* header =
            reinterpret_cast<StackHeader const*>(block - sizeof(StackHeader));

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
