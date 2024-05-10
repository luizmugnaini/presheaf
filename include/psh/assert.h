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
/// Description: Code assertion utilities.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

#include <psh/io.h>

namespace psh {
    // Internal definition of the assert macros
    void assert_(
        bool      expr_res,
        strptr    expr_str,
        strptr    msg  = "",
        LogInfo&& info = LogInfo{LogLevel::Fatal});
}  // namespace psh

/// Assert that an expression evaluates to true.
#define psh_assert(expr) psh::assert_(expr, #expr)

/// Assert that an expression evaluates to true and write a descriptive message in case of failure.
#define psh_assert_msg(expr, msg) psh::assert_(expr, #expr, msg)

/// Macro used to mark code-paths as unreachable.
#define psh_unreachable()                                                  \
    do {                                                                   \
        psh::log(psh::LogLevel::Fatal, "Codepath should be unreachable!"); \
        psh::abort_program();                                              \
    } while (0);

/// Macro used to mark code-paths as unimplemented.
#define psh_todo()                                                        \
    do {                                                                  \
        psh::log(psh::LogLevel::Fatal, "TODO: code-path unimplemented!"); \
        psh::abort_program();                                             \
    } while (0);
