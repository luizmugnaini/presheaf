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

#include <psh/core.hpp>
#include <psh/fat_ptr.hpp>

namespace psh {
    // -------------------------------------------------------------------------------------------------
    // CPU architecture.
    // -------------------------------------------------------------------------------------------------

    psh_api psh_inline bool arch_is_little_endian() psh_no_except {
        i32 integer = 1;
        return static_cast<bool>(*(reinterpret_cast<u8*>(&integer)));
    }

    psh_api psh_inline bool arch_is_big_endian() psh_no_except {
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
    /// The memory allocated is always initialized to zero.
    ///
    /// Directly calls the respective system call for allocating memory. Should be
    /// used for large allocations, if you want small allocations, malloc is the way to go.
    psh_api u8* memory_virtual_alloc(usize size_bytes) psh_no_except;

    /// Release and decommit all memory.
    psh_api void memory_virtual_free(u8* memory, usize size_bytes) psh_no_except;

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
    psh_api usize padding_with_header(
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
    psh_api usize align_forward(uptr ptr, usize alignment) psh_no_except;

    // -------------------------------------------------------------------------------------------------
    // Arena memory allocator.
    // -------------------------------------------------------------------------------------------------

    // Forward declaration.
    struct Arena;

    /// Manually managed checkpoint for arenas.
    ///
    /// You can create a checkpoint with Arena::make_checkpoint and restore the arena to
    /// a given checkpoint via Arena::restore_checkpoint.
    struct psh_api ArenaCheckpoint {
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
    struct psh_api Arena {
        u8*   buf;
        usize capacity = 0;
        usize offset   = 0;
    };

    psh_inline Arena make_arena(u8* buf, usize capacity) psh_no_except {
        return Arena{
            .buf      = buf,
            .capacity = (buf != nullptr) ? capacity : 0,
            .offset   = 0,
        };
    }

    /// Initialize the arena with a given memory buffer and a capacity.
    psh_inline void arena_init(Arena* arena, u8* buf, usize capacity) psh_no_except {
        psh_validate_usage(psh_assert_not_null(arena));
        arena->buf      = buf;
        arena->capacity = (buf != nullptr) ? capacity : 0;
    }

    /// Reset the offset of the allocator.
    psh_inline void arena_clear(Arena* arena) psh_no_except {
        psh_validate_usage(psh_assert_not_null(arena));
        arena->offset = 0;
    }

    /// Make an arena that owns its memory.
    ///
    /// Since the arena is not aware of the ownership, this function call has to be paired
    /// with free_owned_arena.
    psh_inline Arena make_owned_arena(usize capacity) psh_no_except {
        u8* buf = memory_virtual_alloc(capacity);
        return Arena{
            .buf      = buf,
            .capacity = (buf != nullptr) ? capacity : 0,
            .offset   = 0,
        };
    }

    /// Free the memory of an arena that owns its memory.
    ///
    /// This function should only be called for arenas that where created by make_owned_arena.
    psh_inline void free_owned_arena(Arena* arena) psh_no_except {
        psh_validate_usage(psh_assert_not_null(arena));

        usize capacity  = arena->capacity;
        arena->capacity = 0;
        memory_virtual_free(arena->buf, capacity);
    }

    /// Create a restorable checkpoint for the arena. This is a more flexible alternative to the
    /// ScratchArena construct since you can manually restore the arena, not relying on destructors.
    psh_inline ArenaCheckpoint make_arena_checkpoint(Arena* arena) psh_no_except {
        psh_validate_usage(psh_assert_not_null(arena));
        return ArenaCheckpoint{
            .arena        = arena,
            .saved_offset = arena->offset,
        };
    }

    /// Restore the arena state to a given checkpoint.
    psh_inline void arena_checkpoint_restore(ArenaCheckpoint checkpoint) psh_no_except {
        psh_validate_usage({
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
    struct psh_api ScratchArena {
        Arena* arena = nullptr;
        usize  saved_offset;

        psh_inline ScratchArena(Arena* arena_) psh_no_except {
            psh_validate_usage(psh_assert_not_null(arena_));
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
    struct psh_api StackHeader {
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
    struct psh_api Stack {
        u8*   buf;
        usize capacity        = 0;
        usize offset          = 0;
        usize previous_offset = 0;

        psh_inline void init(u8* buf_, usize capacity_) psh_no_except {
            psh_validate_usage(psh_assert_msg(this->capacity == 0, "Stack already initialized."));
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
    struct psh_api MemoryManager {
        usize allocation_count = 0;
        Stack allocator        = {};

        /// Allocate memory and initialize the underlying memory allocator.
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
    // Run-time known fixed length array.
    // -------------------------------------------------------------------------------------------------

    /// Array with run-time known constant capacity.
    ///
    /// The array lifetime is bound to the lifetime of the arena passed at initialization, being
    /// responsible to allocate the memory referenced by the array.
    template <typename T>
    struct psh_api Array {
        T*    buf;
        usize count = 0;

        psh_impl_generate_container_boilerplate(T, this->buf, this->count)
    };

    template <typename T>
    psh_inline Array<T> make_array(Arena* arena, usize count) psh_no_except {
        T* buf = memory_alloc<T>(arena, count);
        return Array<T>{
            .buf   = buf,
            .count = (buf != nullptr) ? count : 0,
        };
    }

    template <typename T>
    psh_inline void array_init(Array<T>* array, Arena* arena, usize count) psh_no_except {
        psh_validate_usage({
            psh_assert_not_null(array);
            psh_assert_msg(array->count == 0, "Tried to re-initialize an Array.");
        });

        T* buf       = memory_alloc<T>(arena, count);
        array->buf   = buf;
        array->count = (buf != nullptr) ? count : 0;
    }

    // -------------------------------------------------------------------------------------------------
    // Dynamically sized array.
    // -------------------------------------------------------------------------------------------------

    static constexpr usize DYNARRAY_DEFAULT_INITIAL_CAPACITY      = 4;
    static constexpr usize DYNARRAY_RESIZE_CAPACITY_GROWTH_FACTOR = 2;

    /// Run-time variable length array.
    ///
    /// A dynamic array has its lifetime bound to its associated arena.
    template <typename T>
    struct psh_api DynArray {
        T*     buf;
        Arena* arena;
        usize  capacity = 0;
        usize  count    = 0;

        psh_impl_generate_container_boilerplate(T, this->buf, this->count)
    };

    template <typename T>
    psh_api psh_inline DynArray<T> make_dynarray(
        Arena* arena,
        usize  capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY) psh_no_except {
        T* buf = memory_alloc<T>(arena, capacity);
        return DynArray<T>{
            .buf      = buf,
            .arena    = arena,
            .capacity = (buf != nullptr) ? capacity : 0,
            .count    = 0,
        };
    }

    /// Initialize the dynamic array with a given capacity.
    template <typename T>
    psh_api psh_inline void dynarray_init(
        DynArray<T>* darray,
        Arena*       arena,
        usize        capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY) psh_no_except {
        psh_validate_usage({
            psh_assert_not_null(darray);
            psh_assert_msg(darray->count == 0, "DynArray already initialized");
        });

        darray->buf      = memory_alloc<T>(arena, capacity);
        darray->arena    = arena;
        darray->capacity = (darray->buf != nullptr) ? capacity : 0;
    }

    /// Grow the capacity of the dynamic array underlying buffer.
    template <typename T>
    psh_api Status dynarray_grow(
        DynArray<T>* darray,
        u32          growth_factor = DYNARRAY_RESIZE_CAPACITY_GROWTH_FACTOR) psh_no_except {
        psh_validate_usage(psh_assert_not_null(darray));

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

    /// @NOTE: If T is a struct with a pointer to itself, the pointer address will be invalidated
    ///        by this procedure. DO NOT use this array structure with types having this property.
    template <typename T>
    psh_api Status dynarray_reserve(DynArray<T>* darray, usize new_capacity) psh_no_except {
        psh_validate_usage({
            psh_assert_not_null(darray);
            psh_assert_msg(darray->capacity < new_capacity, "DynArray doesn't shrink.");
        });

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
    psh_api Status dynarray_push(DynArray<T>* darray, T new_element) psh_no_except {
        psh_validate_usage(psh_assert_not_null(darray));

        usize previous_count = darray->count;

        Status status = STATUS_OK;
        if (darray->capacity == previous_count) {
            status = dynarray_grow(darray);
        }

        if (psh_likely(status)) {
            darray->buf[previous_count] = new_element;
            darray->count               = previous_count + 1;
        }

        return status;
    }

    /// Insert a collection of new elements to the end of the dynamic array.
    template <typename T>
    psh_api Status dynarray_push_many(DynArray<T>* darray, FatPtr<T const> new_elements) psh_no_except {
        psh_validate_usage(psh_assert_not_null(darray));

        usize previous_count = darray->count;

        Status status = STATUS_OK;
        if (darray->capacity < new_elements.count + previous_count) {
            status = dynarray_reserve(darray, previous_count + new_elements.count);
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
    psh_api Status dynarray_pop(DynArray<T>* darray) psh_no_except {
        psh_validate_usage(psh_assert_not_null(darray));

        usize previous_count = darray->count;

        Status status = (previous_count > 0);
        if (psh_likely(status)) {
            darray->count = previous_count - 1u;
        }

        return status;
    }

    /// Clear the dynamic array data, resetting its size.
    template <typename T>
    psh_api psh_inline void dynarray_clear(DynArray<T>* darray) psh_no_except {
        psh_validate_usage(psh_assert_not_null(darray));
        darray->count = 0;
    }

    // -------------------------------------------------------------------------------------------------
    // Removing elements from a container.
    // -------------------------------------------------------------------------------------------------

    psh_api void raw_unordered_remove(FatPtr<u8> fptr, u8* element_ptr, usize element_size) psh_no_except;
    psh_api void raw_ordered_remove(FatPtr<u8> fptr, u8* element_ptr, usize element_size) psh_no_except;

    /// Try to remove a buffer element at a given index.
    ///
    /// This won't preserve the current ordering of the buffer.
    template <typename T>
    psh_api void fat_ptr_unordered_remove(FatPtr<T>* fptr, usize idx) psh_no_except {
        psh_validate_usage({
            psh_assert_not_null(fptr);
            psh_assert_bounds_check(idx, fptr->count);
        });

        T*    buf   = fptr->buf;
        usize count = fptr->count;

        --fptr->count;
        raw_unordered_remove(
            FatPtr{reinterpret_cast<u8*>(buf), count * sizeof(T)},
            reinterpret_cast<u8*>(buf + idx),
            sizeof(T));
    }
    template <typename T>
    psh_api void dynarray_unordered_remove(DynArray<T>* darray, usize idx) psh_no_except {
        psh_validate_usage({
            psh_assert_not_null(darray);
            psh_assert_bounds_check(idx, darray->count);
        });

        T*    buf   = darray->buf;
        usize count = darray->count;

        --darray->count;
        raw_unordered_remove(
            FatPtr{reinterpret_cast<u8*>(buf), count * sizeof(T)},
            reinterpret_cast<u8*>(buf + idx),
            sizeof(T));
    }

    /// Try to remove a buffer element at a given index.
    ///
    /// This will move all of the buffer contents above the removed element index down one.
    template <typename T>
    psh_api void fat_ptr_ordered_remove(FatPtr<T>* fptr, usize idx) psh_no_except {
        psh_validate_usage({
            psh_assert_not_null(fptr);
            psh_assert_bounds_check(idx, fptr->count);
        });

        T*    buf   = fptr->buf;
        usize count = fptr->count;

        --fptr->count;
        raw_ordered_remove(
            FatPtr{reinterpret_cast<u8*>(buf), count * sizeof(T)},
            reinterpret_cast<u8*>(buf + idx),
            sizeof(T));
    }
    template <typename T>
    psh_api void dynarray_ordered_remove(DynArray<T>* darray, usize idx) psh_no_except {
        psh_validate_usage({
            psh_assert_not_null(darray);
            psh_assert_bounds_check(idx, darray->count);
        });

        T*    buf   = darray->buf;
        usize count = darray->count;

        --darray->count;
        raw_ordered_remove(
            FatPtr{reinterpret_cast<u8*>(buf), count * sizeof(T)},
            reinterpret_cast<u8*>(buf + idx),
            sizeof(T));
    }

    // -------------------------------------------------------------------------------------------------
    // Memory manipulation.
    // -------------------------------------------------------------------------------------------------

    /// Query the current size in bytes of a given container.
    template <typename Container, typename T = Container::ValueType>
    psh_api usize size_bytes(Container const* c) psh_no_except {
        psh_validate_usage({
            psh_static_assert_valid_const_container_type(Container, c);
            psh_assert_not_null(c);
        });
        return sizeof(T) * c->count;
    }

    /// Simple wrapper around memset that automatically deals with null values.
    ///
    /// Does nothing if ptr is a null pointer.
    psh_api void memory_set(u8* memory, usize size_bytes, i32 fill) psh_no_except;

    /// Zero-out all of the members of a given structure.
    template <typename T>
    psh_api psh_inline void zero_struct(T* s) psh_no_except {
        memory_set(reinterpret_cast<u8*>(s), sizeof(T), 0);
    }

    /// Simple wrapper around memcpy.
    ///
    /// This function will assert that the blocks of memory don't overlap, avoiding undefined
    /// behaviour introduced by memcpy in this case.
    psh_api void memory_copy(u8* psh_no_alias dst, u8 const* psh_no_alias src, usize size_bytes) psh_no_except;

    /// Simple wrapper around memmove.
    ///
    /// Does nothing if either dst or src are null pointers.
    psh_api void memory_move(u8* psh_no_alias dst, u8 const* psh_no_alias src, usize size_bytes) psh_no_except;

    /// Allocate a new block of memory with a given alignment.
    psh_api u8* memory_alloc_align(Arena* arena, usize size_bytes, u32 alignment) psh_no_except;
    psh_api u8* memory_alloc_align(Stack* stack, usize size_bytes, u32 alignment) psh_no_except;

    /// Allocates a new block of memory capable of holding a certain count of elements of a
    /// given type.
    template <typename T>
    psh_api psh_inline T* memory_alloc(Arena* arena, usize count) psh_no_except {
        return reinterpret_cast<T*>(memory_alloc_align(arena, sizeof(T) * count, alignof(T)));
    }
    template <typename T>
    psh_api psh_inline T* memory_alloc(Stack* stack, usize count) psh_no_except {
        return reinterpret_cast<T*>(memory_alloc_align(stack, sizeof(T) * count, alignof(T)));
    }
    template <typename T>
    psh_api psh_inline T* memory_alloc(MemoryManager* memory_manager, usize count) psh_no_except {
        if (memory_manager == nullptr) {
            return nullptr;
        }

        T* const new_block = memory_alloc<T>(&memory_manager->allocator, count);
        memory_manager->allocation_count += static_cast<usize>(new_block != nullptr);
        return new_block;
    }

    /// Reallocate an existing block of memory with a given alignment.
    psh_api u8* memory_realloc_align(
        Arena* arena,
        u8*    block,
        usize  current_size_bytes,
        usize  new_size_bytes,
        u32    alignment) psh_no_except;
    psh_api u8* memory_realloc_align(
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
    psh_api psh_inline T* memory_realloc(Arena* arena, T* block, usize current_count, usize new_count) psh_no_except {
        return reinterpret_cast<T*>(memory_realloc_align(
            arena,
            reinterpret_cast<u8*>(block),
            sizeof(T) * current_count,
            sizeof(T) * new_count,
            alignof(T)));
    }
    template <typename T>
    psh_api psh_inline T* memory_realloc(Stack* stack, T* block, usize new_count) psh_no_except {
        return reinterpret_cast<T*>(memory_realloc_align(stack, block, sizeof(T) * new_count));
    }
    template <typename T>
    psh_api psh_inline T* memory_realloc(MemoryManager* memory_manager, T* block, usize new_count) psh_no_except {
        if (memory_manager == nullptr) {
            return nullptr;
        }

        T* const new_block = memory_realloc<T>(&memory_manager->allocator, block, new_count);
        memory_manager->allocation_count += static_cast<usize>(new_block != block);
        return new_block;
    }
}  // namespace psh
