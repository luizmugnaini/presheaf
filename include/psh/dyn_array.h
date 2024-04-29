/// Dynamic array.
///
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#pragma once

#include <psh/arena.h>
#include <psh/mem_utils.h>
#include <psh/option.h>
#include <psh/types.h>

#include <initializer_list>

namespace psh {
    /// Resizing factor for the `DynArray::grow` method.
    static constexpr usize DYNARRAY_RESIZE_CAPACITY_FACTOR   = 2;
    /// The initial capacity given to a `DynArray` if none is specified.
    static constexpr usize DYNARRAY_DEFAULT_INITIAL_CAPACITY = 4;

    /// Dynamic array managed by an arena allocator.
    ///
    /// The dynamic array's buffer shouldn't be destroyed by the array itself, since it *never* owns
    /// the memory.
    template <typename T>
        requires IsObject<T>
    struct DynArray {
        Arena* arena_    = nullptr;
        usize  size_     = 0;
        usize  capacity_ = 0;
        T*     buf_      = nullptr;

        explicit constexpr DynArray() noexcept = default;

        /// Initialize the dynamic array with a given capacity.
        void init(Arena* _arena, usize _capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY) noexcept {
            arena_    = _arena;
            capacity_ = _capacity;

            if (capacity_ != 0) {
                psh_assert_msg(
                    arena_ != nullptr,
                    "DynArray::init called with non-zero capacity but an empty arena");

                buf_ = arena_->zero_alloc<T>(capacity_);
                psh_assert_msg(
                    buf_ != nullptr,
                    "DynArray::init unable to acquire enough bytes of memory");
            }
        }

        /// Construct a dynamic array with a given capacity.
        explicit DynArray(
            Arena* _arena,
            usize  _capacity = DYNARRAY_DEFAULT_INITIAL_CAPACITY) noexcept {
            this->init(_arena, _capacity);
        }

        /// Initialize the dynamic array with the contents of an initializer list, and optionally
        /// reserve a given capacity.
        void init(
            std::initializer_list<T> const& list,
            Arena*                          _arena,
            Option<usize> const&            _capacity = {}) noexcept {
            arena_    = _arena;
            size_     = list.size();
            capacity_ = _capacity.val_or(DYNARRAY_RESIZE_CAPACITY_FACTOR * size_);

            psh_assert_msg(
                size_ <= capacity_,
                "DynArray::init called with inconsistent data: capacity less than the size");

            if (capacity_ != 0) {
                psh_assert_msg(
                    arena_ != nullptr,
                    "DynArray::init called with non-zero capacity but an empty arena");

                buf_ = arena_->alloc<T>(capacity_);
                psh_assert_msg(buf_ != nullptr, "DynArray::init unable to allocate enough memory");
            }

            memory_copy(
                reinterpret_cast<u8*>(buf_),
                reinterpret_cast<u8 const*>(list.begin()),
                sizeof(T) * list.size());
        }

        /// Construct a dynamic array with the contents of an initializer list, and optionally
        /// reserve a given capacity.
        explicit DynArray(
            std::initializer_list<T> list,
            Arena*                   _arena,
            Option<usize>            _capacity = {}) noexcept {
            this->init(list, _arena, _capacity);
        }

        /// Initialize the dynamic array with the contents of a fat pointer, and optionally reserve
        /// a given capacity.
        void
        init(FatPtr<T> const& fptr, Arena* _arena, Option<usize> const& _capacity = {}) noexcept {
            arena_    = _arena;
            size_     = fptr.size();
            capacity_ = _capacity.val_or(DYNARRAY_RESIZE_CAPACITY_FACTOR * size_);

            psh_assert_msg(
                size_ <= capacity_,
                "DynArray::init called with inconsistent data: capacity less than the size");

            if (capacity_ != 0) {
                psh_assert_msg(
                    arena_ != nullptr,
                    "DynArray::init called with non-zero capacity but an empty arena");

                buf_ = arena_->alloc<T>(capacity_);
                psh_assert_msg(buf_ != nullptr, "DynArray::init unable to allocate enough memory");
            }

            memory_copy(
                reinterpret_cast<u8*>(buf_),
                reinterpret_cast<u8 const*>(fptr.begin()),
                fptr.size_bytes());
        }

        /// Construct a dynamic array with the contents of a fat pointer, and optionally reserve a
        /// given capacity.
        explicit DynArray(FatPtr<T> fptr, Arena* _arena, Option<usize> _capacity = {}) noexcept {
            this->init(fptr, _arena, _capacity);
        }

