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
/// Description: Bit twiddling utilities.
/// Author: Luiz G. Mugnaini A. <luizmuganini@gmail.com>

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Bit manipulations.
////////////////////////////////////////////////////////////////////////////////

/// Get the number whose n-th bit is set to 1 and all other bits are 0.
#define psh_bit(n) (1 << (n))

/// Get the number whose n-th bit is set to 0 and all other bits are 1.
#define psh_not_bit(n) (~(1 << (n)))

/// Set the n-th bit of a variable to 1.
#define psh_bit_set(var, n) \
    do {                    \
        var |= (1 << (n));  \
    } while (0)

/// Conditionally set (1) or clear (0) the n-th bit of a variable, without branching.
#define psh_bit_set_or_clear_if(var, n, cond)                    \
    do {                                                         \
        var ^= ((var ^ (-static_cast<int>(cond))) & (1 << (n))); \
    } while (0)

/// Get the value of the n-th bit of given value.
#define psh_bit_at(val, n) (((val) >> (n)) & 0b1)

////////////////////////////////////////////////////////////////////////////////
// Word manipulations.
////////////////////////////////////////////////////////////////////////////////

/// Construct a word with the pair (high byte, low byte).
#define psh_u16_from_bytes(hi_u8, lo_u8) \
    (static_cast<unsigned short>((hi_u8) << 8) | static_cast<unsigned short>((lo_u8)))

/// Get the value of the high byte of a word.
#define psh_u16_hi(val_u16) (0xFF00 & (val_u16))

/// Get the value of the low byte of a word.
#define psh_u16_lo(val_u16) (0x00FF & (val_u16))

/// Set the value of the high byte of a word.
#define psh_u16_set_hi(var_u16, hi_u8)                                                      \
    do {                                                                                    \
        unsigned short psh_var_lo_ = (var_u16) & 0x00FF;                                    \
        var_u16                    = static_cast<unsigned short>(hi_u8 << 8) | psh_var_lo_; \
    } while (0)

/// Set the value of the low byte of a word.
#define psh_u16_set_lo(var_u16, lo_u8)                                                 \
    do {                                                                               \
        unsigned short psh_var_hi_ = (var_u16) & 0xFF00;                               \
        var_u16                    = psh_var_hi_ | static_cast<unsigned short>(lo_u8); \
    } while (0)

////////////////////////////////////////////////////////////////////////////////
// Byte manipulations.
////////////////////////////////////////////////////////////////////////////////

/// Low nibble of a byte.
#define psh_u8_lo(val_u8) (0x0F & (val_u8))

/// High nibble of a byte.
#define psh_u8_hi(val_u8) (0xF0 & (val_u8))

/// Transform a byte into the high byte of a word.
#define psh_u8_to_u16_hi(val_u8) static_cast<unsigned short>((val_u8) << 8)

////////////////////////////////////////////////////////////////////////////////
// Integer manipulations.
////////////////////////////////////////////////////////////////////////////////

/// Check if a given pair of integers has the same sign.
#define psh_int_opposite_sign(a, b) (((a) ^ (b)) < 0)

////////////////////////////////////////////////////////////////////////////////
// Variable manipulations.
////////////////////////////////////////////////////////////////////////////////

/// Swap the value of two variables without branching.
#define psh_swap(a, b)                                                              \
    do {                                                                            \
        (void)(((a) == (b)) || ((((a) ^ (b)) && ((b) ^= (a) ^= (b), (a) ^= (b))))); \
    } while (0)
