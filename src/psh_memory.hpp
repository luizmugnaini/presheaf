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
/// Description: Utilities for memory-related operations.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include "psh_core.hpp"
#include "psh_debug.hpp"

namespace psh {
    // -------------------------------------------------------------------------------------------------
    // CPU architecture.
    // -------------------------------------------------------------------------------------------------

    psh_proc psh_inline bool arch_is_little_endian() psh_no_except {
        i32 integer = 1;
        return static_cast<bool>(*(reinterpret_cast<u8*>(&integer)));
    }

    psh_proc psh_inline bool arch_is_big_endian() psh_no_except {
        i32 integer = 1;
        return static_cast<bool>(!*(reinterpret_cast<u8*>(&integer)));
    }

    // -------------------------------------------------------------------------------------------------
    // Virtual memory handling.
    //
    // TODO: add procedures for reserving/commiting memory separately.
    // -------------------------------------------------------------------------------------------------

    /// Reserve and commit a virtual block of memory.
    ///
    /// The memory allocated is always initialised to zero.
    ///
    /// Directly calls the respective system call for allocating memory. Should be
    /// used for large allocations, if you want small allocations, malloc is the way to go.
    psh_proc u8* memory_virtual_alloc(usize size_bytes) psh_no_except;

    /// Release and decommit all memory.
    psh_proc void memory_virtual_free(u8* memory, usize size_bytes) psh_no_except;

    // -------------------------------------------------------------------------------------------------
    // Raw memory manipulation.
    // -------------------------------------------------------------------------------------------------

    /// Set the value of a given range of bytes.
    psh_proc void memory_set(u8* memory, usize size_bytes, i32 fill) psh_no_except;

    /// Zero-out all of the members of a given structure.
    template <typename T>
    psh_proc psh_inline void zero_struct(T* s) psh_no_except {
        memory_set(reinterpret_cast<u8*>(s), sizeof(T), 0);
    }

    /// Copy non-overlapping memory regions.
    psh_proc void memory_copy(u8* psh_no_alias dst, u8 const* psh_no_alias src, usize size_bytes) psh_no_except;

    /// Copy possibly-overlapping memory regions.
    psh_proc void memory_move(u8* psh_no_alias dst, u8 const* psh_no_alias src, usize size_bytes) psh_no_except;

    // -------------------------------------------------------------------------------------------------
    // Alignment utilities.
    // -------------------------------------------------------------------------------------------------

    /// Compute the padding needed for the alignment of the memory and header.
    ///
    /// The padding should contain the header, thus it is ensured that padding >= header_size.
    /// Both the alignment needed for the new memory block as the alignment required by the header
    /// will be accounted when calculating the padding.
    ///
    /// Parameters:
    ///     - ptr: The current memory address.
    ///     - alignment: The alignment requirement for the new memory block.
    ///     - header_size: The total size of the header associated to the new memory block.
    ///     - header_alignment: The alignment required by the header.
    ///
    /// Return: The resulting padding with respect to ptr that should satisfy the alignment
    ///         requirements, as well as accommodating the associated header.
    psh_proc usize padding_with_header(
        uptr  ptr,
        usize alignment,
        usize header_size,
        usize header_alignment) psh_no_except;

    /// Compute the next address that satisfies a given alignment.
    ///
    /// The alignment should always be a power of two.
    ///
    /// Parameters:
    ///     - ptr: The starting address.
    ///     - alignment: The alignment requirement.
    ///
    /// Return: The next address, relative to ptr that satisfies the alignment requirement imposed
    ///         by alignment.
    psh_proc usize align_forward(uptr ptr, usize alignment) psh_no_except;

    // -------------------------------------------------------------------------------------------------
    // Arena memory allocator.
    // -------------------------------------------------------------------------------------------------

    // Forward declaration.
    struct Arena;

    /// Manually managed checkpoint for arenas.
    ///
    /// You can create a checkpoint with Arena::make_checkpoint and restore the arena to
    /// a given checkpoint via Arena::restore_checkpoint.
    struct ArenaCheckpoint {
        Arena* arena;
        usize  saved_offset;
    };

    /// Arena allocator
    ///
    /// The arena allocator is great for the management of temporary allocation of memory, since an
    /// allocation takes nothing more than incrementing an offset.
    ///
    /// @NOTE: - The arena does not own memory, thus it is not responsible for the freeing of it.
    ///        - All allocation procedures will zero-out the whole allocated block.
    struct Arena {
        u8*   buf;
        usize capacity = 0;
        usize offset   = 0;
    };

    psh_proc psh_inline Arena make_arena(u8* buf, usize capacity) psh_no_except {
        return Arena{
            .buf      = buf,
            .capacity = (buf != nullptr) ? capacity : 0,
            .offset   = 0,
        };
    }

