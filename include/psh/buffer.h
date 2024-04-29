/// Buffer with compile-time known size.
///
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#pragma once

#include <psh/fat_ptr.h>
#include <psh/types.h>

#include <type_traits>

namespace psh {
    /// Buffer with a compile-time known size.
    template <typename T, usize size_>
        requires std::is_object_v<T>
    struct Buffer {
        T buf_[size_]{};

        [[nodiscard]] constexpr T* buf() noexcept {
            return reinterpret_cast<T*>(buf_);
        }

        [[nodiscard]] constexpr T const* const_buf() const noexcept {
            return reinterpret_cast<T const*>(buf_);
        }

        [[nodiscard]] constexpr usize size() const noexcept {
            return size_;
        }

        [[nodiscard]] usize size_bytes() const noexcept {
            return sizeof(T) * size_;
        }

        [[nodiscard]] FatPtr<T> as_fat_ptr() noexcept {
            return FatPtr{buf_, size_};
        }

        [[nodiscard]] constexpr FatPtr<T const> as_const_fat_ptr() const noexcept {
            return FatPtr{reinterpret_cast<T const*>(buf_), size_};
        }

        [[nodiscard]] constexpr T* begin() noexcept {
            return reinterpret_cast<T*>(buf_);
        }

        [[nodiscard]] constexpr T const* begin() const noexcept {
            return reinterpret_cast<T const*>(buf_);
        }

        [[nodiscard]] constexpr T* end() noexcept {
            return reinterpret_cast<T*>(buf_) + size_;
        }

        [[nodiscard]] constexpr T const* end() const noexcept {
            return reinterpret_cast<T const*>(buf_) + size_;
        }

        [[nodiscard]] constexpr T& operator[](usize idx) noexcept {
            return buf_[idx];
        }

        [[nodiscard]] constexpr T const& operator[](usize idx) const noexcept {
            return buf_[idx];
        }
    };
}  // namespace psh
