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
/// Description: Implementation of the code assertion utilities.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#include <psh/assert.h>

namespace psh {
    void assert_(bool expr_res, StrPtr expr_str, StrPtr msg, LogInfo&& info) {
#if defined(PSH_DEBUG) || defined(PSH_ENABLE_ASSERTS)
        if (!expr_res) {
            log_fmt(info, "Assertion failed: %s, msg: %s", expr_str, msg);
            abort_program();
        }
#endif
    }
}  // namespace psh
