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
/// Description: Platform-related attributes detection and tweaking.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

// -------------------------------------------------------------------------------------------------
// Architecture detection.
// -------------------------------------------------------------------------------------------------

/// All processor architecture information.
#define PSH_ARCH_X64       0
#define PSH_ARCH_ARM       0
#define PSH_ARCH_SIMD_SSE  0
#define PSH_ARCH_SIMD_SSE2 0
#define PSH_ARCH_SIMD_AVX  0
#define PSH_ARCH_SIMD_AVX2 0
#define PSH_ARCH_SIMD_NEON 0

// Detect processor architecture.
#if defined(__x86_64__) || defined(_M_X64) || defined(__amd64__)
#    undef PSH_ARCH_X64
#    define PSH_ARCH_X64 1
#elif defined(__arm__) || defined(_ARM_) || defined(_ARM_ARCH)
#    undef PSH_ARCH_ARM
#    define PSH_ARCH_ARM 1
#endif

// -------------------------------------------------------------------------------------------------
// Operating system and compiler detection.
// -------------------------------------------------------------------------------------------------

/// All supported operating systems.
#define PSH_OS_WINDOWS_32 0
#define PSH_OS_WINDOWS_64 0
#define PSH_OS_APPLE      0
#define PSH_OS_LINUX      0
#define PSH_OS_UNIX       0
#define PSH_OS_POSIX      0

#if defined(_WIN32)
#    undef PSH_OS_WINDOWS_32
#    define PSH_OS_WINDOWS_32 1
#elif defined(_WIN64)
#    undef PSH_OS_WINDOWS_64
#    define PSH_OS_WINDOWS_64 1
#endif

#if PSH_OS_WINDOWS_32 || PSH_OS_WINDOWS_64
#    undef PSH_OS_WINDOWS
#    define PSH_OS_WINDOWS 1
#endif

#if defined(__APPLE__) || defined(__MACH__)
#    undef PSH_OS_APPLE
#    define PSH_OS_APPLE 1
#endif

#if defined(__linux__)
#    undef PSH_OS_LINUX
#    define PSH_OS_LINUX 1
#endif

#if defined(__unix__)
#    undef PSH_OS_UNIX
#    define PSH_OS_UNIX 1
#endif

#if defined(_POSIX_VERSION)
#    undef PSH_OS_POSIX
#    define PSH_OS_POSIX 1
#endif

// -------------------------------------------------------------------------------------------------
// Windows debloating.
// -------------------------------------------------------------------------------------------------

/// Windows-specific tweaks.
///
/// Disable most of the useless stuff that comes bundled with the Windows header files.
#if PSH_OS_WINDOWS
#    if !defined(WIN32_LEAN_AND_MEAN)
#        define WIN32_LEAN_AND_MEAN
#    endif
#    if !defined(NOMINMAX)
#        define NOMINMAX
#    endif
#    if !defined(NOATOM)
#        define NOATOM
#    endif
#    if !defined(NOGDI)
#        define NOGDI
#    endif
#    if !defined(NOKERNEL)
#        define NOKERNEL
#    endif
#    if !defined(NOUSER)
#        define NOUSER
#    endif
#    if !defined(NONLS)
#        define NONLS
#    endif
#    if !defined(NOMB)
#        define NOMB
#    endif
#    if !defined(NOMEMMGR)
#        define NOMEMMGR
#    endif
#    if !defined(NOMETAFILE)
#        define NOMETAFILE
#    endif
#    if !defined(NOOPENFILE)
#        define NOOPENFILE
#    endif
#    if !defined(NOSERVICE)
#        define NOSERVICE
#    endif
#    if !defined(NOSOUND)
#        define NOSOUND
#    endif
#    if !defined(NOWH)
#        define NOWH
#    endif
#    if !defined(NOCOMM)
#        define NOCOMM
#    endif
#    if !defined(NODEFERWINDOWPOS)
#        define NODEFERWINDOWPOS
#    endif
#    if !defined(NOMCX)
#        define NOMCX
#    endif
#    if !defined(NOIME)
#        define NOIME
#    endif
#endif  // PSH_OS_WINDOWS

