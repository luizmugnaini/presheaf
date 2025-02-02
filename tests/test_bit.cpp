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
/// Description: Tests for the bit twiddling utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh_bit.hpp>
#include "utils.hpp"

namespace psh::test::bit {
    psh_internal void little_endian() {
        psh_assert_msg(arch_is_little_endian(), "Expected the current machine architecture to be little endian.");

        report_test_successful();
    }

    psh_internal void create_with_bit() {
        psh_assert_constexpr(psh_bit(u32, 3) == 0b1000);
        psh_assert_constexpr(psh_bit(u32, 1) == 0b10);
        psh_assert_constexpr(psh_bit(u32, 0) == 0b1);

        psh_assert_constexpr(psh_not_bit(u8, 2) == 0b11111011);
        psh_assert_constexpr(psh_not_bit(u8, 4) == 0b11101111);
        psh_assert_constexpr(psh_not_bit(u32, 0) == 0b11111111111111111111111111111110);
        psh_assert_constexpr(psh_not_bit(u32, 12) == 0b11111111111111111110111111111111);

        psh_assert_constexpr(psh_bit_ones(5) == 0b11111);
        psh_assert_constexpr(psh_bit_ones(1) == 0b1);
        psh_assert_constexpr(psh_bit_ones(0) == 0b0);
        psh_assert_constexpr(psh_bit_ones(7) == 0b1111111);
        psh_assert_constexpr(psh_bit_ones(8) == 0b11111111);

        report_test_successful();
    }

    psh_internal void set_and_clear_bit() {
        u32 x = 0;
        psh_bit_set(x, 6);
        psh_assert(x == 0b1000000);
        psh_bit_set(x, 0);
        psh_assert(x == 0b1000001);
        psh_bit_set(x, 3);
        psh_assert(x == 0b1001001);

        u8 y = 0b11111111;
        psh_bit_clear(y, 5);
        psh_assert(y == 0b11011111);
        psh_bit_clear(y, 0);
        psh_assert(y == 0b11011110);
        psh_bit_clear(y, 7);
        psh_assert(y == 0b01011110);

        u8 z = 0b11111111;
        psh_bit_set_or_clear_if(z, 4, true);
        psh_assert(z == 0b11111111);
        psh_bit_set_or_clear_if(z, 4, false);
        psh_assert(z == 0b11101111);
        psh_bit_set_or_clear_if(z, 0, false);
        psh_assert(z == 0b11101110);
        psh_bit_set_or_clear_if(z, 7, false);
        psh_assert(z == 0b01101110);
        psh_bit_set_or_clear_if(z, 3, false);
        psh_assert(z == 0b01100110);
        psh_bit_set_or_clear_if(z, 3, true);
        psh_assert(z == 0b01101110);
        psh_bit_set_or_clear_if(z, 7, true);
        psh_assert(z == 0b11101110);
        psh_bit_set_or_clear_if(z, 0, true);
        psh_assert(z == 0b11101111);
        psh_bit_set_or_clear_if(z, 4, true);
        psh_assert(z == 0b11111111);

        report_test_successful();
    }

    psh_internal void get_bit_at() {
        constexpr u32 x = 0b1011101011;

        psh_assert_constexpr(psh_bit_at(x, 0) == 1);
        psh_assert_constexpr(psh_bit_at(x, 1) == 1);
        psh_assert_constexpr(psh_bit_at(x, 2) == 0);
        psh_assert_constexpr(psh_bit_at(x, 3) == 1);
        psh_assert_constexpr(psh_bit_at(x, 4) == 0);
        psh_assert_constexpr(psh_bit_at(x, 5) == 1);
        psh_assert_constexpr(psh_bit_at(x, 6) == 1);
        psh_assert_constexpr(psh_bit_at(x, 7) == 1);
        psh_assert_constexpr(psh_bit_at(x, 8) == 0);
        psh_assert_constexpr(psh_bit_at(x, 9) == 1);

        psh_assert_constexpr(psh_bits_at(x, 0, 1) == psh_bit_at(x, 0));
        psh_assert_constexpr(psh_bits_at(x, 1, 1) == psh_bit_at(x, 1));
        psh_assert_constexpr(psh_bits_at(x, 2, 1) == psh_bit_at(x, 2));
        psh_assert_constexpr(psh_bits_at(x, 4, 1) == psh_bit_at(x, 4));
        psh_assert_constexpr(psh_bits_at(x, 3, 1) == psh_bit_at(x, 3));
        psh_assert_constexpr(psh_bits_at(x, 5, 1) == psh_bit_at(x, 5));
        psh_assert_constexpr(psh_bits_at(x, 6, 1) == psh_bit_at(x, 6));
        psh_assert_constexpr(psh_bits_at(x, 7, 1) == psh_bit_at(x, 7));
        psh_assert_constexpr(psh_bits_at(x, 8, 1) == psh_bit_at(x, 8));
        psh_assert_constexpr(psh_bits_at(x, 9, 1) == psh_bit_at(x, 9));

        psh_assert_constexpr(psh_bits_at(x, 4, 6) == 0b101110);
        psh_assert_constexpr(psh_bits_at(x, 0, 4) == 0b1011);
        psh_assert_constexpr(psh_bits_at(x, 0, 2) == 0b11);
        psh_assert_constexpr(psh_bits_at(x, 0, 3) == 0b011);

        report_test_successful();
    }

