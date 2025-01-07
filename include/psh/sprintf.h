/// ------------------------------------------------------------------------------------------------
/// Disclaimer: This file is part of the Presheaf library and is a modified version of the original
///             library.
/// ------------------------------------------------------------------------------------------------
/// stb_sprintf - v1.10 - A public domain snprintf() implementation.
/// Originally written by Jeff Roberts / RAD Game Tools, 2015/10/20.
///
/// Contributors:
///     Fabian "ryg" Giesen (reformatting)
///     github:aganm (attribute format)
/// Contributors (bugfixes):
///     github:d26435
///     github:trex78
///     github:account-login
///     Jari Komppa (SI suffixes)
///     Rohit Nirmal
///     Marcin Wojdyr
///     Leonard Ritter
///     Stefano Zanotti
///     Adam Allison
///     Arvid Gerstmann
///     Markus Kolb
/// ------------------------------------------------------------------------------------------------
///                     Public Domain (www.unlicense.org)
///
/// This is free and unencumbered software released into the public domain.
/// Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
/// software, either in source code form or as a compiled binary, for any purpose,
/// commercial or non-commercial, and by any means.
/// In jurisdictions that recognize copyright laws, the author or authors of this
/// software dedicate any and all copyright interest in the software to the public
/// domain. We make this dedication for the benefit of the public at large and to
/// the detriment of our heirs and successors. We intend this dedication to be an
/// overt act of relinquishment in perpetuity of all present and future rights to
/// this software under copyright law.
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
/// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
/// Upstream source: http://github.com/nothings/stb
/// ------------------------------------------------------------------------------------------------
/// An sprintf replacement:
/// =======================
///
/// Allowed types:  sc uidBboXx p AaGgEef n
/// Lengths:        hh h ll j z t I64 I32 I
///
/// This is a full sprintf replacement that supports everything that
/// the C runtime sprintfs support, including float/double, 64-bit integers,
/// hex floats, field parameters (%*.*d stuff), length reads backs, etc.
///
/// Why would you need this if sprintf already exists?  Well, first off,
/// it's *much* faster (see below). It's also much smaller than the CRT
/// versions code-space-wise. We've also added some simple improvements
/// that are super handy (commas in thousands, callbacks at buffer full,
/// for example). Finally, the format strings for MSVC and GCC differ
/// for 64-bit integers (among other small things), so this lets you use
/// the same format strings in cross platform code.
///
/// It only uses va_args macros from the C runtime to do it's work. It
/// does cast doubles to S64s and shifts and divides U64s, which does
/// drag in CRT code on most platforms.
///
/// It compiles to roughly 8K with float support, and 4K without.
/// As a comparison, when using MSVC static libs, calling sprintf drags
/// in 16K.
///
/// API:
/// ====
///
/// int stbsp_sprintf( char * buf, char const * fmt, ... )
/// int stbsp_snprintf( char * buf, int count, char const * fmt, ... )
///   Convert an arg list into a buffer.  stbsp_snprintf always returns
///   a zero-terminated string (unlike regular snprintf).
///
/// int stbsp_vsprintf( char * buf, char const * fmt, va_list va )
/// int stbsp_vsnprintf( char * buf, int count, char const * fmt, va_list va )
///   Convert a va_list arg list into a buffer.  stbsp_vsnprintf always returns
///   a zero-terminated string (unlike regular snprintf).
///
/// int stbsp_vsprintfcb( STBSP_SPRINTFCB * callback, void * user, char * buf, char const * fmt, va_list va )
///     typedef char * STBSP_SPRINTFCB( char const * buf, void * user, int len );
///   Convert into a buffer, calling back every STB_SPRINTF_MIN chars.
///   Your callback can then copy the chars out, print them or whatever.
///   This function is actually the workhorse for everything else.
///   The buffer you pass in must hold at least STB_SPRINTF_MIN characters.
///   You return the next buffer to use or 0 to stop converting
///
/// void stbsp_set_separators( char comma, char period )
///   Set the comma and period characters to use.
///
/// FLOATS/DOUBLES:
/// ===============
///
/// This code uses a internal float->ascii conversion method that uses
/// doubles with error correction (double-doubles, for ~105 bits of
/// precision).  This conversion is round-trip perfect - that is, an atof
/// of the values output here will give you the bit-exact double back.
///
/// One difference is that our insignificant digits will be different than
/// with MSVC or GCC (but they don't match each other either).  We also
/// don't attempt to find the minimum length matching float (pre-MSVC15
/// doesn't either).
///
/// If you don't need float or doubles at all, define STB_SPRINTF_NOFLOAT
/// and you'll save 4K of code space.
///
/// 64-BIT INTS:
/// ============
///
/// This library also supports 64-bit integers and you can use MSVC style or
/// GCC style indicators (%I64d or %lld).  It supports the C99 specifiers
/// for size_t and ptr_diff_t (%jd %zd) as well.
///
/// EXTRAS:
/// =======
///
/// Like some GCCs, for integers and floats, you can use a ' (single quote)
/// specifier and commas will be inserted on the thousands: "%'d" on 12345
/// would print 12,345.
///
/// For integers and floats, you can use a "$" specifier and the number
/// will be converted to float and then divided to get kilo, mega, giga or
/// tera and then printed, so "%$d" 1000 is "1.0 k", "%$.2d" 2536000 is
/// "2.53 M", etc. For byte values, use two $:s, like "%$$d" to turn
/// 2536000 to "2.42 Mi". If you prefer JEDEC suffixes to SI ones, use three
/// $:s: "%$$$d" -> "2.42 M". To remove the space between the number and the
/// suffix, add "_" specifier: "%_$d" -> "2.53M".
///
/// In addition to octal and hexadecimal conversions, you can print
/// integers in binary: "%b" for 256 would print 100.
///
/// PERFORMANCE vs MSVC 2008 32-/64-bit (GCC is even slower than MSVC):
/// ===================================================================
///
/// "%d" across all 32-bit ints (4.8x/4.0x faster than 32-/64-bit MSVC)
/// "%24d" across all 32-bit ints (4.5x/4.2x faster)
/// "%x" across all 32-bit ints (4.5x/3.8x faster)
/// "%08x" across all 32-bit ints (4.3x/3.8x faster)
/// "%f" across e-10 to e+10 floats (7.3x/6.0x faster)
/// "%e" across e-10 to e+10 floats (8.1x/6.0x faster)
/// "%g" across e-10 to e+10 floats (10.0x/7.1x faster)
/// "%f" for values near e-300 (7.9x/6.5x faster)
/// "%f" for values near e+300 (10.0x/9.1x faster)
/// "%e" for values near e-300 (10.1x/7.0x faster)
/// "%e" for values near e+300 (9.2x/6.0x faster)
/// "%.320f" for values near e-300 (12.6x/11.2x faster)
/// "%a" for random values (8.6x/4.3x faster)
/// "%I64d" for 64-bits with 32-bit values (4.8x/3.4x faster)
/// "%I64d" for 64-bits > 32-bit values (4.9x/5.5x faster)
/// "%s%s%s" for 64 char strings (7.1x/7.3x faster)
/// "...512 char string..." ( 35.0x/32.5x faster!)
/// ------------------------------------------------------------------------------------------------

#pragma once

#include <stdarg.h>
#include <psh/core.hpp>

using StbspSprintfCallback = char*(const char* buf, void* user, i32 len);

#ifndef STB_SPRINTF_DECORATE
#    define STB_SPRINTF_DECORATE(name) stbsp_##name  // define this before including if you want to change the names
#endif

extern "C" i32 STB_SPRINTF_DECORATE(vsprintf)(char* buf, cstring fmt, va_list va);
extern "C" i32 STB_SPRINTF_DECORATE(vsnprintf)(char* buf, i32 count, cstring fmt, va_list va);
extern "C" psh_attr_fmt(2) i32 STB_SPRINTF_DECORATE(sprintf)(char* buf, cstring fmt, ...);
extern "C" psh_attr_fmt(3) i32 STB_SPRINTF_DECORATE(snprintf)(char* buf, i32 count, cstring fmt, ...);

extern "C" i32  STB_SPRINTF_DECORATE(vsprintfcb)(StbspSprintfCallback* callback, void* user, char* buf, cstring fmt, va_list va);
