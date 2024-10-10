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
/// Description: Optional type.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <psh/assert.hpp>

namespace psh {
    enum struct Status : bool {
        OK     = true,
        FAILED = false,
    };

    /// Option type.
    ///
    /// Note: This struct shouldn't be used with pointer types, as a null will still indicate a value
    ///       being present.
    template <typename T>
    struct Option {
        T    val     = {};
        bool has_val = false;

        constexpr Option() noexcept = default;

        constexpr Option(T _val) noexcept
            : val{_val}, has_val{true} {}

        constexpr Option& operator=(T _val) noexcept {
            this->val     = _val;
            this->has_val = true;
            return *this;
        }

        T const& val_or(T const& default_val = {}) const noexcept {
            return this->has_val ? this->val : default_val;
        }

        T const& demand(strptr msg = "Option::demand failed") const noexcept {
            psh_assert_msg(this->has_val, msg);
            return this->val;
        }
    };
}  // namespace psh