    /// initialise the arena with a given memory buffer and a capacity.
    psh_proc psh_inline void init_arena(Arena* arena, u8* buf, usize capacity) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(arena));
        arena->buf      = buf;
        arena->capacity = (buf != nullptr) ? capacity : 0;
    }

    /// Reset the offset of the allocator.
    psh_proc psh_inline void arena_clear(Arena* arena) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(arena));
        arena->offset = 0;
    }

    /// Make an arena that owns its memory.
    ///
    /// Since the arena is not aware of the ownership, this function call has to be paired
    /// with destroy_owned_arena.
    psh_proc Arena make_owned_arena(usize capacity) psh_no_except;

    /// Free the memory of an arena that owns its memory.
    ///
    /// This function should only be called for arenas that where created by make_owned_arena.
    psh_proc void destroy_owned_arena(Arena* arena) psh_no_except;

    /// Create a restorable checkpoint for the arena. This is a more flexible alternative to the
    /// ScratchArena construct since you can manually restore the arena, not relying on destructors.
    psh_proc psh_inline ArenaCheckpoint make_arena_checkpoint(Arena* arena) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(arena));
        return ArenaCheckpoint{
            .arena        = arena,
            .saved_offset = arena->offset,
        };
    }

    /// Restore the arena state to a given checkpoint.
    psh_proc psh_inline void arena_checkpoint_restore(ArenaCheckpoint checkpoint) psh_no_except {
        psh_paranoid_validate_usage({
            psh_assert_not_null(checkpoint.arena);
            psh_assert_fmt(
                checkpoint.saved_offset <= checkpoint.arena->offset,
                "Invalid checkpoint. Cannot restore the arena to an offset (%zu) bigger than the current (%zu).",
                checkpoint.saved_offset,
                checkpoint.arena->offset);
        });

        checkpoint.arena->offset = checkpoint.saved_offset;
    }

    /// Scratch arena, an automatic checkpoint manager for arena offsets.
    ///
    /// This allocator is used to save the state of the parent arena at creation time and
    /// restore the parent arena offset state at destruction time.
    ///
    /// You can nest many scratch arenas throughout different lifetimes by creating scratch arenas
    /// at distinct scopes.
    ///
    /// Scratch arenas shouldn't be passed to other functions, they should be used in a single scope.
    struct ScratchArena {
        Arena* arena = nullptr;
        usize  saved_offset;

        psh_inline ScratchArena(Arena* arena_) psh_no_except {
            psh_paranoid_validate_usage(psh_assert_not_null(arena_));
            this->arena        = arena_;
            this->saved_offset = arena_->offset;
        }

        psh_inline ~ScratchArena() psh_no_except {
            this->arena->offset = this->saved_offset;
        }

        // @NOTE: Required when compiling as a DLL since the compiler will require all standard
        //        member functions to be defined.
        ScratchArena& operator=(ScratchArena&) = delete;
    };

    // -------------------------------------------------------------------------------------------------
    // Stack memory allocator.
    // -------------------------------------------------------------------------------------------------

    /// Header associated with each memory block in the stack allocator.
    ///
    /// Memory layout:
    ///
    ///           previous_offset                        |-capacity-|
    ///                  ^                               ^          ^
    ///                  |                               |          |
    ///  |previous header|previous memory|++++++++|header|  memory  |
    ///                                  ^               ^
    ///                                  |----padding----|
    ///
    /// where "header" represents this current header, and "memory" represents the memory block
    /// associated to this header. The composition of the structure is given by:
    /// - Padding, in bytes, needed for the alignment of the memory block associated with the
    ///   header. The padding accounts for both the size of the header and the needed alignment.
    /// - The capacity, in bytes, of the memory block associated with this header.
    /// - Pointer offset, relative to the stack allocator memory  block, to the start of the
    ///   memory address of the last allocated block (after its header).
    struct StackHeader {
        usize padding;
        usize capacity;
        usize previous_offset;
    };

    /// Stack memory allocator.
    ///
    /// Note: The stack allocator **doesn't own** its memory, but merely manages it. That being
    ///       said, a stack allocator will never call malloc or free.
    ///
    /// Memory layout:
    ///
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
    ///
    /// Where each block of memory is preceded by a padding that comprises both the alignment needed
    /// for the memory block and its corresponding header. This header will store the size of the
    /// padding and the offset to the start the previous memory block with respect to itself.
    ///
    /// It is to be noted that the pointers returned by the methods associated to StackAlloc are
    /// all raw pointers. This means that if you get a memory block via StackAlloc::alloc and
    /// later free it via a StackAlloc::clear_at, you'll end up with a dangling pointer and
    /// use-after-free problems may arise if you later read from this pointer. This goes to say that
    /// the user should know how to correctly handle their memory reads and writes.
    struct Stack {
        u8*   buf;
        usize capacity        = 0;
        usize offset          = 0;
        usize previous_offset = 0;

        psh_inline void init(u8* buf_, usize capacity_) psh_no_except {
            psh_paranoid_validate_usage(psh_assert_msg(this->capacity == 0, "Stack already initialised."));
            this->buf      = buf_;
            this->capacity = (buf_ != nullptr) ? capacity_ : 0;
        }

        /// Gets a pointer to the memory of the last allocated memory block of the stack.
        u8* top() psh_no_except;

        /// Gets a pointer to the header associated to the top memory block of the stack.
        StackHeader const* top_header() const psh_no_except;

        /// Get the size of the top memory block.
        usize top_size() const psh_no_except;

        /// Get the previous offset of the top memory block.
        usize top_previous_offset() const psh_no_except;

        /// Gets a pointer to the header associated to the given memory block.
        StackHeader const* header_of(u8 const* block) const psh_no_except;

        /// Get the size of the given memory block.
        usize size_of(u8 const* block) const psh_no_except;

        /// Get the previous offset of the given memory block.
        usize previous_offset_of(u8 const* block) const psh_no_except;

        /// Tries to pop the last memory block allocated by the given stack.
        ///
        /// This function won't panic if the stack is empty, it will simply return false.
        Status pop() psh_no_except;

        /// Tries to reset the office to the start of the header of the block pointed by ptr.
        ///
        /// Parameters:
        ///     - block: Pointer to the memory block that should be freed (all blocks above ptr
        ///              will also be freed).
        ///
        /// Note:
        ///     - If block is null, we simply return false.
        ///     - If block doesn't correspond to a correct memory block we'll never be able to
        ///       match its location and therefore we'll end up clearing the entirety of the stack.
        ///       If this is your goal, prefer using StackAlloc::clear() instead.
        Status clear_at(u8 const* block) psh_no_except;

        /// Reset the allocator's offset.
        psh_inline void clear() psh_no_except {
            this->offset          = 0;
            this->previous_offset = 0;
        }
    };

    // -------------------------------------------------------------------------------------------------
    // Memory allocation manager based on the stack allocator.
    // -------------------------------------------------------------------------------------------------

    // @TODO: Add the following capabilities
    // - Resizing.
    // - Reserving memory without commiting.
    // - Commiting memory manually.
    // - More memory statistics for debugging purposes.

    /// A stack allocator manager that can be used as the central memory resource of an application.
    struct MemoryManager {
        usize allocation_count = 0;
        Stack allocator        = {};

        /// Allocate memory and initialise the underlying memory allocator.
        void init(usize capacity) psh_no_except;

        /// Free all acquired memory.
        void destroy() psh_no_except;

        /// Try to free the last allocated block of memory.
        Status pop() psh_no_except;

        /// Try to reset the allocator offset until the specified memory block.
        ///
        /// Note: If the caller passes a pointer to a wrong address and we can't tell that easily,
        ///       the stack will be *completely cleaned*, beware!
        ///
        /// Parameters:
        ///     - block: Pointer to the memory block that should be freed (all blocks above
        ///              the given one will also be freed). If this pointer is null, outside of
        ///              the stack allocator buffer, or already free, the program return false and
        ///              won't panic.
        Status clear_until(u8 const* block) psh_no_except;

        /// Resets the manager by zeroing the memory offset and statistics.
        void clear() psh_no_except;

        // @NOTE: Required when compiling as a DLL since the compiler will require all standard member
        //        functions to be defined.
        MemoryManager& operator=(MemoryManager&) = delete;
    };

    // -------------------------------------------------------------------------------------------------
    // Memory handling using the Presheaf custom allocators.
    // -------------------------------------------------------------------------------------------------

    /// Allocate a new block of memory with a given alignment.
    psh_proc u8* memory_alloc_align(Arena* arena, usize size_bytes, u32 alignment) psh_no_except;
    psh_proc u8* memory_alloc_align(Stack* stack, usize size_bytes, u32 alignment) psh_no_except;

    /// Allocates a new block of memory capable of holding a certain count of elements of a
    /// given type.
    template <typename T>
    psh_proc psh_inline T* memory_alloc(Arena* arena, usize count) psh_no_except {
        return reinterpret_cast<T*>(memory_alloc_align(arena, sizeof(T) * count, alignof(T)));
    }
    template <typename T>
    psh_proc psh_inline T* memory_alloc(Stack* stack, usize count) psh_no_except {
        return reinterpret_cast<T*>(memory_alloc_align(stack, sizeof(T) * count, alignof(T)));
    }
    template <typename T>
    psh_proc psh_inline T* memory_alloc(MemoryManager* memory_manager, usize count) psh_no_except {
        if (memory_manager == nullptr) {
            return nullptr;
        }

        T* const new_block = memory_alloc<T>(&memory_manager->allocator, count);
        memory_manager->allocation_count += static_cast<usize>(new_block != nullptr);
        return new_block;
    }

    /// Reallocate an existing block of memory with a given alignment.
    psh_proc u8* memory_realloc_align(
        Arena* arena,
        u8*    block,
        usize  current_size_bytes,
        usize  new_size_bytes,
        u32    alignment) psh_no_except;
    psh_proc u8* memory_realloc_align(
        Stack* stack,
        u8*    block,
        usize  new_size_bytes,
        u32    alignment) psh_no_except;

    /// Reallocate a block of memory of a given type.
    ///
    /// Parameters:
    ///     - block: Pointer to the start of the memory block to be resized.
    ///     - current_count: The current number of entities of type T that fits in the block.
    ///     - new_count: Number of entities of type T that the new memory block should be
    ///                  able to contain.
    template <typename T>
    psh_proc psh_inline T* memory_realloc(Arena* arena, T* block, usize current_count, usize new_count) psh_no_except {
        return reinterpret_cast<T*>(memory_realloc_align(
            arena,
            reinterpret_cast<u8*>(block),
            sizeof(T) * current_count,
            sizeof(T) * new_count,
            alignof(T)));
    }
    template <typename T>
    psh_proc psh_inline T* memory_realloc(Stack* stack, T* block, usize new_count) psh_no_except {
        return reinterpret_cast<T*>(memory_realloc_align(stack, block, sizeof(T) * new_count));
    }
    template <typename T>
    psh_proc psh_inline T* memory_realloc(MemoryManager* memory_manager, T* block, usize new_count) psh_no_except {
        if (memory_manager == nullptr) {
            return nullptr;
        }

        T* const new_block = memory_realloc<T>(&memory_manager->allocator, block, new_count);
        memory_manager->allocation_count += static_cast<usize>(new_block != block);
        return new_block;
    }

    // -------------------------------------------------------------------------------------------------
    // Common code-generation for index-based/iterator access to Presheaf containers.
    //
    // Also defines the inner type ValueType, which is used for functions that are polymorphic on any of
    // the Presheaf containers.
    // -------------------------------------------------------------------------------------------------

