#pragma once

#include <psh/concepts.h>

namespace psh {
    /// Type holding an immutable pointer that can never be null.
    template <typename T>
        requires NotPointer<T>
    struct NotNull {
        T* const ptr;

        NotNull(T* _ptr) noexcept : ptr{_ptr} {
            psh_assert_msg(ptr != nullptr, "NotNull created with a null pointer");
        }

        [[nodiscard]] T& operator*() noexcept {
            return *ptr;
        }

        [[nodiscard]] T* operator->() noexcept {
            return ptr;
        }

        NotNull() = delete;
    };
}  // namespace psh
