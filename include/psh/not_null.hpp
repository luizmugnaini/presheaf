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
/// Description: Simple wrapper around pointers that ensures that the internal pointer is non-null.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

namespace psh {
    /// Type holding an immutable pointer that can never be null.
    ///
    /// Note: If you use this struct with a pointer type, beware that the pointer may not be null but
    ///       the pointer that it points to may be null as we don't check that.
    template <typename T>
    struct psh_api NotNull {
        T* const ptr;

        NotNull(T* ptr_) psh_noexcept {
            psh_assert_msg(ptr_ != nullptr, "Invalid pointer, should be non-null.");
            this->ptr = ptr_;
        }

        T& operator*() psh_noexcept {
            return *this->ptr;
        }

        T* operator->() psh_noexcept {
            return this->ptr;
        }

        NotNull() = delete;
    };
}  // namespace psh