    psh_internal void get_u8_nibbles() {
        constexpr u16 x = 0xAB;
        psh_assert_constexpr(psh_u8_lo(x) == 0x0B);
        psh_assert_constexpr(psh_u8_hi(x) == 0x0A);

        report_test_successful();
    }

    psh_internal void make_u16_from_bytes() {
        psh_assert_constexpr(psh_u8_to_u16_hi(0xFC) == 0xFC00);

        psh_assert_constexpr(psh_u16_from_bytes(0xFB, 0xA3) == 0xFBA3);
        psh_assert_constexpr(psh_u16_from_bytes(0x01, 0x01) == 0x0101);
        psh_assert_constexpr(psh_u16_from_bytes(0xAB, 0xBA) == 0xABBA);
        psh_assert_constexpr(psh_u16_from_bytes(0x00, 0x00) == 0x0000);
        psh_assert_constexpr(psh_u16_from_bytes(0xAA, 0xAA) == 0xAAAA);

        report_test_successful();
    }

    psh_internal void get_u16_bytes() {
        psh_assert_constexpr(psh_u16_hi(0xFBA3) == 0xFB);
        psh_assert_constexpr(psh_u16_hi(0x1101) == 0x11);
        psh_assert_constexpr(psh_u16_hi(0xABBA) == 0xAB);
        psh_assert_constexpr(psh_u16_hi(0x0000) == 0x00);
        psh_assert_constexpr(psh_u16_hi(0xAAAA) == 0xAA);

        psh_assert_constexpr(psh_u16_lo(0xFBA3) == 0xA3);
        psh_assert_constexpr(psh_u16_lo(0x1101) == 0x01);
        psh_assert_constexpr(psh_u16_lo(0xABBA) == 0xBA);
        psh_assert_constexpr(psh_u16_lo(0x0000) == 0x00);
        psh_assert_constexpr(psh_u16_lo(0xAAAA) == 0xAA);

        report_test_successful();
    }

    psh_internal void set_u16_bytes() {
        u16 x = 0x01BB;
        psh_u16_set_hi(x, 0xFA);
        psh_assert(x == 0xFABB);
        psh_u16_set_lo(x, 0xAB);
        psh_assert(x == 0xFAAB);

        psh_assert_constexpr(psh_u16_from_bytes(0x00, 0x00) == 0x0000);
        psh_assert_constexpr(psh_u16_from_bytes(0xFF, 0xC3) == 0xFFC3);
        psh_assert_constexpr(psh_u16_from_bytes(0xAB, 0xCD) == 0xABCD);

        report_test_successful();
    }

    psh_internal void integers_have_opposite_sign() {
        psh_assert_constexpr(!psh_int_opposite_sign(8127, 1209831));
        psh_assert_constexpr(psh_int_opposite_sign(-8127, 1209831));
        psh_assert_constexpr(psh_int_opposite_sign(8127, -1209831));

        report_test_successful();
    }

    psh_internal void integer_swap_values() {
        i32 x = -1238;
        i32 y = 2193;
        psh_int_swap(x, y);
        psh_assert((x == 2193) && (y == -1238));

        u32 z = 256;
        u32 w = 102983;
        psh_int_swap(z, w);
        psh_assert((z == 102983) && (w == 256));

        report_test_successful();
    }

    psh_internal void run_all() {
        little_endian();
        create_with_bit();
        set_and_clear_bit();
        get_bit_at();
        get_u8_nibbles();
        make_u16_from_bytes();
        get_u16_bytes();
        set_u16_bytes();
        integers_have_opposite_sign();
        integer_swap_values();
    }
}  // namespace psh::test::bit

#if !defined(PSH_TEST_NOMAIN)
int main() {
    psh::test::bit::run_all();
    return 0;
}
#endif
