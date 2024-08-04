///                          Presheaf Library
///    Copyright (C) 2024 Luiz Gustavo Mugnaini Anselmo
///
///    This program is free software; you can redistribute it and/or modify
///    it under the terms of the GNU General Public License as published by
///    the Free Software Foundation; either version 2 of the License, or
///    (at your option) any later version.
///
///    This program is distributed in the hope that it will be useful,
///    but WITHOUT ANY WARRANTY; without even the implied warranty of
///    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///    GNU General Public License for more details.
///
///    You should have received a copy of the GNU General Public License along
///    with this program; if not, write to the Free Software Foundation, Inc.,
///    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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

#include <psh/type_utils.h>

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
