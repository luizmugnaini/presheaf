/// Stack memory allocator.
///
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#pragma once

#include <psh/io.h>
#include <psh/math.h>
#include <psh/mem_utils.h>
#include <psh/types.h>

namespace psh {
    /// Header associated with each memory block in the stack allocator.
    ///
    /// Memory layout:
    ///
    /// ```md
    ///           `previous_offset`                     |-`capacity`-|
    ///                  ^                              ^            ^
    ///                  |                              |            |
    ///  |previous header|previous memory|+++++++|header|  memory    |
    ///                                  ^              ^
    ///                                  |--`padding`---|
    /// ```
    ///
    /// where "header" represents this current header, and "memory" represents the memory block
    /// associated to this header.
    struct StackHeader {
        /// Padding, in bytes, needed for the alignment of the memory block associated with the
        /// header. The padding accounts for both the size of the header and the needed alignment.
        usize padding;

        /// The capacity, in bytes, of the memory block associated with this header.
        usize capacity;

        /// Pointer offset, relative to the stack allocator memory  block, to the start of the
        /// memory address of the last allocated block (after its header).
        usize previous_offset;
    };

    /// Stack memory allocator.
    ///
    /// Note: The stack allocator **doesn't own** its memory, but merely manages it. That being
    /// said,
    ///       a stack allocator will never call `malloc` or `free`
    ///
    /// Memory layout:
    ///
    /// ```md
    ///          previous
    ///           offset                          current
    ///        for header 2                       offset
    ///             ^                               ^
    ///             |                               |
    ///    |header 1|memory 1|++++|header 2|memory 2| free space |
    ///    ^                 ^             ^                     ^
    ///    |                 |---padding---|                     |
    ///  start                             |                    end
    ///    |                            previous                 |
    ///    |                             offset                  |
    ///    |                                                     |
    ///    |--------------------- capacity ----------------------|
    /// ```
    ///
    /// Where each block of memory is preceded by a padding that comprises both the alignment needed
    /// for the memory block and its corresponding header. This header will store the size of the
    /// padding and the offset to the start the previous memory block with respect to itself.
    ///
    /// It is to be noted that the pointers returned by the methods associated to `StackAlloc` are
    /// all raw pointers. This means that if you get a memory block via `StackAlloc::alloc` and
    /// later free it via a `StackAlloc::clear_at`, you'll end up with a dangling pointer and
    /// use-after-free problems may arise if you later read from this pointer. This goes to say that
    /// the user should know how to correctly handle their memory reads and writes.
    struct Stack {
        /// Pointer to the memory region managed by the allocator.
        u8* memory = nullptr;

        /// Total capacity, in bytes, of the allocator.
        usize capacity = 0;

        /// Offset, in bytes, to the start of the free space region.
        usize offset = 0;

        /// Pointer offset relative to the start of the memory address of the last allocated block
        /// (after its header).
        usize previous_offset = 0;

        constexpr explicit Stack() noexcept = default;

        explicit Stack(u8* _memory, usize _capacity) noexcept
            : memory{_memory}, capacity{_capacity} {
            if (capacity != 0) {
                psh_assert_msg(
                    memory != nullptr,
                    "Stack constructed with non-zero capacity but null memory");
            }
        }

        void init(u8* _memory, usize _capacity) noexcept {
            memory   = _memory;
            capacity = _capacity;
            if (capacity != 0) {
                psh_assert_msg(
                    memory != nullptr,
                    "Stack::init called with non-zero capacity but null memory");
            }
        }

        /// Gets the total size, in bytes, of the memory used by the allocator.
        [[nodiscard]] usize used() const noexcept {
            return offset;
        }

        /// Gets a pointer to the memory of the last allocated memory block of the stack.
        [[nodiscard]] u8* top() const noexcept {
            if (memory == nullptr || previous_offset == 0) {
                return nullptr;
            }
            return memory + previous_offset;
        }

        /// Gets a pointer to the header associated to the top memory block of the stack.
        [[nodiscard]] StackHeader const* top_header() const noexcept {
            if (memory == nullptr || previous_offset == 0) {
                return nullptr;
            }

            return reinterpret_cast<StackHeader const*>(
                memory + previous_offset - sizeof(StackHeader));
        }

