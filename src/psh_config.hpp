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
/// Description: Library behaviour configuration flags.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

// -------------------------------------------------------------------------------------------------
// Presheaf library compile-time flags.
//
// - PSH_ENABLE_ASSERTIONS: Enable the use of asserts.
// - PSH_ENABLE_USAGE_VALIDATION: Insert checks to ensure that any given Presheaf function is given
//   valid arguments. In other words, this will assert that the implicit contract between caller
//   and callee is followed.
// - PSH_ENABLE_PARANOID_USAGE_VALIDATION: Inserts even more validation checks (this option isn't
//   enabled via PSH_ENABLE_DEBUG, you have to set it manually).
// - PSH_ENABLE_ASSERT_NOT_NULL: Assert that arguments that shouldn't be null, aren't.
// - PSH_ENABLE_ASSERT_NO_ALIAS: Assert that the no-aliasing rule is followed.
// - PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE: Check at compile time if template based functions have
//   arguments satisfying the procedure assumptions.
// - PSH_ENABLE_ASSERT_BOUNDS_CHECK: For every container-like struct, check if the accessing index stays
//   within the container memory region bounds.
// - PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP: Before calling memcpy, assert that the memory regions being
//   copied don't overlap.
// - PSH_ENABLE_ASSERT_NO_MEMORY_ERROR: When a memory acquisition function fails, abort the program.
// - PSH_ENABLE_LOGGING: Enable logging calls to execute.
// - PSH_ENABLE_DEBUG: Enables all of the above debug checks.
// - PSH_ENABLE_ANSI_COLOURS: When logging, use ANSI colour codes for pretty printing. This may not
//   be desired if you're printing to a log file, hence the option is disabled by default.
// - PSH_ENABLE_FORCED_INLINING: Disable the use of forced inlining hints via psh_inline.
// - PSH_ENABLE_NO_ALIAS: Disable the use of the no aliasing restriction hints in function
//   arguments that use psh_no_alias.
//
// @TODO: update the flag description to account for the new ones.
// -------------------------------------------------------------------------------------------------

// Enable all debug checks when compiled in debug mode. Otherwise, disable all.
#if defined(PSH_ENABLE_DEBUG) && PSH_ENABLE_DEBUG
#    if !defined(PSH_ENABLE_ASSERTIONS)
#        define PSH_ENABLE_ASSERTIONS 1
#    endif
#    if !defined(PSH_ENABLE_USAGE_VALIDATION)
#        define PSH_ENABLE_USAGE_VALIDATION 1
#    endif
#    if !defined(PSH_ENABLE_ASSERT_NOT_NULL)
#        define PSH_ENABLE_ASSERT_NOT_NULL 1
#    endif
#    if !defined(PSH_ENABLE_ASSERT_BOUNDS_CHECK)
#        define PSH_ENABLE_ASSERT_BOUNDS_CHECK 1
#    endif
#    if !defined(PSH_ENABLE_ASSERT_NO_ALIAS)
#        define PSH_ENABLE_ASSERT_NO_ALIAS 1
#    endif
#    if !defined(PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE)
#        define PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE 1
#    endif
#    if !defined(PSH_ENABLE_ASSERT_NO_MEMORY_ERROR)
#        define PSH_ENABLE_ASSERT_NO_MEMORY_ERROR 1
#    endif
#    if !defined(PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP)
#        define PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP 1
#    endif
#    if !defined(PSH_ENABLE_LOGGING)
#        define PSH_ENABLE_LOGGING 1
#    endif
#    if !defined(PSH_ENABLE_FORCED_INLINING)
#        define PSH_ENABLE_FORCED_INLINING 1
#    endif
#    if !defined(PSH_ENABLE_NO_ALIAS)
#        define PSH_ENABLE_NO_ALIAS 1
#    endif
#endif

//
// Set the default values for all flags:
//

#if !defined(PSH_ENABLE_USAGE_VALIDATION)
#    define PSH_ENABLE_USAGE_VALIDATION 0
#endif
#if !defined(PSH_ENABLE_PARANOID_USAGE_VALIDATION)
#    define PSH_ENABLE_PARANOID_USAGE_VALIDATION 0
#endif

#if !defined(PSH_ENABLE_ASSERTIONS)
#    define PSH_ENABLE_ASSERTIONS 0
#endif
#if !defined(PSH_ENABLE_ASSERT_NOT_NULL)
#    define PSH_ENABLE_ASSERT_NOT_NULL 0
#endif
#if !defined(PSH_ENABLE_ASSERT_BOUNDS_CHECK)
#    define PSH_ENABLE_ASSERT_BOUNDS_CHECK 0
#endif
#if !defined(PSH_ENABLE_ASSERT_NO_ALIAS)
#    define PSH_ENABLE_ASSERT_NO_ALIAS 0
#endif
#if !defined(PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE)
#    define PSH_ENABLE_STATIC_ASSERT_TEMPLATE_USAGE 0
#endif
#if !defined(PSH_ENABLE_ASSERT_NO_MEMORY_ERROR)
#    define PSH_ENABLE_ASSERT_NO_MEMORY_ERROR 0
#endif
#if !defined(PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP)
#    define PSH_ENABLE_ASSERT_MEMCPY_NO_OVERLAP 0
#endif

#if !defined(PSH_ENABLE_LOGGING)
#    define PSH_ENABLE_LOGGING 0
#endif
#if !defined(PSH_ENABLE_ANSI_COLOURS)
#    define PSH_ENABLE_ANSI_COLOURS 0
#endif

// @TODO: these are the new ones:

#if !defined(PSH_ENABLE_USE_STB_SPRINTF)
#    define PSH_ENABLE_USE_STB_SPRINTF 1
#endif

#if !defined(PSH_ENABLE_BRANCH_HINTS)
#    define PSH_ENABLE_BRANCH_HINTS 1
#endif

// @TODO: Currently not supported. Fix!
// #if !defined(PSH_ENABLE_STATIC_PROCEDURES)
// #    define PSH_ENABLE_STATIC_PROCEDURES 0
// #endif

#if !defined(PSH_ENABLE_NO_EXCEPT_PROCEDURES)
#    define PSH_ENABLE_NO_EXCEPT_PROCEDURES 1
#endif
#if !defined(PSH_ENABLE_FORCED_INLINING)
#    define PSH_ENABLE_FORCED_INLINING 1
#endif
#if !defined(PSH_ENABLE_NO_ALIAS)
#    define PSH_ENABLE_NO_ALIAS 1
#endif
