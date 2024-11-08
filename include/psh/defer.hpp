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
/// Description: Scope-based code deferrer.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
///
/// Usage example:
/// ```
/// ... do stuff ...
/// {
///     VkShaderModule shaders[2]{
///         make_shader_module("shaders/foo.frag.spv"),
///         make_shader_module("shaders/foo.vert.spv"),
///     };
///     psh_defer({
///         VkDestroyShaderModule(my_logical_device, my_shaders[0], my_allocator);
///         VkDestroyShaderModule(my_logical_device, my_shaders[1], my_allocator);
///     })
///
///     ... create my graphics pipeline ...
/// }
/// ... do some other stuff ...
/// ```
/// Here the shader modules will be automatically destroyed at the end of the scope. Notice that the
/// macro `psh_defer` can accept a block of code to be run, or a single function call like
/// `psh_defer(bar_fn(x, y, z))`.

#pragma once

#include <type_traits>

namespace psh::impl::defer {
    // -----------------------------------------------------------------------------
    // Implementation details, some type trickery.
    // -----------------------------------------------------------------------------

    template <typename T>
    struct RemoveRef {
        using Type = T;
    };
    template <typename T>
    struct RemoveRef<T&> {
        using Type = T;
    };
    template <typename T>
    struct RemoveRef<T&&> {
        using Type = T;
    };

    template <typename T>
        requires(!std::is_lvalue_reference_v<T>)
    T&& cast_forward(typename RemoveRef<T>::Type x) {
        return static_cast<T&&>(x);
    }

    template <typename Func>
    struct psh_api Deferrer {
        Func fn;

        Deferrer(Func&& fn_) {
            this->fn = fn_;
        }
        ~Deferrer() {
            fn();
        }
    };

    template <typename Func>
    Deferrer<Func> make_defer_fn(Func&& fn) {
        return Deferrer{cast_forward<Func>(fn)};
    }
}  // namespace psh::impl::defer

// -----------------------------------------------------------------------------
// Defer interface.
// -----------------------------------------------------------------------------

#define psh_defer(code)                                                               \
    [[maybe_unused]] auto psh_token_concat(psh_deferred_, psh_source_line_number()) = \
        psh::impl::defer::make_defer_fn([&]() {                                       \
            code;                                                                     \
        })
