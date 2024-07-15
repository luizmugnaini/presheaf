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
/// Description: Bit twiddling utilities. Implemented as macros to provide speed.
/// Author: Luiz G. Mugnaini A. <luizmugnaini@gmail.com>

#pragma once

#include <limits.h>

// -----------------------------------------------------------------------------
// - Bit Manipulations -
// -----------------------------------------------------------------------------
// Note: The bit indexing count starts at zero for all macros.

/// Get the number of bits that compose a given value.
#define psh_bit_count(val) (CHAR_BIT * sizeof(decltype(val)))

/// Get the number of bits that compose a given type.
#define psh_bit_count_type(T) (CHAR_BIT * sizeof(T))

/// Get the number of type T whose n-th bit is set to 1 and all other bits are 0.
#define psh_bit(T, n) static_cast<T>(1 << (n))

/// Get the number of type T whose n-th bit is set to 0 and all other bits are 1.
#define psh_not_bit(T, n) \
    static_cast<T>(       \
        ~(static_cast<T>(1 << (n))) & static_cast<T>((1ULL << psh_bit_count_type(T)) - 1))

/// Get the number whose first `count` bits are 1's.
#define psh_bit_ones(count) ((1ULL << (count)) - 1)

/// Set the n-th bit of a variable to 1.
#define psh_bit_set(var, n)                 \
    do {                                    \
        var |= psh_bit(decltype(var), (n)); \
    } while (0)

/// Set the n-th bit of a variable to 0.
#define psh_bit_clear(var, n)                   \
    do {                                        \
        var &= psh_not_bit(decltype(var), (n)); \
    } while (0)

/// Set the n-th bit to 1 if the condition passes, otherwise set the bit to 0.
#define psh_bit_set_or_clear_if(var, n, cond)                                       \
    do {                                                                            \
        decltype(var) psh_var_mask_ = psh_bit(decltype(var), (n));                  \
        var = ((var) & ~psh_var_mask_) | (-static_cast<int>(cond) & psh_var_mask_); \
    } while (0)

/// Get the value of the n-th bit of given value.
#define psh_bit_at(val, n) (((val) >> (n)) & 0b1)

/// Get `count` bits from a number, starting at position `pos`.
#define psh_bits_at(val, pos, count) (((val) >> pos) & ((1ULL << (count)) - 1))

/// Test a bit: true if the bit at the given position is set, false otherwise.
#define psh_test_bit_at(val, n) static_cast<bool>(psh_bit_at((val), (n)))

// -----------------------------------------------------------------------------
// - Word (16-bit) manipulations -
// -----------------------------------------------------------------------------

/// Construct a word with the pair (high byte, low byte).
#define psh_u16_from_bytes(hi_u8, lo_u8) \
    (static_cast<unsigned short>((hi_u8) << 8) | static_cast<unsigned short>((lo_u8)))

/// Get the value of the high byte of a word.
#define psh_u16_hi(val_u16) ((val_u16) >> 8)

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

// -----------------------------------------------------------------------------
// - Byte manipulations -
// -----------------------------------------------------------------------------

/// Low nibble of a byte.
#define psh_u8_lo(val_u8) (0x0F & (val_u8))

/// High nibble of a byte.
#define psh_u8_hi(val_u8) ((val_u8) >> 4)

/// Make a byte from a pair of nibbles.
#define psh_u8_from_nibbles(hi_nib, lo_nib) (static_cast<unsigned char>(((hi_nib) << 4) | lo_nib))

/// Transform a byte into the high byte of a word.
#define psh_u8_to_u16_hi(val_u8) (static_cast<unsigned short>((val_u8) << 8))

// -----------------------------------------------------------------------------
// - Integer manipulations -
// -----------------------------------------------------------------------------

/// Check if a given pair of integers has the same sign.
#define psh_int_opposite_sign(a, b) (((a) ^ (b)) < 0)

/// Swap the value of two variables without branching.
#define psh_int_swap(a, b)                                                          \
    do {                                                                            \
        (void)(((a) == (b)) || ((((a) ^ (b)) && ((b) ^= (a) ^= (b), (a) ^= (b))))); \
    } while (0)

/// Rotate right by `n` digits.
#define psh_int_rotr(val, n) \
    (static_cast<decltype(val)>((val >> (n)) | (val << (psh_bit_count(val) - (n)))))

/// Rotate left by `n` digits.
#define psh_int_rotl(val, n) \
    (static_cast<decltype(val)>((val << (n)) | (val >> (psh_bit_count(val) - (n)))))
