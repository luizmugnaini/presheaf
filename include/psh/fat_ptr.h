#pragma once

#include <psh/assert.h>
#include <psh/types.h>

namespace psh {
    /// Fat pointer.
    template <typename T>
    struct FatPtr {
        T*    buf_  = nullptr;
        usize size_ = 0;

        [[nodiscard]] constexpr T* buf() noexcept {
            return buf_;
        }

        [[nodiscard]] constexpr T const* const_buf() const noexcept {
            return buf_;
        }

        [[nodiscard]] constexpr usize size() const noexcept {
            return size_;
        }

        [[nodiscard]] constexpr usize size_bytes() const noexcept {
            return sizeof(T) * size_;
        }

        [[nodiscard]] constexpr T* begin() noexcept {
            return buf_;
        }

        [[nodiscard]] constexpr T const* begin() const noexcept {
            return static_cast<T const*>(buf_);
        }

        [[nodiscard]] constexpr T* end() noexcept {
            return (buf_ == nullptr) ? nullptr : buf_ + size_;
        }

        [[nodiscard]] constexpr T const* end() const noexcept {
            return (buf_ == nullptr) ? nullptr : static_cast<T const*>(buf_ + size_);
        }

        [[nodiscard]] constexpr T& operator[](usize idx) noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(idx < size_, "Index out of bounds for fat pointer");
#endif
            return buf_[idx];
        }

        [[nodiscard]] constexpr T const& operator[](usize idx) const noexcept {
#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
            psh_assert_msg(idx < size_, "Index out of bounds for fat pointer");
#endif
            return buf_[idx];
        }
    };

    template <typename T>
    FatPtr<u8> fat_ptr_as_bytes(T* buf, usize length) noexcept {
        return FatPtr{reinterpret_cast<u8*>(buf), sizeof(T) * length};
    }

    template <typename T>
    FatPtr<u8 const> fat_ptr_as_bytes(T const* buf, usize length) noexcept {
        return FatPtr{reinterpret_cast<u8 const*>(buf), sizeof(T) * length};
    }
}  // namespace psh
