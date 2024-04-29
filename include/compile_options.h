/// Compilation options.
///
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>
#pragma once

namespace psh {
#if defined(PSH_DEBUG)
    [[maybe_unused]] constexpr bool DEBUG_MODE = true;
#else
    [[maybe_unused]] constexpr bool DEBUG_MODE = false;
#endif

#if defined(PSH_DEBUG) || defined(PSH_ENABLE_ASSERTS)
    [[maybe_unused]] constexpr bool ASSERTS_ENABLED = true;
#else
    [[maybe_unused]] constexpr bool ASSERTS_ENABLED = false;
#endif

#if defined(PSH_DEBUG) || defined(PSH_CHECK_BOUNDS)
    [[maybe_unused]] constexpr bool CHECK_BOUNDS = true;
#else
    [[maybe_unused]] constexpr bool CHECK_BOUNDS = false;
#endif

#if defined(PSH_DEBUG) || defined(PSH_ENABLE_LOGGING)
    [[maybe_unused]] constexpr bool LOGGING_ENABLED = true;
#else
    [[maybe_unused]] constexpr bool LOGGING_ENABLED = false;
#endif

#if defined(PSH_DEBUG) || defined(PSH_CHECK_MEMCPY_OVERLAP)
    [[maybe_unused]] constexpr bool CHECK_MEMCPY_OVERLAP = true;
#else
    [[maybe_unused]] constexpr bool CHECK_MEMCPY_OVERLAP = false;
#endif

#if defined(PSH_DEBUG) || defined(PSH_CHECK_ALIGNMENT)
    [[maybe_unused]] constexpr bool CHECK_ALIGNMENT = true;
#else
    [[maybe_unused]] constexpr bool CHECK_ALIGNMENT = false;
#endif

#if defined(PSH_DEBUG) || defined(PSH_CHECK_SHADER_COMPILATION)
    [[maybe_unused]] constexpr bool CHECK_SHADER_COMPILATION = true;
#else
    [[maybe_unused]] constexpr bool CHECK_SHADER_COMPILATION = false;
#endif

#if defined(PSH_DEBUG) || defined(PSH_CHECK_SHADER_LINKING)
    [[maybe_unused]] constexpr bool CHECK_SHADER_LINKING = true;
#else
    [[maybe_unused]] constexpr bool CHECK_SHADER_LINKING = false;
#endif

#if defined(PSH_DEBUG) || defined(PSH_CHECK_SHADER_LINKING)
    [[maybe_unused]] constexpr bool VULKAN_VALIDATION_LAYERS_ENABLED = true;
#else
    [[maybe_unused]] constexpr bool VULKAN_VALIDATION_LAYERS_ENABLED = false;
#endif
}  // namespace psh
