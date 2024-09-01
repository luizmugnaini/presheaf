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

#include <psh/type_utils.hh>

namespace psh {
    template <typename Func>
    struct Deferrer {
        Func fn;

        Deferrer(Func&& _fn) {
            this->fn = _fn;
        }
        ~Deferrer() {
            fn();
        }
    };

    template <typename Func>
    Deferrer<Func> impl_make_defer_fn(Func&& fn) {
        return Deferrer{cast_forward<Func>(fn)};
    }

#define psh_impl_defer_join(x, y)      x##y
#define psh_impl_defer_var(prefix, id) psh_impl_defer_join(prefix, id)
#define psh_defer(code)                                                 \
    [[maybe_unused]] auto psh_impl_defer_var(psh_deferred_, __LINE__) = \
        psh::impl_make_defer_fn([&]() {                                 \
            code;                                                       \
        })
}  // namespace psh