        /// Get the capacity of the top memory block.
        [[nodiscard]] usize top_capacity() const noexcept {
            auto* const header = top_header();
            return (header == nullptr) ? 0 : header->capacity;
        }

        /// Get the previous offset of the top memory block.
        [[nodiscard]] usize top_previous_offset() const noexcept {
            auto* const header = top_header();
            return (header == nullptr) ? 0 : header->previous_offset;
        }

        /// Gets a pointer to the header associated to the given memory block.
        [[nodiscard]] StackHeader const* header_of(u8 const* block) const noexcept {
            if (block == nullptr) {
                return nullptr;
            }

            // Ensure the memory address is within the allocator memory.
            if (block < memory || block >= memory + capacity) {
                log(LogLevel::Error,
                    "StackAlloc::header_of called with a pointer to a block of memory outside "
                    "of the stack allocator scope.");
                return nullptr;
            }

            // Ensure the block isn't free.
            if (block > memory + previous_offset) {
                log(LogLevel::Error,
                    "StackAlloc::header_of called with a pointer to a freed block of memory.");
                return nullptr;
            }

            // Ensure the header address is valid.
            u8 const* const block_header = block + sizeof(StackHeader);
            if (block_header < memory) {
                log(LogLevel::Error,
                    "StackAlloc::header_of expected the memory block header to be contained in "
                    "the stack allocator scope.");
                return nullptr;
            }

            return reinterpret_cast<StackHeader const*>(block_header);
        }

        /// Get the capacity of the given memory block.
        [[nodiscard]] usize capacity_of(u8 const* mem) const noexcept {
            auto* const header = header_of(mem);
            return (header == nullptr) ? 0 : header->capacity;
        }

        /// Get the previous offset of the given memory block.
        [[nodiscard]] usize previous_offset_of(u8 const* mem) const noexcept {
            auto* const header = header_of(mem);
            return (header == nullptr) ? 0 : header->previous_offset;
        }

        /// Allocates a new block of memory.
        ///
        /// Parameters:
        ///     * `length`: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        T* alloc(usize length) noexcept {
            if (length == 0) {
                return nullptr;
            }

            usize const new_block_size = array_size<T>(length);

            u8* const   free_mem = memory + offset;
            usize const padding  = padding_with_header(
                reinterpret_cast<uptr>(free_mem),
                alignof(T),
                sizeof(StackHeader),
                alignof(StackHeader));

            if (usize req = padding + new_block_size, avail = wrap_sub(capacity, offset);
                req > avail) {
                log_fmt(
                    LogLevel::Error,
                    "StackAlloc::alloc unable to allocate %zu bytes of memory (%zu bytes required "
                    "due to alignment and padding). The stack allocator has only %zu bytes "
                    "remaining.",
                    new_block_size,
                    req,
                    avail);
                return nullptr;
            }

            // Address to the start of the new block of memory.
            u8* const new_block = free_mem + padding;

            // Write to the header associated with the new block of memory.
            auto* const new_header =
                reinterpret_cast<StackHeader*>(new_block - sizeof(StackHeader));
            new_header->padding         = padding;
            new_header->capacity        = new_block_size;
            new_header->previous_offset = previous_offset;

            // Update the stack offsets.
            previous_offset = offset + padding;
            offset += padding + new_block_size;

            return reinterpret_cast<T*>(new_block);
        }

        /// Allocate a new zeroed block of memory.
        ///
        /// Parameters:
        ///     * `length`: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        T* zero_alloc(usize length) noexcept {
            auto* const ptr = alloc<T>(length);
            if (ptr == nullptr) {
                log_fmt(
                    LogLevel::Error,
                    "StackAlloc::zero_alloc unable to allocate %zu bytes of memory.",
                    array_size<T>(length));
                return nullptr;
            }