#define psh_impl_generate_container_boilerplate(InnerType, this_buf, this_count)      \
    using ValueType = InnerType;                                                      \
    psh_inline InnerType& operator[](usize idx) psh_no_except {                       \
        psh_assert_bounds_check(idx, this_count);                                     \
        return this_buf[idx];                                                         \
    }                                                                                 \
    psh_inline InnerType const& operator[](usize idx) const psh_no_except {           \
        psh_assert_bounds_check(idx, this_count);                                     \
        return this_buf[idx];                                                         \
    }                                                                                 \
    psh_inline InnerType*       begin() psh_no_except { return this_buf; }            \
    psh_inline InnerType*       end() psh_no_except { return this_buf + this_count; } \
    psh_inline InnerType const* begin() const psh_no_except { return this_buf; }      \
    psh_inline InnerType const* end() const psh_no_except { return this_buf + this_count; }

#define psh_impl_generate_constexpr_container_boilerplate(InnerType, this_buf, this_count)      \
    using ValueType = InnerType;                                                                \
    psh_inline constexpr InnerType& operator[](usize idx) psh_no_except {                       \
        psh_assert_bounds_check(idx, this_count);                                               \
        return this_buf[idx];                                                                   \
    }                                                                                           \
    psh_inline constexpr InnerType const& operator[](usize idx) const psh_no_except {           \
        psh_assert_bounds_check(idx, this_count);                                               \
        return this_buf[idx];                                                                   \
    }                                                                                           \
    psh_inline constexpr InnerType*       begin() psh_no_except { return this_buf; }            \
    psh_inline constexpr InnerType*       end() psh_no_except { return this_buf + this_count; } \
    psh_inline constexpr InnerType const* begin() const psh_no_except { return this_buf; }      \
    psh_inline constexpr InnerType const* end() const psh_no_except { return this_buf + this_count; }

    // -------------------------------------------------------------------------------------------------
    // Fat pointers.
    // -------------------------------------------------------------------------------------------------

    /// Fat pointer, holds a pointer to a buffer and its corresponding size.
    template <typename T>
    struct FatPtr {
        T*    buf;
        usize count = 0;

        psh_impl_generate_container_boilerplate(T, this->buf, this->count)
    };

    /// Create a fat pointer with mutable access to a slice of a given Presheaf container.
    template <typename Container, typename T = Container::ValueType>
    psh_proc psh_inline FatPtr<T> make_slice(Container* c, usize start_idx, usize slice_count) psh_no_except {
        psh_validate_usage(psh_static_assert_valid_const_container_type(Container, c));
        psh_paranoid_validate_usage(psh_assert_not_null(c));
        psh_validate_usage(psh_assert_bounds_check(slice_count, c->count + 1));

        return FatPtr<T>{c->buf + start_idx, slice_count};
    }

    /// Create a fat pointer with immutable access to a slice of a given Presheaf container.
    template <typename Container, typename T = Container::ValueType>
    psh_proc psh_inline FatPtr<T const> make_const_slice(Container const* c, usize start_idx, usize slice_count) psh_no_except {
        psh_validate_usage(psh_static_assert_valid_const_container_type(Container, c));
        psh_paranoid_validate_usage(psh_assert_not_null(c));
        psh_validate_usage(psh_assert_bounds_check(start_idx + slice_count - 1, c->count + 1));

        return FatPtr<T const>{c->buf + start_idx, slice_count};
    }

    /// Create a fat pointer with mutable access to a Presheaf container.
    template <typename Container, typename T = Container::ValueType>
    psh_proc psh_inline FatPtr<T> make_fat_ptr(Container* c) psh_no_except {
        psh_validate_usage(psh_static_assert_valid_mutable_container_type(Container, c));
        psh_paranoid_validate_usage(psh_assert_not_null(c));

        return FatPtr<T>{c->buf, c->count};
    }

    /// Create a fat pointer with immutable access to a Presheaf container.
    template <typename Container, typename T = Container::ValueType>
    psh_proc psh_inline FatPtr<T const> make_const_fat_ptr(Container const* c) psh_no_except {
        psh_validate_usage(psh_static_assert_valid_const_container_type(Container, c));
        psh_paranoid_validate_usage(psh_assert_not_null(c));

        return FatPtr<T const>{reinterpret_cast<T const*>(c->buf), c->count};
    }

    /// Query the current size in bytes of a given Presheaf container.
    template <typename Container, typename T = Container::ValueType>
    psh_proc usize size_bytes(Container const* c) psh_no_except {
        psh_validate_usage(psh_static_assert_valid_const_container_type(Container, c));
        psh_paranoid_validate_usage(psh_assert_not_null(c));

        return sizeof(T) * c->count;
    }

    // -------------------------------------------------------------------------------------------------
    // Plain old buffers.
    // -------------------------------------------------------------------------------------------------

    /// Buffer with a compile-time known size.
    template <typename T, usize count_>
    struct Buffer {
        T                      buf[count_] = {};
        static constexpr usize count       = count_;

        psh_impl_generate_constexpr_container_boilerplate(T, this->buf, count_)
    };

    /// Growable buffer with limited a compile-time known maximum size.
    template <typename T, usize max_count_>
    struct PushBuffer {
        T                      buf[max_count_] = {};
        usize                  count           = 0;
        static constexpr usize max_count       = max_count_;

        psh_impl_generate_constexpr_container_boilerplate(T, this->buf, this->count)
    };

    template <typename T, usize max_count>
    psh_proc psh_inline void push_buffer_push(PushBuffer<T, max_count>* push_buffer, T element) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(push_buffer));

        usize count = push_buffer->count;
        psh_validate_usage(psh_assert_fmt(count < max_count, "The buffer of max count %zu is full.", max_count));

        push_buffer->buf[count] = element;
        push_buffer->count      = count + 1;
    }

    template <typename T, usize max_count>
    psh_proc psh_inline void push_buffer_push_many(PushBuffer<T, max_count>* push_buffer, FatPtr<T const> elements) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(push_buffer));

        usize count = push_buffer->count;
        psh_validate_usage({
            psh_assert_fmt(
                count < max_count,
                "The elements don't fit in the buffer of max count %zu: current count is %zu and you're trying to push %zu elements.",
                push_buffer->max_count,
                count,
                elements.count);
        });

        memory_copy(reinterpret_cast<u8*>(push_buffer->buf + count), reinterpret_cast<u8 const*>(elements.buf), size_bytes(&elements));
        push_buffer->count = count + elements.count;
    }

    /// Effectively bump the element count by one and return the offset to the first new empty element.
    ///
    /// Note: This procedure won't cleanup the previous values (if any) in these new elements.
    template <typename T, usize max_count>
    psh_proc psh_inline usize push_buffer_push_empty(PushBuffer<T, max_count>* push_buffer, usize empty_count = 1) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(push_buffer));

        usize count = push_buffer->count;
        psh_validate_usage({
            psh_assert_fmt(
                count + empty_count <= max_count,
                "The elements don't fit in the buffer of max count %zu: current count is %zu and you're trying to push %zu elements.",
                max_count,
                count,
                empty_count);
        });

        push_buffer->count = count + empty_count;
        return count;
    }

    /// Effectively bump the element count by one and return the offset to the first new empty element.
    ///
    /// Note: This procedure won't create the default values of the underlying type, it will literally
    ///       zero-out a whole region of memory.
    template <typename T, usize max_count>
    psh_proc psh_inline usize push_buffer_push_zero(PushBuffer<T, max_count>* push_buffer, usize zeros_count = 1) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(push_buffer));

        usize count = push_buffer->count;
        psh_validate_usage({
            psh_assert_fmt(
                count + zeros_count <= max_count,
                "The elements don't fit in the buffer of max count %zu: current count is %zu and you're trying to push %zu elements.",
                max_count,
                count,
                zeros_count);
        });

        memory_set(reinterpret_cast<u8*>(push_buffer->buf + count), zeros_count * sizeof(T), 0);
        push_buffer->count = count + zeros_count;
        return count;
    }

    /// Remove elements from the end of the buffer.
    ///
    /// Note: This procedure won't call any destructors.
    template <typename T, usize max_count>
    psh_proc psh_inline void push_buffer_pop(PushBuffer<T, max_count>* push_buffer, usize pop_count = 1) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(push_buffer));
        psh_validate_usage(psh_assert_fmt(pop_count <= push_buffer->count, "The buffer has %zu elements but tried to pop %zu elements.", push_buffer->count, pop_count));

        push_buffer->count -= pop_count;
    }

    // -------------------------------------------------------------------------------------------------
    // Run-time known fixed length array.
    // -------------------------------------------------------------------------------------------------

    /// Array with run-time known constant capacity.
    ///
    /// The array lifetime is bound to the lifetime of the arena passed at initialization, being
    /// responsible to allocate the memory referenced by the array.
    template <typename T>
    struct Array {
        T*    buf;
        usize count = 0;

        psh_impl_generate_container_boilerplate(T, this->buf, this->count)
    };

    template <typename T>
    psh_proc psh_inline Array<T> make_array(Arena* arena, usize count) psh_no_except {
        T* buf = memory_alloc<T>(arena, count);
        return Array<T>{
            .buf   = buf,
            .count = (buf != nullptr) ? count : 0,
        };
    }

    template <typename T>
    psh_proc psh_inline void init_array(Array<T>* array, Arena* arena, usize count) psh_no_except {
        psh_paranoid_validate_usage({
            psh_assert_not_null(array);
            psh_assert_msg(array->count == 0, "Array already initialised.");
        });

        T* buf = memory_alloc<T>(arena, count);
        *array = {
            .buf   = buf,
            .count = (buf != nullptr) ? count : 0,
        };
    }

    /// Growable array with run-time known constant maximum capacity.
    ///
    /// The array lifetime is bound to the lifetime of the arena passed at initialization, being
    /// responsible to allocate the memory referenced by the array.
    template <typename T>
    struct PushArray {
        T*    buf;
        usize count     = 0;
        usize max_count = 0;

        psh_impl_generate_container_boilerplate(T, this->buf, this->count)
    };

    template <typename T>
    psh_proc psh_inline PushArray<T> make_push_array(Arena* arena, usize max_count) psh_no_except {
        T* buf = memory_alloc<T>(arena, max_count);
        return PushArray<T>{
            .buf       = buf,
            .count     = 0,
            .max_count = (buf != nullptr) ? max_count : 0,
        };
    }

    template <typename T>
    psh_proc psh_inline void init_push_array(PushArray<T>* push_array, Arena* arena, usize max_count) psh_no_except {
        psh_paranoid_validate_usage({
            psh_assert_not_null(push_array);
            psh_assert_msg(push_array->max_count == 0, "Already initialised.");
        });

        T* buf      = memory_alloc<T>(arena, max_count);
        *push_array = {
            .buf       = buf,
            .count     = 0,
            .max_count = (buf != nullptr) ? max_count : 0,
        };
    }

    template <typename T>
    psh_proc psh_inline void push_array_push(PushArray<T>* push_array, T element) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(push_array));

        usize count = push_array->count;
        psh_validate_usage(psh_assert_fmt(count < push_array->max_count, "The array of max count %zu is full.", push_array->max_count));

        push_array->buf[count] = element;
        push_array->count      = count + 1;
    }

    template <typename T>
    psh_proc psh_inline void push_array_push_many(PushArray<T>* push_array, FatPtr<T const> elements) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(push_array));

        usize count = push_array->count;
        psh_validate_usage({
            psh_assert_fmt(
                count < push_array->max_count,
                "The elements don't fit in the array of max count %zu: current count is %zu and you're trying to push %zu elements.",
                push_array->max_count,
                count,
                elements.count);
        });

        memory_copy(reinterpret_cast<u8*>(push_array->buf + count), reinterpret_cast<u8 const*>(elements.buf), size_bytes(&elements));
        push_array->count = count + elements.count;
    }

    /// Effectively bump the element count by one and return the offset to the first new empty element.
    ///
    /// Note: This procedure won't cleanup the previous values (if any) in these new elements.
    template <typename T>
    psh_proc psh_inline usize push_array_push_empty(PushArray<T>* push_array, usize empty_count = 1) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(push_array));

        usize count = push_array->count;
        psh_validate_usage({
            psh_assert_fmt(
                count + empty_count <= push_array->max_count,
                "The elements don't fit in the array of max count %zu: current count is %zu and you're trying to push %zu elements.",
                push_array->max_count,
                count,
                empty_count);
        });

        push_array->count = count + empty_count;
        return count;
    }

    /// Effectively bump the element count by one and return the offset to the first new empty element.
    ///
    /// Note: This procedure won't create the default values of the underlying type, it will literally
    ///       zero-out a whole region of memory.
    template <typename T>
    psh_proc psh_inline usize push_array_push_zero(PushArray<T>* push_array, usize zeros_count = 1) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(push_array));

        usize count = push_array->count;
        psh_validate_usage({
            psh_assert_fmt(
                count + zeros_count <= push_array->max_count,
                "The elements don't fit in the array of max count %zu: current count is %zu and you're trying to push %zu elements.",
                push_array->max_count,
                count,
                zeros_count);
        });

        memory_set(reinterpret_cast<u8*>(push_array->buf + count), zeros_count * sizeof(T), 0);
        push_array->count = count + zeros_count;
        return count;
    }

    template <typename T>
    psh_proc psh_inline void push_array_pop(PushArray<T>* push_array, usize pop_count = 1) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(push_array));
        psh_validate_usage(psh_assert_fmt(pop_count <= push_array->count, "The array has %zu elements but tried to pop %zu elements.", push_array->count, pop_count));

        push_array->count -= pop_count;
    }

    // -------------------------------------------------------------------------------------------------
    // Dynamically sized array.
    // -------------------------------------------------------------------------------------------------

    psh_global constexpr usize DYNARRAY_DEFAULT_INITIAL_CAPACITY      = 4;
    psh_global constexpr usize DYNARRAY_RESIZE_CAPACITY_GROWTH_FACTOR = 2;

    /// Run-time variable length array.
    ///
    /// A dynamic array has its lifetime bound to its associated arena.
    template <typename T>
    struct DynamicArray {
        T*     buf;
        Arena* arena;
        usize  capacity = 0;
        usize  count    = 0;

        psh_impl_generate_container_boilerplate(T, this->buf, this->count)
    };

    template <typename T>
    psh_proc psh_inline DynamicArray<T> make_dynamic_array(
        Arena* arena,
        usize  capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY) psh_no_except {
        T* buf = memory_alloc<T>(arena, capacity);
        return DynamicArray<T>{
            .buf      = buf,
            .arena    = arena,
            .capacity = (buf != nullptr) ? capacity : 0,
            .count    = 0,
        };
    }

    /// initialise the dynamic array with a given capacity.
    template <typename T>
    psh_proc psh_inline void init_dynamic_array(
        DynamicArray<T>* darray,
        Arena*           arena,
        usize            capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY) psh_no_except {
        psh_paranoid_validate_usage({
            psh_assert_not_null(darray);
            psh_assert_msg(darray->count == 0, "DynamicArray already initialised.");
        });

        T* buf  = memory_alloc<T>(arena, capacity);
        *darray = {
            .buf      = buf,
            .arena    = arena,
            .capacity = (buf != nullptr) ? capacity : 0,
            .count    = 0,
        };
    }

    /// Grow the capacity of the dynamic array underlying buffer.
    template <typename T>
    psh_proc Status dynamic_array_grow(
        DynamicArray<T>* darray,
        u32              growth_factor = DYNARRAY_RESIZE_CAPACITY_GROWTH_FACTOR) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(darray));

        usize  new_capacity      = 0;
        T*     new_buf           = nullptr;
        Arena* arena             = darray->arena;
        usize  previous_capacity = darray->capacity;

        if (psh_likely(previous_capacity != 0)) {
            new_capacity = previous_capacity * growth_factor;
            new_buf      = memory_realloc<T>(arena, darray->buf, previous_capacity, new_capacity);
        } else {
            new_capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY;
            new_buf      = memory_alloc<T>(arena, new_capacity);
        }

        Status status = (new_buf != nullptr);
        if (psh_likely(status)) {
            darray->buf      = new_buf;
            darray->capacity = new_capacity;
        }

        return status;
    }

    /// Adjust the capacity of the dynamic array.
    ///
    /// Note: If T is a struct with a pointer to itself, the pointer address will be invalidated
    ///       by this procedure. DO NOT use this array structure with types having this property.
    template <typename T>
    psh_proc Status dynamic_array_reserve(DynamicArray<T>* darray, usize new_capacity) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(darray));
        psh_validate_usage(psh_assert_msg(darray->capacity < new_capacity, "DynamicArray doesn't shrink."));

        T*     new_buf          = nullptr;
        Arena* arena            = darray->arena;
        usize  current_capacity = darray->capacity;
        if (current_capacity == 0) {
            new_buf = memory_alloc<T>(arena, new_capacity);
        } else {
            new_buf = memory_realloc<T>(arena, darray->buf, current_capacity, new_capacity);
        }

        Status status = (new_buf != nullptr);
        if (psh_likely(status)) {
            darray->buf      = new_buf;
            darray->capacity = new_capacity;
        }

        return status;
    }

    /// Inserts a new element to the end of the dynamic array.
    template <typename T>
    psh_proc Status dynamic_array_push(DynamicArray<T>* darray, T new_element) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(darray));

        usize previous_count = darray->count;

        Status status = STATUS_OK;
        if (darray->capacity == previous_count) {
            status = dynamic_array_grow(darray);
        }

        if (psh_likely(status)) {
            darray->buf[previous_count] = new_element;
            darray->count               = previous_count + 1;
        }

        return status;
    }

    /// Insert a collection of new elements to the end of the dynamic array.
    template <typename T>
    psh_proc Status dynamic_array_push_many(DynamicArray<T>* darray, FatPtr<T const> new_elements) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(darray));

        usize previous_count = darray->count;

        Status status = STATUS_OK;
        if (darray->capacity < new_elements.count + previous_count) {
            status = dynamic_array_reserve(darray, previous_count + new_elements.count);
        }

        if (psh_likely(status)) {
            memory_copy(
                reinterpret_cast<u8*>(darray->buf + previous_count),
                reinterpret_cast<u8 const*>(new_elements.buf),
                sizeof(T) * new_elements.count);
            darray->count = previous_count + new_elements.count;
        }

        return status;
    }

    /// Try to pop the last element of the dynamic array.
    template <typename T>
    psh_proc Status dynamic_array_pop(DynamicArray<T>* darray) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(darray));

        usize previous_count = darray->count;

        Status status = (previous_count > 0);
        if (psh_likely(status)) {
            darray->count = previous_count - 1u;
        }

        return status;
    }

    /// Clear the dynamic array data, resetting its size.
    template <typename T>
    psh_proc psh_inline void dynamic_array_clear(DynamicArray<T>* darray) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(darray));
        darray->count = 0;
    }

    // -------------------------------------------------------------------------------------------------
    // Memory manipulation procedures common to all containers.
    // -------------------------------------------------------------------------------------------------

    psh_proc void raw_unordered_remove(FatPtr<u8> fptr, u8* element_ptr, usize element_size) psh_no_except;
    psh_proc void raw_ordered_remove(FatPtr<u8> fptr, u8* element_ptr, usize element_size) psh_no_except;

    ///
    /// Try to remove a buffer element at a given index.
    ///
    /// This won't preserve the current ordering of the buffer.
    ///

    template <typename T>
    psh_proc psh_inline void fat_ptr_unordered_remove(FatPtr<T>* fptr, usize idx) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(fptr));
        psh_validate_usage(psh_assert_bounds_check(idx, fptr->count));

        T*    buf   = fptr->buf;
        usize count = fptr->count;

        --fptr->count;
        raw_unordered_remove(
            FatPtr{reinterpret_cast<u8*>(buf), count * sizeof(T)},
            reinterpret_cast<u8*>(buf + idx),
            sizeof(T));
    }

    template <typename T>
    psh_proc psh_inline void dynamic_array_unordered_remove(DynamicArray<T>* darray, usize idx) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(darray));
        psh_validate_usage(psh_assert_bounds_check(idx, darray->count));

        T*    buf   = darray->buf;
        usize count = darray->count;

        --darray->count;
        raw_unordered_remove(
            FatPtr{reinterpret_cast<u8*>(buf), count * sizeof(T)},
            reinterpret_cast<u8*>(buf + idx),
            sizeof(T));
    }

    ///
    /// Try to remove a buffer element at a given index.
    ///
    /// This will move all of the buffer contents above the removed element index down one.
    ///

    template <typename T>
    psh_proc void fat_ptr_ordered_remove(FatPtr<T>* fptr, usize idx) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(fptr));
        psh_validate_usage(psh_assert_bounds_check(idx, fptr->count));

        T*    buf   = fptr->buf;
        usize count = fptr->count;

        --fptr->count;
        raw_ordered_remove(
            FatPtr{reinterpret_cast<u8*>(buf), count * sizeof(T)},
            reinterpret_cast<u8*>(buf + idx),
            sizeof(T));
    }

    template <typename T>
    psh_proc void dynamic_array_ordered_remove(DynamicArray<T>* darray, usize idx) psh_no_except {
        psh_paranoid_validate_usage(psh_assert_not_null(darray));
        psh_validate_usage(psh_assert_bounds_check(idx, darray->count));

        T*    buf   = darray->buf;
        usize count = darray->count;

        --darray->count;
        raw_ordered_remove(
            FatPtr{reinterpret_cast<u8*>(buf), count * sizeof(T)},
            reinterpret_cast<u8*>(buf + idx),
            sizeof(T));
    }
}  // namespace psh
