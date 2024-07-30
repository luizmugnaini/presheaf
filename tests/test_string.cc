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
/// Description: Tests for the string types.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/buffer.h>
#include <psh/core.h>
#include <psh/string.h>
#include <cstdlib>
#include <cstring>
#include "utils.h"

void test_str_type() {
    constexpr auto s = psh_str("Frodo Baggins");
    psh_assert(psh::str_equal(s.buf, "Frodo Baggins"));
    psh_assert(s.size() == std::strlen("Frodo Baggins"));
    psh_assert(s.buf[s.size()] == 0);
    test_passed();
}

void test_string_view_type() {
    psh::StringView v1{"Nine for the Elven-kings under moon and star"};
    psh_assert(psh::str_equal(v1.data.buf, "Nine for the Elven-kings under moon and star"));
    psh_assert(v1.data.size == std::strlen("Nine for the Elven-kings under moon and star"));
    psh_assert(v1.data.buf[v1.data.size] == 0);

    constexpr psh::StringView v2 = psh_string_view("Nine for the Elven-kings under moon and star");
    psh_assert(psh::str_equal(v2.data.buf, "Nine for the Elven-kings under moon and star"));
    psh_assert(v2.data.size == std::strlen("Nine for the Elven-kings under moon and star"));
    psh_assert(v2.data.buf[v1.data.size] == 0);

    psh::StringView v3{"Nine for the Elven-kings under moon and star"};
    psh_assert(psh::str_equal(v3.data.buf, "Nine for the Elven-kings under moon and star"));
    psh_assert(v3.data.size == std::strlen("Nine for the Elven-kings under moon and star"));
    psh_assert(v3.data.buf[v1.data.size] == 0);
    test_passed();
}

void test_string_type() {
    psh::Arena arena{reinterpret_cast<u8*>(std::malloc(512)), 512};
    {
        psh::String s{&arena, psh_string_view("Seven for the Dwarf-lords in their halls of stone")};
        psh_assert(psh::str_equal(s.data.buf, "Seven for the Dwarf-lords in their halls of stone"));
        psh_assert(s.data.size == std::strlen("Seven for the Dwarf-lords in their halls of stone"));
        psh_assert(s.data.capacity == s.data.size + 1);
        psh_assert(s.data.buf[s.data.capacity - 1] == 0);
    }
    std::free(arena.buf);
    test_passed();
}

void test_string_join() {
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

    psh::Arena arena{reinterpret_cast<u8*>(std::malloc(psh_kibibytes(5))), psh_kibibytes(5)};
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

        // With initializer list.
        psh::String s{&arena, "Hello"};
        psh_assert(s.join({"Middle", "Earth"}, " ++ ") == psh::Status::OK);
        psh_assert(psh::str_equal(s.data.buf, "Hello ++ Middle ++ Earth"));
    }
    std::free(arena.buf);
    test_passed();
}

void test_string() {
    test_str_type();
    test_string_view_type();
    test_string_type();
    test_string_join();
}

#if !defined(NOMAIN)
int main() {
    test_string();
    return 0;
}
#endif