// -------------------------------------------------------------------------------------------------
// Compiler detection.
// -------------------------------------------------------------------------------------------------

/// All supported compilers.
#define PSH_COMPILER_MSVC      0
#define PSH_COMPILER_MSVC_YEAR 0
#define PSH_COMPILER_CLANG     0
#define PSH_COMPILER_CLANG_CL  0
#define PSH_COMPILER_GCC       0

#if defined(_MSC_VER)
#    undef PSH_COMPILER_MSVC
#    define PSH_COMPILER_MSVC 1
#    undef PSH_COMPILER_MSVC_YEAR

#    if _MSC_VER >= 1920
#        define PSH_COMPILER_MSVC_YEAR 2019
#    elif _MSC_VER >= 1910
#        define PSH_COMPILER_MSVC_YEAR 2017
#    elif _MSC_VER >= 1900
#        define PSH_COMPILER_MSVC_YEAR 2015
#    else
#        error "Only MSVC 2015 or later is supported."
#    endif

#    if defined(__clang_major__)
#        undef PSH_COMPILER_CLANG_CL
#        define PSH_COMPILER_CLANG_CL 1
#        undef PSH_COMPILER_CLANG
#        define PSH_COMPILER_CLANG 1
#    endif
#elif defined(__clang__)
#    undef PSH_COMPILER_CLANG
#    define PSH_COMPILER_CLANG 1
#elif defined(__GNUC__)
#    undef PSH_COMPILER_GCC
#    define PSH_COMPILER_GCC 1
#else
#    error "Compiler not supported. Please use MSVC, Clang or GCC."
#endif

// -------------------------------------------------------------------------------------------------
// SIMD detection.
// -------------------------------------------------------------------------------------------------

// Detect SIMD availability in x64 processors.
#if PSH_ARCH_X64
#    if PSH_COMPILER_MSVC
#        if defined(_M_AMD64)
#            undef PSH_ARCH_SIMD_SSE
#            define PSH_ARCH_SIMD_SSE 1
#            undef PSH_ARCH_SIMD_SSE2
#            define PSH_ARCH_SIMD_SSE2 1
#        endif
#        if defined(__AVX2__)
#            undef PSH_ARCH_SIMD_AVX
#            define PSH_ARCH_SIMD_AVX 1
#            undef PSH_ARCH_SIMD_AVX2
#            define PSH_ARCH_SIMD_AVX2 1
#        elif defined(__AVX__)
#            undef PSH_ARCH_SIMD_AVX
#            define PSH_ARCH_SIMD_AVX 1
#        endif
#    elif PSH_COMPILER_CLANG || PSH_COMPILER_GCC
#        if defined(__SSE__)
#            undef PSH_ARCH_SIMD_SSE
#            define PSH_ARCH_SIMD_SSE 1
#        endif
#        if defined(__SSE2__)
#            undef PSH_ARCH_SIMD_SSE2
#            define PSH_ARCH_SIMD_SSE2 1
#        endif
#        if defined(__AVX__)
#            undef PSH_ARCH_SIMD_AVX
#            define PSH_ARCH_SIMD_AVX 1
#        endif
#        if defined(__AVX2__)
#            undef PSH_ARCH_SIMD_AVX2
#            define PSH_ARCH_SIMD_AVX2 1
#        endif
#    endif
#endif

// Detect SIMD availability in ARM processors.
#if PSH_ARCH_ARM && defined(__ARM_NEON)
#    undef PSH_ARCH_SIMD_NEON
#    define PSH_ARCH_SIMD_NEON 1
#endif

// -------------------------------------------------------------------------------------------------
// Compiler capabilities.
// -------------------------------------------------------------------------------------------------

/// All compiler capabilities to be detected.
#define PSH_COMPILER_CAPABILITY_HAS_FEATURE 0
#define PSH_COMPILER_CAPABILITY_HAS_INCLUDE 0

#if defined(__has_feature)
#    undef PSH_COMPILER_CAPABILITY_HAS_FEATURE
#    define PSH_COMPILER_CAPABILITY_HAS_FEATURE 1
#endif

