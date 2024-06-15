#include <psh/stack.h>

namespace psh {
    void Stack::init(u8* _buf, usize _capacity) noexcept {
        buf      = _buf;
        capacity = _capacity;
        if (psh_likely(capacity != 0)) {
            psh_assert_msg(
                buf != nullptr,
                "Stack::init called with non-zero capacity but null memory");
        }
    }
    Stack::Stack(u8* _buf, usize _capacity) noexcept {
        this->init(_buf, _capacity);
    }

    usize Stack::used() const noexcept {
        return offset;
    }

    u8* Stack::top() const noexcept {
        return psh_ptr_add(buf, previous_offset);
    }

    StackHeader const* Stack::top_header() const noexcept {
        return reinterpret_cast<StackHeader const*>(
            psh_ptr_add(buf, previous_offset - sizeof(StackHeader)));
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
        if (psh_unlikely((block < buf) || (block >= buf + capacity))) {
            psh_error("StackAlloc::header_of called with a pointer to a block of memory "
                      "outside of the stack allocator scope.");
            valid = false;
        }
        if (psh_unlikely(block > buf + previous_offset)) {
            psh_error("StackAlloc::header_of called with a pointer to a freed block of memory.");
            valid = false;
        }
        if (psh_unlikely(block_header < buf)) {
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
        if (psh_unlikely(previous_offset == 0)) {
            return Status::FAILED;
        }

        u8 const*          top = buf + previous_offset;
        StackHeader const* top_header =
            reinterpret_cast<StackHeader const*>(top - sizeof(StackHeader));

        offset          = previous_offset - top_header->padding;
        previous_offset = top_header->previous_offset;
        return Status::OK;
    }

    Status Stack::clear_at(u8 const* block) noexcept {
        if (psh_unlikely(block == nullptr)) {
            return Status::FAILED;
        }

        // Check if the block is within the allocator's memory.
        if (psh_unlikely((block < buf) || (block > buf + previous_offset))) {
            strptr fail_reason =
                (block > buf + capacity)
                    ? "StackAlloc::free_at called with a pointer outside of the stack "
                      "allocator memory region."
                    : "StackAlloc::free_at called with a pointer to an already free region of "
                      "the stack allocator memory.";
            psh_error(fail_reason);
            return Status::FAILED;
        }

        StackHeader const* header =
            reinterpret_cast<StackHeader const*>(block - sizeof(StackHeader));

        offset = wrap_sub(
            wrap_sub(reinterpret_cast<uptr>(block), header->padding),
            reinterpret_cast<uptr>(buf));
        previous_offset = header->previous_offset;

        return Status::OK;
    }

    /// Reset the allocator's offset.
    void Stack::clear() noexcept {
        offset          = 0;
        previous_offset = 0;
    }
}  // namespace psh
