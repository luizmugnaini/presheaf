#pragma once

#include <psh/assert.h>
#include <psh/concepts.h>

namespace psh {
    /// Option type.
    template <typename T>
        requires NotPointer<T>
    struct Option {
        T    val_{};
        bool has_val_ = false;

        constexpr Option() = default;
        constexpr Option(T _val) noexcept : val_{_val}, has_val_{true} {}
        constexpr Option& operator=(T _val) noexcept {
            val_     = _val;
            has_val_ = true;
            return *this;
        }

        [[nodiscard]] bool is_some() const noexcept {
            return has_val_;
        }

        [[nodiscard]] bool is_none() const noexcept {
            return !has_val_;
        }

        [[nodiscard]] T const& val_or(T const& default_val = {}) const noexcept {
            return has_val_ ? val_ : default_val;
        }

        [[nodiscard]] T const& demand(StrPtr msg = "Option::demand failed") const noexcept {
            psh_assert_msg(has_val_, msg);
            return val_;
        }

        [[nodiscard]] T val_unchecked() const noexcept {
            return val_;
        }

        [[nodiscard]] T const& val_ref_unchecked() const noexcept {
            return val_;
        }
    };
}  // namespace psh
