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

#define psh_bit(n) (1 << n)

#define psh_not_bit(n) (~(1 << n))

#define psh_bit_set(var, bit_pos) \
    do {                          \
        var |= (1 << n)           \
    } while (0)

#define psh_bit_set_or_clear_if(var, bit_pos, cond)                    \
    do {                                                               \
        var ^= ((var ^ (-static_cast<int>(cond))) & psh_bit(bit_pos)); \
    } while (0)

#define psh_bit_at(val, bit_pos) (((val) >> (bit_pos)) & 0b1)

////////////////////////////////////////////////////////////////////////////////
// Word manipulations.
////////////////////////////////////////////////////////////////////////////////

#define psh_u16_from_bytes(hi_u8, lo_u8) \
    (static_cast<unsigned short>((hi_u8) << 8) | static_cast<unsigned short>((lo_u8)))

#define psh_u16_lo(val_u16) (0x00FF & (val_u16))

#define psh_u16_hi(val_u16) (0xFF00 & (val_u16))

#define psh_u16_set_hi(var_u16, val_u8)                                                       \
    do {                                                                                      \
        unsigned short psh_var_lo_ = (var_u16) & 0x00FF;                                      \
        var_u16                    = static_cast<unsigned short>(val_u8 << 8) | psh_var_lo__; \
    } while (0)

////////////////////////////////////////////////////////////////////////////////
// Byte manipulations.
////////////////////////////////////////////////////////////////////////////////

#define psh_u8_low_nibble(val_u8) (0x0F & (val_u8))

#define psh_u8_high_nibble(val_u8) (0xF0 & (val_u8))

#define psh_u8_to_u16_hi(val_u8) static_cast<unsigned short>((val_u8) << 8)
