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
/// Description: Tests for the string types.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <stdlib.h>
#include <string.h>
#include <psh_memory.hpp>
#include <psh_string.hpp>
#include "utils.hpp"

namespace psh::test::string {
    psh_internal void string_type() {
        String v1 = make_string("Nine for the Elven-kings under moon and star");
        psh_assert(string_equal(v1, "Nine for the Elven-kings under moon and star"));
        psh_assert(v1.count == cstring_length("Nine for the Elven-kings under moon and star"));
        psh_assert(v1.buf[v1.count] == 0);

        constexpr String v2 = psh_comptime_make_string("Nine for the Elven-kings under moon and star");
        psh_assert(string_equal(v2, "Nine for the Elven-kings under moon and star"));
        psh_assert(v2.count == cstring_length("Nine for the Elven-kings under moon and star"));
        psh_assert(v2.buf[v1.count] == 0);

        String v3 = make_string("Nine for the Elven-kings under moon and star");
        psh_assert(string_equal(v3, "Nine for the Elven-kings under moon and star"));
        psh_assert(v3.count == cstring_length("Nine for the Elven-kings under moon and star"));
        psh_assert(v3.buf[v1.count] == 0);
        report_test_successful();
    }

    psh_internal void dynamic_string_type() {
        Arena arena = make_owned_arena(512);
        {
            DynamicString s = make_dynamic_string(&arena, psh_comptime_make_string("Seven for the Dwarf-lords in their halls of stone"));
            psh_assert(string_equal(make_string(s), "Seven for the Dwarf-lords in their halls of stone"));
            psh_assert(s.count == strlen("Seven for the Dwarf-lords in their halls of stone"));
            psh_assert(s.capacity == s.count + 1u);
            psh_assert(s.buf[s.capacity - 1u] == 0);
        }
        destroy_owned_arena(&arena);
        report_test_successful();
    }

    psh_internal void string_joining_operation() {
        constexpr String check_str1 = psh_comptime_make_string(
            "One ring to rule them all, "
            "One ring to find them, "
            "One ring to bring them all, "
            "and in the darkness bind them.");

        constexpr Buffer<String, 4> views1 = {
            psh_comptime_make_string("One ring to rule them all"),
            psh_comptime_make_string("One ring to find them"),
            psh_comptime_make_string("One ring to bring them all"),
            psh_comptime_make_string("and in the darkness bind them."),
        };

        constexpr String check_str2 = psh_comptime_make_string(
            "Three Rings for the Elven-kings under the sky,\n"
            "Seven for the Dwarf-lords in their halls of stone,\n"
            "Nine for Mortal Men doomed to die,\n"
            "One for the Dark Lord on his dark throne\n"
            "In the Land of Mordor where the Shadows lie.\n"
            "One Ring to rule them all, One Ring to find them,\n"
            "One Ring to bring them all, and in the darkness bind them\n"
            "In the Land of Mordor where the Shadows lie.\n");

        constexpr Buffer<String, 8> views2 = {
            psh_comptime_make_string("Three Rings for the Elven-kings under the sky,\n"),
            psh_comptime_make_string("Seven for the Dwarf-lords in their halls of stone,\n"),
            psh_comptime_make_string("Nine for Mortal Men doomed to die,\n"),
            psh_comptime_make_string("One for the Dark Lord on his dark throne\n"),
            psh_comptime_make_string("In the Land of Mordor where the Shadows lie.\n"),
            psh_comptime_make_string("One Ring to rule them all, One Ring to find them,\n"),
            psh_comptime_make_string("One Ring to bring them all, and in the darkness bind them\n"),
            psh_comptime_make_string("In the Land of Mordor where the Shadows lie.\n"),
        };

        Arena arena = make_owned_arena(psh_kibibytes(5));
        {
            // Join into an empty string.
            {
                DynamicString estr = make_dynamic_string(&arena, 20);
                psh_assert(join_strings(estr, make_const_fat_ptr(&views1), psh_comptime_make_string(", ")));

                psh_assert(string_equal(make_string(estr), check_str1));
                psh_assert(estr.count == check_str1.count);
                psh_assert(estr.capacity == estr.count + 1u);
                psh_assert(estr.buf[estr.count] == 0);
            }

            // Join into a non-empty string 1.
            {
                DynamicString nestr = make_dynamic_string(&arena, views2[0]);
                psh_assert(join_strings(nestr, FatPtr{&views2[1], views2.count - 1u}));
                psh_assert(string_equal(make_string(nestr), check_str2));
                psh_assert(nestr.capacity == nestr.count + 1);
                psh_assert(nestr.buf[nestr.count] == 0);
            }

            // Join into a non-empty string 2.
            {
                Buffer<String, 3> words = {
                    make_string("Ring"),
                    make_string("to"),
                    make_string("rule"),
                };
                DynamicString s = make_dynamic_string(&arena, make_string("One"));

                psh_assert(join_strings(s, make_const_fat_ptr(&words), psh_comptime_make_string(", ")));
                psh_assert(string_equal(make_string(s), "One, Ring, to, rule"));
            }
        }
        destroy_owned_arena(&arena);

        report_test_successful();
    }

    psh_internal void run_all() {
        string_type();
        dynamic_string_type();
        string_joining_operation();
    }
}  // namespace psh::test::string

#if !defined(PSH_TEST_NOMAIN)
int main() {
    psh::test::string::run_all();
    return 0;
}
#endif