        [[nodiscard]] T* buf() noexcept {
            return buf_;
        }

        [[nodiscard]] T const* const_buf() const noexcept {
            return reinterpret_cast<T const*>(buf_);
        }

        [[nodiscard]] usize size() const noexcept {
            return size_;
        }

        [[nodiscard]] usize capacity() const noexcept {
            return capacity_;
        }

        [[nodiscard]] bool is_empty() const noexcept {
            return (size_ == 0);
        }

        [[nodiscard]] usize size_bytes() noexcept {
            return size_ * sizeof(T);
        }

        [[nodiscard]] usize capacity_bytes() const noexcept {
            return capacity_ * sizeof(T);
        }

        [[nodiscard]] FatPtr<T> as_fat_ptr() noexcept {
            return FatPtr{buf_, size_};
        }

        [[nodiscard]] FatPtr<T const> as_const_fat_ptr() const noexcept {
            return FatPtr{reinterpret_cast<T const*>(buf_), size_};
        }

        /// Get a pointer to the last element of the dynamic array.
        [[nodiscard]] T* peek() const noexcept {
            return this->is_empty() ? nullptr : &buf_[size_ - 1];
        }

        [[nodiscard]] T* begin() noexcept {
            return buf_;
        }

        [[nodiscard]] T const* begin() const noexcept {
            return reinterpret_cast<T const*>(buf_);
        }

        [[nodiscard]] T* end() noexcept {
            return ptr_add(buf_, size_);
        }

        [[nodiscard]] T const* end() const noexcept {
            return ptr_add(reinterpret_cast<T const*>(buf_), size_);
        }

        [[nodiscard]] T& operator[](usize idx) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(idx < size_, "Index out of bounds for dynamic array");
#endif
            return buf_[idx];
        }

        [[nodiscard]] T const& operator[](usize idx) const noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(idx < size_, "Index out of bounds for dynamic array");
#endif
            return buf_[idx];
        }

        /// Fill the entire dynamic array's buffer (up to its current capacity) with a given
        /// element.
        void fill(T _fill) noexcept
            requires TriviallyCopyable<T>
        {
            psh::fill(this->as_fat_ptr(), _fill);
            size_ = capacity_;
        }

        /// Inserts a new element to the end of the dynamic array.
        ///
        /// If the dynamic array capacity is full, the capacity is doubled by making a reallocation
        /// of the underlying memory buffer.
        void push(T new_element) noexcept {
            if (capacity_ == size_) {
                this->grow();
            }
            buf_[size_++] = new_element;
        }

        /// Resize the dynamic array underlying buffer.
        void grow(u32 factor = DYNARRAY_RESIZE_CAPACITY_FACTOR) noexcept {
            if (buf_ != nullptr) {
                // Reallocate the existing buffer.
                usize const previous_capacity = capacity_;
                capacity_ *= factor;
                buf_ = arena_->realloc<T>(buf_, previous_capacity, capacity_);
            } else {
                // Create a new buffer with a default capacity.
                capacity_ = DYNARRAY_DEFAULT_INITIAL_CAPACITY;
                buf_      = arena_->zero_alloc<T>(capacity_);
            }

            if (capacity_ != 0) {
                psh_assert_msg(buf_ != nullptr, "DynArray::grow unable to rellocate buffer");
            }
        }

        /// Try to pop the last element of the dynamic array.
        bool pop() noexcept {
            bool op = false;
            if (size_ > 0) {
                --size_;
                op = true;
            }
            return op;
        }

        /// Try to remove a dynamic array element at a given index.
        bool remove(usize idx) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            if (idx >= size_) {
                log_fmt(
                    LogLevel::Error,
                    "DynArray::remove index %zu is out of bounds for dynamic array of size "
                    "%zu.",
                    idx,
                    size_);
                return false;
            }
#endif

            if (idx != size_ - 1) {
                // If the element isn't the last we have copy the array content with overlap.
                u8*       dest = reinterpret_cast<u8*>(buf_) + sizeof(T) * idx;
                u8 const* src  = reinterpret_cast<u8*>(buf_) + sizeof(T) * (idx + 1);
                memory_move(dest, src, sizeof(T) * (size_ - idx - 1));
            }
            --size_;
            return true;
        }

        /// Clear the dynamic array data, resetting its size.
        constexpr void clear() noexcept {
            size_ = 0;
        }
    };
}  // namespace psh
