/// Scope-based code deferrer.
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
///
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#pragma once

#include <psh/concepts.h>

namespace psh {
    template <typename T>
        requires NotLValueRef<T>
    T&& cast_forward(typename RemoveRef<T>::Type x) {
        return static_cast<T&&>(x);
    }

    template <typename Func>
    struct Deferrer {
        Func fn;

        Deferrer(Func&& _fn) : fn{_fn} {}
    };

    template <typename Func>
    Deferrer<Func> impl_make_defer_fn(Func&& fn) {
        return Deferrer{std::forward<Func>(fn)};
    }

#define psh_impl_defer_join(x, y)      x##y
#define psh_impl_defer_var(prefix, id) psh_impl_defer_join(prefix, id)
#define psh_defer(code)                                                 \
    [[maybe_unused]] auto psh_impl_defer_var(psh_deferred_, __LINE__) = \
        psh::impl_make_defer_fn([&]() {                                 \
            code;                                                       \
        })
}  // namespace psh