            memory_set(fat_ptr_as_bytes(ptr, length), 0);
            return ptr;
        }

        /// Reallocate a block of memory of a given type.
        ///
        /// Parameters:
        ///     * `block`: Pointer to the start of the memory block to be resized.
        ///     * `new_length`: Number of entities of type `T` that the new memory block should be
        ///                     able to contain.
        template <typename T>
        T* realloc(T const* block, usize new_length) noexcept {
            u8 const* ublock = reinterpret_cast<u8*>(block);
            if (new_length == 0) {
                clear_at(ublock);
                return nullptr;
            }

            usize const new_size = array_size<T>(new_length);

            // If `ptr` is the last allocated block, just adjust the offsets.
            if (ublock == top()) {
                offset = previous_offset + new_size;
                return block;
            }

            // Check if the address is within the allocator's memory.
            if (ublock < memory || ublock >= memory + capacity) {
                log(LogLevel::Error,
                    "StackAlloc::realloc called with a pointer outside of the memory region "
                    "managed by the stack allocator.");
                return nullptr;
            }

            // Check if the address is already free.
            if (ublock >= memory + offset) {
                log(LogLevel::Error,
                    "StackAlloc::realloc called with a free block of memory (use-after-free "
                    "error).");
                return nullptr;
            }

            auto const* const header =
                reinterpret_cast<StackHeader const*>(ublock - sizeof(StackHeader));

            if (header->capacity == new_size) {
                log_fmt(
                    LogLevel::Warning,
                    "StackAlloc::realloc called to reallocate to resize a block with its own "
                    "current size of %zu, which is unneeded.",
                    header->capacity);
                return block;
            }

            // Check memory availability.
            if (usize const avail = wrap_sub(capacity, offset); new_size > avail) {
                log_fmt(
                    LogLevel::Error,
                    "StackAlloc::realloc cannot reallocate memory from size %zu to %zu. Only %zu "
                    "bytes of memory remaining.",
                    header->capacity,
                    new_size,
                    avail);
                return nullptr;
            }

            // Allocate the new block.
            auto* const new_mem = alloc<T>(new_length);
            if (new_mem == nullptr) {
                log(LogLevel::Error,
                    "StackAlloc::realloc unable to resize the given block of memory.");
                return nullptr;
            }

            // Copy the data to the new address.
            usize const min_size = min(header->capacity, new_size);
            memory_copy(
                reinterpret_cast<u8*>(new_mem),
                reinterpret_cast<u8 const*>(block),
                min_size);

            return new_mem;
        }

        /// Tries to pop the last memory block allocated by the given stack.
        ///
        /// This function won't panic if the stack is empty, it will simply return false.
        bool pop() noexcept {
            if (previous_offset == 0) {
                return false;
            }

            u8 const* const   top = memory + previous_offset;
            auto const* const top_header =
                reinterpret_cast<StackHeader const*>(top - sizeof(StackHeader));

            offset          = previous_offset - top_header->padding;
            previous_offset = top_header->previous_offset;
            return true;
        }

        /// Tries to reset the office to the start of the header of the block pointed by `ptr`.
        ///
        /// Parameters:
        ///     * `block`: Pointer to the memory block that should be freed (all blocks above `ptr`
        ///     will
        ///              also be freed).
        ///
        /// Note:
        ///     * If `block` is null, we simply return `false`.
        ///     * If `block` doesn't correspond to a correct memory block we'll never be able to
        ///       match its location and therefore we'll end up clearing the entirety of the stack.
        ///       If this is your goal, prefer using StackAlloc::clear() instead.
        bool clear_at(u8 const* block) noexcept {
            if (block == nullptr) {
                return false;
            }

            // Check if the block is within the allocator's memory.
            if (block < memory || block > memory + previous_offset) {
                if (block > memory + capacity) {
                    log(LogLevel::Error,
                        "StackAlloc::free_at called with a pointer outside of the stack "
                        "allocator memory region.");
                    return false;
                }
                log(LogLevel::Error,
                    "StackAlloc::free_at called with a pointer to an already free region of the "
                    "stack allocator memory.");
                return false;
            }

            auto const* const header =
                reinterpret_cast<StackHeader const*>(block - sizeof(StackHeader));

            offset = wrap_sub(
                wrap_sub(reinterpret_cast<uptr>(block), header->padding),
                reinterpret_cast<uptr>(memory));
            previous_offset = header->previous_offset;

            return true;
        }

        /// Reset the allocator's offset.
        void clear() noexcept {
            offset          = 0;
            previous_offset = 0;
        }
    };
}  // namespace psh
