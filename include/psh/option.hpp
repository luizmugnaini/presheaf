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
    /// Option type.
    ///
    /// Note: This struct shouldn't be used with pointer types, as a null will still indicate a value
    ///       being present.
    template <typename T>
    struct psh_api Option {
        T    value     = {};
        bool has_value = false;

        constexpr Option() psh_no_except = default;

        constexpr Option(T value_) psh_no_except {
            this->value     = value_;
            this->has_value = true;
        }

        constexpr Option& operator=(T value_) psh_no_except {
            this->value     = value_;
            this->has_value = true;
            return *this;
        }

        T const& value_or(T const& default_value = {}) const psh_no_except {
            return this->has_value ? this->value : default_value;
        }

        T const& demand(cstring msg = "") const psh_no_except {
            psh_assert_msg(this->has_value, msg);
            return this->value;
        }
    };
}  // namespace psh
