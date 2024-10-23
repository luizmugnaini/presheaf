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
#include <psh/buffer.hpp>
#include <psh/core.hpp>
#include <psh/string.hpp>
#include "utils.hpp"

psh_internal void test_str_type() {
    constexpr auto s = psh_str("Frodo Baggins");
    psh_assert(psh::str_equal(s.buf, "Frodo Baggins"));
    psh_assert(s.size() == strlen("Frodo Baggins"));
    psh_assert(s.buf[s.size()] == 0);
    test_passed();
}

psh_internal void test_string_view_type() {
    psh::StringView v1{"Nine for the Elven-kings under moon and star"};
    psh_assert(psh::str_equal(v1.data.buf, "Nine for the Elven-kings under moon and star"));
    psh_assert(v1.data.size == strlen("Nine for the Elven-kings under moon and star"));
    psh_assert(v1.data.buf[v1.data.size] == 0);

    constexpr psh::StringView v2 = psh_string_view("Nine for the Elven-kings under moon and star");
    psh_assert(psh::str_equal(v2.data.buf, "Nine for the Elven-kings under moon and star"));
    psh_assert(v2.data.size == strlen("Nine for the Elven-kings under moon and star"));
    psh_assert(v2.data.buf[v1.data.size] == 0);

    psh::StringView v3{"Nine for the Elven-kings under moon and star"};
    psh_assert(psh::str_equal(v3.data.buf, "Nine for the Elven-kings under moon and star"));
    psh_assert(v3.data.size == strlen("Nine for the Elven-kings under moon and star"));
    psh_assert(v3.data.buf[v1.data.size] == 0);
    test_passed();
}

psh_internal void test_string_type() {
    psh::Arena arena{reinterpret_cast<u8*>(malloc(512)), 512};
    {
        psh::String s{&arena, psh_string_view("Seven for the Dwarf-lords in their halls of stone")};
        psh_assert(psh::str_equal(s.data.buf, "Seven for the Dwarf-lords in their halls of stone"));
        psh_assert(s.data.size == strlen("Seven for the Dwarf-lords in their halls of stone"));
        psh_assert(s.data.capacity == s.data.size + 1);
        psh_assert(s.data.buf[s.data.capacity - 1] == 0);
    }
    free(arena.buf);
    test_passed();
}

psh_internal void test_string_join() {
    constexpr auto check_str1 = psh_str("One ring to rule them all, "
                                        "One ring to find them, "
                                        "One ring to bring them all, "
                                        "and in the darkness bind them.");

    constexpr psh::Buffer<psh::StringView, 4> views1{
        psh_string_view("One ring to rule them all"),
        psh_string_view("One ring to find them"),
        psh_string_view("One ring to bring them all"),
        psh_string_view("and in the darkness bind them."),
    };

    constexpr auto check_str2 =
        psh_str("Three Rings for the Elven-kings under the sky,\n"
                "Seven for the Dwarf-lords in their halls of stone,\n"
                "Nine for Mortal Men doomed to die,\n"
                "One for the Dark Lord on his dark throne\n"
                "In the Land of Mordor where the Shadows lie.\n"
                "One Ring to rule them all, One Ring to find them,\n"
                "One Ring to bring them all, and in the darkness bind them\n"
                "In the Land of Mordor where the Shadows lie.\n");

    constexpr psh::Buffer<psh::StringView, 8> views2{
        psh_string_view("Three Rings for the Elven-kings under the sky,\n"),
        psh_string_view("Seven for the Dwarf-lords in their halls of stone,\n"),
        psh_string_view("Nine for Mortal Men doomed to die,\n"),
        psh_string_view("One for the Dark Lord on his dark throne\n"),
        psh_string_view("In the Land of Mordor where the Shadows lie.\n"),
        psh_string_view("One Ring to rule them all, One Ring to find them,\n"),
        psh_string_view("One Ring to bring them all, and in the darkness bind them\n"),
        psh_string_view("In the Land of Mordor where the Shadows lie.\n"),
    };

    psh::Arena arena{reinterpret_cast<u8*>(malloc(psh_kibibytes(5))), psh_kibibytes(5)};
    {
        // Empty string.
        psh::String estr{&arena, 20};
        psh_assert(estr.join(psh::const_fat_ptr(views1), ", ") == psh::Status::OK);

        psh_assert(psh::str_equal(estr.data.buf, check_str1.buf));
        psh_assert(estr.data.size == check_str1.size());
        psh_assert(estr.data.capacity == estr.data.size + 1);
        psh_assert(estr.data.buf[estr.data.size] == 0);

        // Non-empty string.
        psh::String nestr{&arena, views2[0]};
        psh_assert(nestr.join(psh::FatPtr{&views2[1], views2.size() - 1}) == psh::Status::OK);
        psh_assert(psh::str_equal(nestr.data.buf, check_str2.buf));
        psh_assert(nestr.data.capacity == nestr.data.size + 1);
        psh_assert(nestr.data.buf[nestr.data.size] == 0);
    }
    free(arena.buf);

    test_passed();
}

psh_internal void test_string() {
    test_str_type();
    test_string_view_type();
    test_string_type();
    test_string_join();
}

#if !defined(PSH_TEST_NOMAIN)
int main() {
    test_string();
    return 0;
}
#endif
