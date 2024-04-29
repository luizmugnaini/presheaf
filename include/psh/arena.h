/// Arena allocator.
///
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#pragma once

#include <psh/assert.h>
#include <psh/io.h>
#include <psh/math.h>
#include <psh/mem_utils.h>
#include <psh/types.h>

namespace psh {
    /// Arena allocator
    ///
    /// The arena allocator is great for the management of temporary allocation of memory, since an
    /// allocation takes nothing more than incrementing an offset.
    ///
    /// Note:
    ///     * The arena does not own memory, thus it is not responsible for the freeing of it.
    ///     * Any functions that return a raw pointer should be checked for nullity since allocation
    ///       may fail.
    struct Arena {
        u8*   memory   = nullptr;  ///< Not-owned memory block managed by the arena allocator.
        usize offset   = 0;        ///< The current offset to the free-space in the memory block.
        usize capacity = 0;        ///< The capacity, in bytes, of the memory block.

        constexpr explicit Arena() noexcept = default;

        /// Create an arena with a given block of memory and capacity.
        explicit Arena(u8* _memory, usize _capacity) noexcept
            : memory{_memory}, capacity{_capacity} {
            if (capacity != 0) {
                psh_assert_msg(
                    memory != nullptr,
                    "Arena created with inconsistent data: non-zero capacity but null memory");
            }
        }

        /// Reset the offset of the allocator.
        inline void clear() noexcept {
            offset = 0;
        }

        /// Allocates a block of memory that fits a given number of entities of type `T`.
        ///
        /// Parameters:
        ///     * length: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        T* alloc(usize length) noexcept {
            if (length == 0 || capacity == 0) return nullptr;

            uptr const memory_addr    = reinterpret_cast<uptr>(memory);
            uptr const new_block_addr = align_forward(memory_addr + offset, alignof(T));

            // Check if there is enough memory.
            usize const size = sizeof(T) * length;
            if (new_block_addr + size > capacity + memory_addr) {
                log_fmt(
                    LogLevel::Error,
                    "ArenaAlloc::alloc unable to allocate %zu bytes of memory (%zu bytes required "
                    "due to alignment). The allocator has only %zu bytes remaining.",
                    size,
                    wrap_sub(size + new_block_addr, (offset + memory_addr)),
                    capacity - offset);
                return nullptr;
            }

            offset = wrap_sub(size + new_block_addr, memory_addr);
            return reinterpret_cast<T*>(new_block_addr);
        }

        /// Allocate a zeroed block of memory.
        ///
        /// Allocates a block of memory of a given size for a given type, accounting for the
        /// alignment needed for the given type `T`. Additionally the block is zeroed before
        /// returning to the caller.
        ///
        /// Parameters:
        ///     * length: Number of entities of type `T` that should fit in the new block.
        template <typename T>
        T* zero_alloc(usize length) noexcept {
            T* const ptr = alloc<T>(length);
            if (ptr == nullptr) return nullptr;

            memory_set(fat_ptr_as_bytes(ptr, length), 0);
            return ptr;
        }

        /// Reallocate a block of memory of a given type.
        ///
        /// The memory alignment required for the type `T` is automatically handled for the user.
        ///
        /// Parameters:
        ///     * block: Pointer to the start of the memory block to be resized.
        ///     * current_capacity: Current number of entities of type `T` that `block` is capable
        ///       of holding.
        ///     * new_capacity: Number of entities of type `T` that the new memory block should be
        ///       able to contain.
        template <typename T>
        T* realloc(T* block, usize current_capacity, usize new_capacity) noexcept {
            // Arena allocators cannot deallocate willy-nilly.
            psh_assert_msg(
                new_capacity != 0,
                "ArenaAlloc::realloc called with a zero `new_capacity` parameter");

            // Check if there is any memory at all.
            if (capacity == 0 || memory == nullptr) return nullptr;

            // Check if the user wants to allocate a completely new block.
            if (block == nullptr || current_capacity == 0) return alloc<T>(new_capacity);

            auto const block_addr      = reinterpret_cast<uptr>(block);
            auto const memory_start    = reinterpret_cast<uptr>(memory);
            auto const memory_end      = memory_start + capacity;
            auto const start_free_addr = memory_start + offset;

            // Check if the block lies within the allocator's memory.
            if (block_addr < memory_start || block_addr >= memory_end) {
                log(LogLevel::Error,
                    "ArenaAlloc::realloc called with pointer outside of its domain.");
                return nullptr;
            }

            // Check if the block is already free.
            if (block_addr >= start_free_addr) {
                log(LogLevel::Error,
                    "ArenaAlloc::realloc called with a pointer to a free address of the arena "
                    "domain.");
                return nullptr;
            }

            usize const current_size = sizeof(T) * current_capacity;
            usize const new_size     = sizeof(T) * new_capacity;

            // If the block is the last allocated, just bump the offset.
            if (block_addr == wrap_sub(start_free_addr, current_size)) {
                // Check if there is enough space.
                if (block_addr + new_size > memory_end) {
                    log_fmt(
                        LogLevel::Error,
                        "ArenaAlloc::realloc unable to reallocate block from %zu bytes to %zu "
                        "bytes.",
                        current_size,
                        new_size);
                    return nullptr;
                }
                offset += wrap_sub(new_size, current_size);
                return block;
            }

            T* const new_mem = zero_alloc<T>(new_capacity);
            if (new_mem == nullptr) return nullptr;

            // Copy the existing data to the new block.
            usize const copy_size = min(current_size, new_size);
            memory_copy(
                reinterpret_cast<u8*>(new_mem),
                reinterpret_cast<u8 const*>(block),
                copy_size);

            return new_mem;
        }

        /// Scratch arena.
        ///
        /// This allocator is used to save the state of the parent arena at creation time and
        /// restore the parent arena offset state at destruction time.
        ///
        /// You can nest many scratch arenas throughout different lifetimes by decoupling the
        /// current scratch arena into a new one.
        struct Scratch {
            Arena* parent_;
            usize  saved_offset_ = 0;

            Scratch(Scratch&&) = default;

            /// Create a new scratch arena out of an existing arena.
            constexpr explicit Scratch(Arena* parent) noexcept
                : parent_{parent}, saved_offset_{parent->offset} {}

            /// Reset the parent offset.
            ~Scratch() noexcept {
                parent_->offset = saved_offset_;
            }

            /// The parent arena associated to the scratch arena.
            [[nodiscard]] Arena* arena() const noexcept {
                return parent_;
            }

            /// The offset of the parent arena at the time of creation of the scratch arena.
            [[nodiscard]] usize saved_offset() const noexcept {
                return saved_offset_;
            }

            /// Create a new scratch arena with the current state of the parent.
            [[nodiscard]] Scratch decouple() const noexcept {
                return Scratch{parent_};
            }

            // The scratch arena should never be passed around by reference because it defeats its
            // purpose. You should always opt for `Scratch::decouple` in such cases.
            Scratch()                          = delete;
            Scratch(Scratch&)                  = delete;
            Scratch(Scratch const&)            = delete;
            Scratch& operator=(Scratch)        = delete;
            Scratch& operator=(Scratch&)       = delete;
            Scratch& operator=(Scratch const&) = delete;
        };

        /// Create a new scratch arena.
        Scratch make_scratch() noexcept {
            return Scratch{this};
        }
    };

    using ScratchArena = Arena::Scratch;
}  // namespace psh
