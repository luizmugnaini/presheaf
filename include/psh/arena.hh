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

#include <cstring>
#include <psh/assert.hh>
#include <psh/core.hh>
#include <psh/log.hh>
#include <psh/math.hh>
#include <psh/memory_utils.hh>

namespace psh {
    // Forward declaration.
    struct Arena;

    /// Scratch arena.
    ///
    /// This allocator is used to save the state of the parent arena at creation time and
    /// restore the parent arena offset state at destruction time.
    ///
    /// You can nest many scratch arenas throughout different lifetimes by decoupling the
    /// current scratch arena into a new one.
    ///
    /// # Intended usage pattern
    ///
    /// ```cpp
    /// f32 do_temp_work_and_restore_arena(ScratchArena&& s) {
    ///    DynArray<f32> arr{s.arena, 3};
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
    /// i32* alloc_in_scratch_lifetime(ScratchArena& s, usize count) {
    ///     return s.arena.alloc<i32>(count);
    /// }
    ///
    /// int main() {
    ///     Arena a{...};
    ///
    ///     u8* x = a.alloc<u8>(1024);  // `a` has offset 1024.
    ///     {
    ///         ScratchArena s = a.make_scratch();  // Record the offset 1024.
    ///
    ///         i32 y = s.arena.alloc<u8>(32);  // `a` has an offset of 1024 + 4 * 32 = 1152.
    ///
    ///         // Record the offset 1152 into a new scratch arena, pass it a function, which
    ///         // will possibly allocate. When the function returns, `a` will go back to the
    ///         // 1152 offset.
    ///         f32 result = do_temp_work_and_restore_arena(s.decouple());
    ///
    ///         // Pass `s` to a function that will allocate 100 * 4 = 400 bits, bumping the
    ///         // offset of `a` to 1152 + 400 = 1552.
    ///         i32* val = alloc_in_scratch_lifetime(s, 100);
    ///     }
    ///     // `a` goes back to having an offset of 1024.
    ///
    ///     return 0;
    /// }
    /// ```
    ///
    /// # Bad usage
    ///
    /// You should avoid the following usage pattern at all costs
    /// ```cpp
    /// f32* bad_alloc(ScratchArena const& s, usize count) {
    ///     return s.arena.alloc<f32>(count);
    /// }
    ///
    /// int main() {
    ///     Arena a{...}; // `a` has 0 offset.
    ///     {
    ///         ScratchArena s = a.make_scratch(); // Record the 0 offset.
    ///
    ///         u32 foo = s.arena.alloc<u32>(10); // `a` has offset 4 * 10 = 40.
    ///
    ///         // - Records the offset `40`.
    ///         // - Allocates 30 * 4 = 120 bits internally.
    ///         // - Restore the arena offset to `40` when returning.
    ///         // - The resulting `bad_ptr` can be overwritten by any subsequent allocation.
    ///         f32* bad_ptr = bad_alloc(s.decouple(), 30);
    ///
    ///         // Will overwrite the memory region "pertaining" to `bad_ptr`
    ///         i64* over_ptr s.arena.alloc<i64>(50);
    ///     }
    ///     // `a` is restored to offset 0.
    ///     return 0;
    /// }
    /// ```
    /// If you passed `s` directly to `bad_alloc` you wouldn't have this problem because
    /// `bad_alloc` wouldn't restore the arena offset when returning and the arena would have an
    /// offset of 40 + 120 = 160. In order to mitigate this usage pattern, you should never have
    /// a function signature accepting `ScratchArena const&` as a parameter, since the callee
    /// may pass the scratch as an rvalue. In order to truly solve the above code problem, you
    /// should substitute `ScratchArena const&` of `bad_alloc` by `ScratchArena&` since this
    /// prohibits passing the scratch arena by rvalue.
    ///
    /// In general, your options will then be to pass as `ScratchArena&` or `ScratchArena&&`.
    struct ScratchArena {
        Arena* arena;
        usize  saved_offset = 0;

        ScratchArena(Arena* arena) noexcept;
        ~ScratchArena() noexcept;

        /// Create a new scratch arena with the current state of the parent.
        ScratchArena decouple() const noexcept;
    };

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
        u8*   buf    = nullptr;  ///< Non-owned memory block managed by the arena allocator.
        usize size   = 0;        ///< The capacity, in bytes, of the memory block.
        usize offset = 0;        ///< The current offset to the free-space in the memory block.

        // -----------------------------------------------------------------------------
        // - Allocation methods -
        // -----------------------------------------------------------------------------

        u8* alloc_align(usize size_bytes, u32 alignment) noexcept;
        u8* zero_alloc_align(usize size_bytes, u32 alignment) noexcept;
        u8* realloc_align(u8* block, usize current_size_bytes, usize new_size_bytes, u32 alignment) noexcept;

        template <typename T>
        T* alloc(usize count) noexcept;

        template <typename T>
        T* zero_alloc(usize count) noexcept;

        template <typename T>
        T* realloc(T* block, usize current_count, usize new_count) noexcept;

        // -----------------------------------------------------------------------------
        // - Temporary memory management -
        // -----------------------------------------------------------------------------

        /// Reset the offset of the allocator.
        void clear() noexcept;

        /// Create a new scratch arena with the current offset state.
        ScratchArena make_scratch() noexcept;
    };

    // -----------------------------------------------------------------------------
    // - Implementation of the arena allocation methods -
    // -----------------------------------------------------------------------------

    template <typename T>
    T* Arena::alloc(usize count) noexcept {
        return reinterpret_cast<T*>(this->alloc_align(sizeof(T) * count, alignof(T)));
    }

    template <typename T>
    T* Arena::zero_alloc(usize count) noexcept {
        return reinterpret_cast<T*>(this->zero_alloc_align(sizeof(T) * count, alignof(T)));
    }

    template <typename T>
    T* Arena::realloc(T* block, usize current_count, usize new_count) noexcept {
        return reinterpret_cast<T*>(this->realloc_align(
            reinterpret_cast<u8*>(block),
            sizeof(T) * current_count,
            sizeof(T) * new_count,
            alignof(T)));
    }
}  // namespace psh
