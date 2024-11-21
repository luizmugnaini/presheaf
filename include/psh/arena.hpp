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
/// Description: Arena memory allocator, also known as a linear allocator.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#pragma once

#include <psh/core.hpp>
#include <psh/memory.hpp>

namespace psh {
    // Forward declaration.
    struct Arena;

    /// Scratch arena, an automatic checkpoint manager for arena offsets.
    ///
    /// This allocator is used to save the state of the parent arena at creation time and
    /// restore the parent arena offset state at destruction time.
    ///
    /// You can nest many scratch arenas throughout different lifetimes by creating scratch arenas
    /// at distinct scopes.
    ///
    /// Scratch arenas shouldn't be passed to other functions, they should be used in a single scope.
    ///
    /// # Intended usage pattern
    ///
    /// ```cpp
    /// f32 do_temp_work_and_restore_arena(Arena& arena) {
    ///    ScratchArena s = arena.make_scratch(); // The arena will be restored at the end of the function.
    ///
    ///    DynArray<f32> arr{arena};
    ///    arr.push(4.0f);
    ///    arr.push(5.5f);
    ///    arr.push(20.5f);
    ///
    ///    f32 sum = 0;
    ///    for (f32 x : arr) sum += x;
    ///
    ///    return sum;
    /// }
    ///
    /// i32* alloc_in_scratch_lifetime(Arena& arena, usize count) {
    ///     return arena.alloc<i32>(count);
    /// }
    ///
    /// int main() {
    ///     Arena arena{...};
    ///
    ///     u8* x = arena.alloc<u8>(1024);  // `arena` has offset 1024.
    ///     {
    ///         ScratchArena sarena = arena.make_scratch();  // Record the offset 1024.
    ///
    ///         i32 y = arena.alloc<u8>(32);  // `arena` has an offset of 1024 + 4 * 32 = 1152.
    ///
    ///         // Record the offset 1152 into a new scratch arena, pass it to a function, which
    ///         // will possibly allocate. When the function returns, `arena` will go back to the
    ///         // 1152 offset (see `do_temp_work_and_restore_arena`).
    ///         f32 result = do_temp_work_and_restore_arena(arena);
    ///
    ///         // Pass `sarena` to a function that will allocate 100 * 4 = 400 bits, bumping the
    ///         // offset of `arena` to 1152 + 400 = 1552.
    ///         i32* val = alloc_in_scratch_lifetime(arena, 100);
    ///     }
    ///     // `arena` goes back to having an offset of 1024.
    ///
    ///     return 0;
    /// }
    /// ```
    struct psh_api ScratchArena {
        Arena* arena;
        usize  saved_offset;

        psh_inline ScratchArena(Arena* parent) noexcept;
        psh_inline ~ScratchArena() noexcept;

        // @NOTE: Required when compiling as a DLL since the compiler will require all standard
        //        member functions to be defined.
        ScratchArena& operator=(ScratchArena&) = delete;
    };

    /// Manually managed checkpoint for arenas.
    ///
    /// You can create a checkpoint with `Arena::make_checkpoint` and restore the arena to
    /// a given checkpoint via `Arena::restore_state`.
    struct psh_api ArenaCheckpoint {
        Arena* arena;
        usize  saved_offset;
    };

    /// Arena allocator
    ///
    /// The arena allocator is great for the management of temporary allocation of memory, since an
    /// allocation takes nothing more than incrementing an offset.
    ///
    /// The arena does not own memory, thus it is not responsible for the freeing of it.
    struct psh_api Arena {
        /// Not-owned block of memory.
        u8*   buf;
        /// Capacity in bytes of the arena block of memory.
        usize capacity = 0;
        /// The current offset to the free-space in the memory block.
        usize offset   = 0;

        // -----------------------------------------------------------------------------
        // Allocation methods.
        //
        // @NOTE: All allocation procedures will zero-out the whole allocated block.
        // -----------------------------------------------------------------------------

        u8* alloc_align(usize size_bytes, u32 alignment) noexcept;

        u8* realloc_align(u8* block, usize current_size_bytes, usize new_size_bytes, u32 alignment) noexcept;

        template <typename T>
        T* alloc(usize count) noexcept {
            return reinterpret_cast<T*>(this->alloc_align(sizeof(T) * count, alignof(T)));
        }

        template <typename T>
        T* realloc(T* block, usize current_count, usize new_count) noexcept {
            return reinterpret_cast<T*>(this->realloc_align(
                reinterpret_cast<u8*>(block),
                sizeof(T) * current_count,
                sizeof(T) * new_count,
                alignof(T)));
        }

        // -----------------------------------------------------------------------------
        // Temporary memory management.
        // -----------------------------------------------------------------------------

        /// Reset the offset of the allocator.
        psh_inline void clear() noexcept {
            this->offset = 0;
        }

        /// Create a new scratch arena with the current offset state.
        psh_inline ScratchArena make_scratch() noexcept {
            return ScratchArena{this};
        }

        /// Create a restorable checkpoint for the arena. This is a more flexible alternative to the
        /// `ScratchArena` construct since you can manually restore the arena, not relying in destructors.
        psh_inline ArenaCheckpoint make_checkpoint() noexcept {
            return ArenaCheckpoint{.arena = this, .saved_offset = this->offset};
        }

        /// Restore the arena state to a given checkpoint.
        psh_inline void restore_state(ArenaCheckpoint& checkpoint) noexcept {
            psh_assert_msg(checkpoint.arena == this, "Checkpoint originates from a distinct arena.");
            psh_assert_fmt(
                checkpoint.saved_offset <= this->offset,
                "Invalid checkpoint. Cannot restore the arena to an offset (%zu) bigger than the current (%zu).",
                checkpoint.saved_offset,
                this->offset);

            checkpoint.arena = nullptr;  // Invalidate the checkpoint for further uses.
            this->offset     = checkpoint.saved_offset;
        }
    };

    /// Make an arena that owns its memory.
    ///
    /// Since the arena is not aware of the ownership, this function call has to be paired
    /// with `free_owned_arena`.
    psh_inline Arena make_owned_arena(usize capacity) noexcept {
        FatPtr<u8> memory = psh::memory_virtual_alloc(capacity);
        psh_assert_msg(memory.count != 0, "Failed to allocate memory.");

        return Arena{.buf = memory.buf, .capacity = memory.count};
    }

    /// Free the memory of an arena that owns its memory.
    ///
    /// This function should only be called for arenas that where created by `make_owned_arena`.
    psh_inline void free_owned_arena(Arena& arena) noexcept {
        psh::memory_virtual_free({arena.buf, arena.capacity});
        arena.capacity = 0;
    }

    // -----------------------------------------------------------------------------
    // Implementation of the scratch arena methods.
    // -----------------------------------------------------------------------------

    ScratchArena::ScratchArena(Arena* parent) noexcept {
        if (psh_likely(parent != nullptr)) {
            this->arena        = parent;
            this->saved_offset = parent->offset;
        }
    }

    ScratchArena::~ScratchArena() noexcept {
        if (psh_likely(this->arena != nullptr)) {
            this->arena->offset = this->saved_offset;
        }
    }

}  // namespace psh