#if defined(__has_include)
#    undef PSH_COMPILER_CAPABILITY_HAS_INCLUDE
#    define PSH_COMPILER_CAPABILITY_HAS_INCLUDE 1
#endif

// -------------------------------------------------------------------------------------------------
// Address sanitizer.
// -------------------------------------------------------------------------------------------------

/// All address sanitizer related macros.
#define PSH_ADDRESS_SANITIZER_ENABLED     0
#define PSH_RUNTIME_HAS_ADDRESS_SANITIZER 0
#define psh_attribute_disable_asan

// Function attribute for locally disabling the address sanitizer.
#if PSH_COMPILER_MSVC
#    if defined(__SANITIZE_ADDRESS__) && __SANITIZE_ADDRESS__
#        undef PSH_ADDRESS_SANITIZER_ENABLED
#        define PSH_ADDRESS_SANITIZER_ENABLED 1
#        undef psh_attribute_disable_asan
#        define psh_attribute_disable_asan __declspec(no_sanitize_address)
#    endif
#elif PSH_COMPILER_CLANG
#    if PSH_COMPILER_CAPABILITY_HAS_FEATURE
#        if __has_feature(address_sanitizer)
#            undef PSH_ADDRESS_SANITIZER_ENABLED
#            define PSH_ADDRESS_SANITIZER_ENABLED 1
#            undef psh_attribute_disable_asan
#            define psh_attribute_disable_asan __attribute__((__no_sanitize__("address")))
#        endif
#    endif
#elif PSH_COMPILER_GCC
#    if defined(__SANITIZE_ADDRESS__) && __SANITIZE_ADDRESS__
#        undef PSH_ADDRESS_SANITIZER_ENABLED
#        define PSH_ADDRESS_SANITIZER_ENABLED 1
#        undef psh_attribute_disable_asan
#        define psh_attribute_disable_asan __attribute__((__no_sanitize_address__))
#    endif
#endif

// Check if we can use the address sanitizer runtime interface.
#if PSH_COMPILER_CAPABILITY_HAS_INCLUDE
#    if __has_include(<sanitizer/asan_interface.h>)
#        undef PSH_RUNTIME_HAS_ADDRESS_SANITIZER
#        define PSH_RUNTIME_HAS_ADDRESS_SANITIZER 1
#    endif
#endif

// -------------------------------------------------------------------------------------------------
// Compiler hints.
// -------------------------------------------------------------------------------------------------

/// printf-like function attribute.
///
/// Parameters:
///     - fmt_pos: The position of the argument containing the formatting string (the first argument
///                of a function has position 1).
#if PSH_COMPILER_CLANG || PSH_COMPILER_GCC
#    define psh_attribute_fmt(fmt_pos) __attribute__((__format__(__printf__, fmt_pos, fmt_pos + 1)))
#else
#    define psh_attribute_fmt(fmt_pos)
#endif

// -------------------------------------------------------------------------------------------------
// Source introspection.
// -------------------------------------------------------------------------------------------------

/// Query the string representing the signature of the current function.
#if PSH_COMPILER_CLANG || PSH_COMPILER_GCC
#    define psh_source_function_signature() __PRETTY_FUNCTION__
#elif PSH_COMPILER_MSVC
#    define psh_source_function_signature() __FUNCSIG__
#else
#    define psh_source_function_signature() "<unknown signature>"
#endif

/// Query the string representing the unadorned name of the current function.
#if PSH_COMPILER_CLANG || PSH_COMPILER_GCC
#    define psh_source_function_name() __func__
#elif PSH_COMPILER_MSVC
#    define psh_source_function_name() __FUNCTION__
#else
#    define psh_source_function_name() "<unknown name>"
#endif

#if PSH_COMPILER_CLANG || PSH_COMPILER_GCC || PSH_COMPILER_MSVC
#    define psh_source_file_name()   __builtin_FILE()
#    define psh_source_line_number() __builtin_LINE()
#else
#    define psh_source_file_name()   "<unknown file>"
#    define psh_source_line_number() 0
#endif
