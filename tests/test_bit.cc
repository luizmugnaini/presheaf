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
/// Description: Tests for the bit twiddling utilities.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#include <psh/assert.h>
#include <psh/bit.h>
#include <psh/log.h>
#include <psh/types.h>
#include "utils.h"

void test_create_bit() {
    psh_assert(psh_bit(u32, 3) == 0b1000);
    psh_assert(psh_bit(u32, 1) == 0b10);
    psh_assert(psh_bit(u32, 0) == 0b1);

    psh_assert(psh_not_bit(u8, 2) == 0b11111011);
    psh_assert(psh_not_bit(u8, 4) == 0b11101111);
    psh_assert(psh_not_bit(u32, 0) == 0b11111111111111111111111111111110);
    psh_assert(psh_not_bit(u32, 12) == 0b11111111111111111110111111111111);

    psh_assert(psh_bit_ones(5) == 0b11111);
    psh_assert(psh_bit_ones(1) == 0b1);
    psh_assert(psh_bit_ones(0) == 0b0);
    psh_assert(psh_bit_ones(7) == 0b1111111);
    psh_assert(psh_bit_ones(8) == 0b11111111);

    test_passed();
}

void test_bit_set_and_clear() {
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

    test_passed();
}

void test_bit_at() {
    u32 x = 0b1011101011;

    psh_assert(psh_bit_at(x, 0) == 1);
    psh_assert(psh_bit_at(x, 1) == 1);
    psh_assert(psh_bit_at(x, 2) == 0);
    psh_assert(psh_bit_at(x, 3) == 1);
    psh_assert(psh_bit_at(x, 4) == 0);
    psh_assert(psh_bit_at(x, 5) == 1);
    psh_assert(psh_bit_at(x, 6) == 1);
    psh_assert(psh_bit_at(x, 7) == 1);
    psh_assert(psh_bit_at(x, 8) == 0);
    psh_assert(psh_bit_at(x, 9) == 1);

    psh_assert(psh_bits_at(x, 0, 1) == psh_bit_at(x, 0));
    psh_assert(psh_bits_at(x, 1, 1) == psh_bit_at(x, 1));
    psh_assert(psh_bits_at(x, 2, 1) == psh_bit_at(x, 2));
    psh_assert(psh_bits_at(x, 4, 1) == psh_bit_at(x, 4));
    psh_assert(psh_bits_at(x, 3, 1) == psh_bit_at(x, 3));
    psh_assert(psh_bits_at(x, 5, 1) == psh_bit_at(x, 5));
    psh_assert(psh_bits_at(x, 6, 1) == psh_bit_at(x, 6));
    psh_assert(psh_bits_at(x, 7, 1) == psh_bit_at(x, 7));
    psh_assert(psh_bits_at(x, 8, 1) == psh_bit_at(x, 8));
    psh_assert(psh_bits_at(x, 9, 1) == psh_bit_at(x, 9));

    psh_assert(psh_bits_at(x, 4, 6) == 0b101110);
    psh_assert(psh_bits_at(x, 0, 4) == 0b1011);
    psh_assert(psh_bits_at(x, 0, 2) == 0b11);
    psh_assert(psh_bits_at(x, 0, 3) == 0b011);

    test_passed();
}

void test_u16_bytes() {
    psh_assert(psh_u16_from_bytes(0xFB, 0xA3) == 0xFBA3);
    psh_assert(psh_u16_from_bytes(0x01, 0x01) == 0x0101);
    psh_assert(psh_u16_from_bytes(0xAB, 0xBA) == 0xABBA);
    psh_assert(psh_u16_from_bytes(0x00, 0x00) == 0x0000);
    psh_assert(psh_u16_from_bytes(0xAA, 0xAA) == 0xAAAA);

    psh_assert(psh_u16_hi(0xFBA3) == 0xFB);
    psh_assert(psh_u16_hi(0x1101) == 0x11);
    psh_assert(psh_u16_hi(0xABBA) == 0xAB);
    psh_assert(psh_u16_hi(0x0000) == 0x00);
    psh_assert(psh_u16_hi(0xAAAA) == 0xAA);

    psh_assert(psh_u16_lo(0xFBA3) == 0xA3);
    psh_assert(psh_u16_lo(0x1101) == 0x01);
    psh_assert(psh_u16_lo(0xABBA) == 0xBA);
    psh_assert(psh_u16_lo(0x0000) == 0x00);
    psh_assert(psh_u16_lo(0xAAAA) == 0xAA);

    test_passed();
}

void test_u16_set_bytes() {
    u16 x = 0x01BB;
    psh_u16_set_hi(x, 0xFA);
    psh_assert(x == 0xFABB);
    psh_u16_set_lo(x, 0xAB);
    psh_assert(x == 0xFAAB);

    psh_assert(psh_u16_from_bytes(0x00, 0x00) == 0x0000);
    psh_assert(psh_u16_from_bytes(0xFF, 0xC3) == 0xFFC3);
    psh_assert(psh_u16_from_bytes(0xAB, 0xCD) == 0xABCD);

    test_passed();
}

void test_u8_bit() {
    u16 x = 0xAB;

    psh_assert(psh_u8_lo(x) == 0x0B);
    psh_assert(psh_u8_hi(x) == 0xA0);

    psh_assert(psh_u8_to_u16_hi(0xFC) == 0xFC00);

    test_passed();
}

void test_sign() {
    psh_assert(!psh_int_opposite_sign(8127, 1209831));
    psh_assert(psh_int_opposite_sign(-8127, 1209831));
    psh_assert(psh_int_opposite_sign(8127, -1209831));

    test_passed();
}

void test_swap() {
    i32 x = -1238;
    i32 y = 2193;
    psh_int_swap(x, y);
    psh_assert((x == 2193) && (y == -1238));

    u32 z = 256;
    u32 w = 102983;
    psh_int_swap(z, w);
    psh_assert((z == 102983) && (w == 256));

    test_passed();
}

void test_bit() {
    test_create_bit();
    test_bit_set_and_clear();
    test_bit_at();
    test_u16_bytes();
    test_u16_set_bytes();
    test_sign();
    test_swap();
}

#if !defined(NOMAIN)
int main() {
    test_bit();
    return 0;
}
#endif
