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
/// Description: Tests for the system time interface.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#include <psh_debug.hpp>
#include "utils.hpp"

// @TODO: can we programatically validate if the loggings are correct? Is it even worth it?
namespace psh::test::logging {
    struct Foo {
        cstring var_cstring;
        f64     var_f64;
        f32     var_f32;
        i32     var_i32;
        u32     var_u32;
        char    var_char;
    };

    psh_internal void printing_to_console() {
        psh_log_debug("Message logging works! Gandalf war right this whole time!");
        report_test_successful();
    }

    psh_internal void formatted_printing() {
        Foo f = {
            .var_cstring = "test",
            .var_f64     = 12903710293.1823719,
            .var_f32     = 3.14f,
            .var_i32     = -1234,
            .var_u32     = 1234,
            .var_char    = 'Z',
        };
        psh_log_debug_fmt(
            "Formatted %s logging %s: %s (cstring) %f (f64) %f (f32) %d (i32) %u (u32) %c (char) %p (pointer)",
            "message",
            "works",
            f.var_cstring,
            f.var_f64,
            static_cast<f64>(f.var_f32),
            f.var_i32,
            f.var_u32,
            f.var_char,
            reinterpret_cast<void*>(&f));

        report_test_successful();
    }

    psh_internal void run_all() {
        printing_to_console();
        formatted_printing();
    }
}  // namespace psh::test::logging

#if !defined(PSH_TEST_NOMAIN)
int main() {
    psh::test::logging::run_all();
    return 0;
}
#endif
