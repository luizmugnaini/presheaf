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
/// Description: Implementation of utility functions for working with assertions.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#include <psh/assert.hpp>
#include <psh/core.hpp>

namespace psh {
    namespace impl::assertion {
        psh_internal void default_abort_function(void* arg) psh_no_except {
            psh_discard_value(arg);

#if defined(PSH_COMPILER_MSVC)
            __debugbreak();
#elif defined(PSH_COMPILER_CLANG) || defined(PSH_COMPILER_GCC)
            __builtin_trap();
#else
            // Stall the program if we don't have a sane default.
            for (;;) {}
#endif
        };

        psh_internal void*          abort_context  = nullptr;
        psh_internal AbortFunction* abort_function = default_abort_function;
    }  // namespace impl::assertion

    void set_abort_function(AbortFunction* abort_function, void* abort_context) psh_no_except {
        impl::assertion::abort_context  = abort_context;
        impl::assertion::abort_function = abort_function;
    }

    psh_api void abort_program() psh_no_except {
        impl::assertion::abort_function(impl::assertion::abort_context);
    }
}  // namespace psh
