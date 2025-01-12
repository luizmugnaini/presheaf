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
///
/// ------------------------------------------------------------------------------------------------
///
/// Disclaimer: This file is part of the Presheaf library and is a highly modified version of the
///             original stb_sprintf library code. As the original, we still respect its public
///             domain license. Note, however, that the Presheaf code used here isn't licensed under
///             public domain.
///
/// ------------------------------------------------------------------------------------------------
///
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
/// FLOATS/DOUBLES:
/// ===============
///
/// This code uses a internal float->ascii conversion method that uses doubles with error correction
/// (double-doubles, for ~105 bits of precision).  This conversion is round-trip perfect - that is,
/// an atof of the values output here will give you the bit-exact double back.
///
/// One difference is that our insignificant digits will be different than with MSVC or GCC (but
/// they don't match each other either).  We also don't attempt to find the minimum length matching
/// float (pre-MSVC15 doesn't either).
///
/// If you don't need float or doubles at all, define STB_SPRINTF_NOFLOAT and you'll save 4K of code
/// space.
///
/// 64-BIT INTS:
/// ============
///
/// This library also supports 64-bit integers and you can use MSVC style or GCC style indicators
/// (%I64d or %lld).  It supports the C99 specifiers for size_t and ptr_diff_t (%jd %zd) as well.
///
/// EXTRAS:
/// =======
///
/// Like some GCCs, for integers and floats, you can use a ' (single quote) specifier and commas
/// will be inserted on the thousands: "%'d" on 12345 would print 12,345.
///
/// For integers and floats, you can use a "$" specifier and the number will be converted to float
/// and then divided to get kilo, mega, giga or tera and then printed, so "%$d" 1000 is "1.0 k",
/// "%$.2d" 2536000 is "2.53 M", etc. For byte values, use two $:s, like "%$$d" to turn 2536000 to
/// "2.42 Mi". If you prefer JEDEC suffixes to SI ones, use three $:s: "%$$$d" -> "2.42 M". To
/// remove the space between the number and the suffix, add "_" specifier: "%_$d" -> "2.53M".
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
///
/// ------------------------------------------------------------------------------------------------
///
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
///
/// ------------------------------------------------------------------------------------------------

#include "psh_core.hpp"
#include "psh_platform.hpp"

#if PSH_COMPILER_CLANG
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wcast-align"
#    pragma clang diagnostic ignored "-Wcast-qual"
#    pragma clang diagnostic ignored "-Wold-style-cast"
#    pragma clang diagnostic ignored "-Wconditional-uninitialized"
#endif

#if !defined(PSH_IMPL_MIN_LENGTH_PER_CALLBACK)
#    define PSH_IMPL_MIN_LENGTH_PER_CALLBACK 512
#endif

namespace psh::impl {
    psh_internal constexpr i32 SPECIAL = 0x7000;

    psh_internal constexpr char PERIOD = '.';
    psh_internal constexpr char COMMA  = ',';

    psh_internal constexpr struct {
        i16  unused_16_bits;  // force next field to be 2-byte aligned
        char pair[201];
    } DIGIT_PAIR = {
        0,
        "00010203040506070809101112131415161718192021222324"
        "25262728293031323334353637383940414243444546474849"
        "50515253545556575859606162636465666768697071727374"
        "75767778798081828384858687888990919293949596979899",
    };

    psh_internal constexpr u32 LEFTJUST       = 1;
    psh_internal constexpr u32 LEADINGPLUS    = 2;
    psh_internal constexpr u32 LEADINGSPACE   = 4;
    psh_internal constexpr u32 LEADING_0X     = 8;
    psh_internal constexpr u32 LEADINGZERO    = 16;
    psh_internal constexpr u32 INTMAX         = 32;
    psh_internal constexpr u32 TRIPLET_COMMA  = 64;
    psh_internal constexpr u32 NEGATIVE       = 128;
    psh_internal constexpr u32 METRIC_SUFFIX  = 256;
    psh_internal constexpr u32 HALFWIDTH      = 512;
    psh_internal constexpr u32 METRIC_NOSPACE = 1024;
    psh_internal constexpr u32 METRIC_1024    = 2048;
    psh_internal constexpr u32 METRIC_JEDEC   = 4096;

    psh_internal constexpr f64 BOT[23] = {
        1e+000,
        1e+001,
        1e+002,
        1e+003,
        1e+004,
        1e+005,
        1e+006,
        1e+007,
        1e+008,
        1e+009,
        1e+010,
        1e+011,
        1e+012,
        1e+013,
        1e+014,
        1e+015,
        1e+016,
        1e+017,
        1e+018,
        1e+019,
        1e+020,
        1e+021,
        1e+022,
    };
    psh_internal constexpr f64 NEGATIVE_BOT[22] = {
        1e-001,
        1e-002,
        1e-003,
        1e-004,
        1e-005,
        1e-006,
        1e-007,
        1e-008,
        1e-009,
        1e-010,
        1e-011,
        1e-012,
        1e-013,
        1e-014,
        1e-015,
        1e-016,
        1e-017,
        1e-018,
        1e-019,
        1e-020,
        1e-021,
        1e-022,
    };
    psh_internal constexpr f64 NEGATIVE_BOT_ERROR[22] = {
        -5.551115123125783e-018,
        -2.0816681711721684e-019,
        -2.0816681711721686e-020,
        -4.7921736023859299e-021,
        -8.1803053914031305e-022,
        4.5251888174113741e-023,
        4.5251888174113739e-024,
        -2.0922560830128471e-025,
        -6.2281591457779853e-026,
        -3.6432197315497743e-027,
        6.0503030718060191e-028,
        2.0113352370744385e-029,
        -3.0373745563400371e-030,
        1.1806906454401013e-032,
        -7.7705399876661076e-032,
        2.0902213275965398e-033,
        -7.1542424054621921e-034,
        -7.1542424054621926e-035,
        2.4754073164739869e-036,
        5.4846728545790429e-037,
        9.2462547772103625e-038,
        -4.8596774326570872e-039,
    };
    psh_internal constexpr f64 TOP[13] = {
        1e+023,
        1e+046,
        1e+069,
        1e+092,
        1e+115,
        1e+138,
        1e+161,
        1e+184,
        1e+207,
        1e+230,
        1e+253,
        1e+276,
        1e+299,
    };
    psh_internal constexpr f64 NEGATIVE_TOP[13] = {
        1e-023,
        1e-046,
        1e-069,
        1e-092,
        1e-115,
        1e-138,
        1e-161,
        1e-184,
        1e-207,
        1e-230,
        1e-253,
        1e-276,
        1e-299,
    };
    psh_internal constexpr f64 TOP_ERROR[13] = {
        8388608,
        6.8601809640529717e+028,
        -7.253143638152921e+052,
        -4.3377296974619174e+075,
        -1.5559416129466825e+098,
        -3.2841562489204913e+121,
        -3.7745893248228135e+144,
        -1.7356668416969134e+167,
        -3.8893577551088374e+190,
        -9.9566444326005119e+213,
        6.3641293062232429e+236,
        -5.2069140800249813e+259,
        -5.2504760255204387e+282,
    };
    psh_internal constexpr f64 NEGATIVE_TOP_ERROR[13] = {
        3.9565301985100693e-040,
        -2.299904345391321e-063,
        3.6506201437945798e-086,
        1.1875228833981544e-109,
        -5.0644902316928607e-132,
        -6.7156837247865426e-155,
        -2.812077463003139e-178,
        -5.7778912386589953e-201,
        7.4997100559334532e-224,
        -4.6439668915134491e-247,
        -6.3691100762962136e-270,
        -9.436808465446358e-293,
        8.0970921678014997e-317,
    };
    psh_internal constexpr u64 POW_TEN[20] = {
        1,
        10,
        100,
        1000,
        10000,
        100000,
        1000000,
        10000000,
        100000000,
        1000000000,
        10000000000,
        100000000000,
        1000000000000,
        10000000000000,
        100000000000000,
        1000000000000000,
        10000000000000000ull,
        100000000000000000ull,
        1000000000000000000ull,
        10000000000000000000ull,
    };
    psh_internal constexpr u64 TEN_TO_19TH = 1000000000000000000;

    struct SprintfContext {
        char* buf;
        i32   count;
        i32   length;
        char  tmp[PSH_IMPL_MIN_LENGTH_PER_CALLBACK];
    };

    using StringFormatCallbackFunction = char*(cstring buf, void* user, i32 len);

    psh_internal void lead_sign(u32 fl, char* sign) psh_no_except {
        sign[0] = 0;
        if (fl & NEGATIVE) {
            sign[0] = 1;
            sign[1] = '-';
        } else if (fl & LEADINGSPACE) {
            sign[0] = 1;
            sign[1] = ' ';
        } else if (fl & LEADINGPLUS) {
            sign[0] = 1;
            sign[1] = '+';
        }
    }

    psh_internal char* count_clamp_callback(cstring buf, void* user, i32 len) psh_no_except {
        SprintfContext* c = (SprintfContext*)user;
        psh_discard_value(buf);

        c->length += len;
        return c->tmp;  // go direct into buffer if you can
    }

    // -------------------------------------------------------------------------------------------------
    // Low level f32 utility functions
    // -------------------------------------------------------------------------------------------------

// copies d to bits w/ strict aliasing (this compiles to nothing on /Ox)
#define psh_impl_copy_fp(dest, src)                 \
    do {                                            \
        for (i32 cn = 0; cn < 8; ++cn) {            \
            ((char*)&dest)[cn] = ((char*)&src)[cn]; \
        }                                           \
    } while (0)

#define psh_impl_ddmulthi(oh, ol, xh, yh)                                 \
    do {                                                                  \
        oh = xh * yh;                                                     \
        i64 bt;                                                           \
        psh_impl_copy_fp(bt, xh);                                         \
        bt &= ((~(u64)0) << 27);                                          \
        f64 ahi = 0;                                                      \
        psh_impl_copy_fp(ahi, bt);                                        \
        f64 alo = xh - ahi;                                               \
        psh_impl_copy_fp(bt, yh);                                         \
        bt &= ((~(u64)0) << 27);                                          \
        f64 bhi = 0;                                                      \
        psh_impl_copy_fp(bhi, bt);                                        \
        f64 blo = yh - bhi;                                               \
        ol      = ((ahi * bhi - oh) + ahi * blo + alo * bhi) + alo * blo; \
    } while (0)
#define psh_impl_dd_to_s64(ob, xh, xl)         \
    do {                                       \
        f64 ahi = 0;                           \
        ob      = (i64)xh;                     \
        f64 vh  = (f64)ob;                     \
        ahi     = (xh - vh);                   \
        f64 t   = (ahi - xh);                  \
        f64 alo = (xh - (ahi - t)) - (vh + t); \
        ob += (i64)(ahi + alo + xl);           \
    } while (0)
#define psh_impl_ddrenorm(oh, ol) \
    do {                          \
        f64 s = oh + ol;          \
        ol    = ol - (s - oh);    \
        oh    = s;                \
    } while (0)
#define psh_impl_ddmultlo(oh, ol, xh, xl, yh, yl) \
    do {                                          \
        ol = ol + (xh * yl + xl * yh);            \
    } while (0)
#define psh_impl_ddmultlos(oh, ol, xh, yl) \
    do {                                   \
        ol = ol + (xh * yl);               \
    } while (0)

    // get f32 info
    psh_internal i32 real_to_parts(i64* bits, i32* expo, f64 value) psh_no_except {
        f64 d;
        i64 b = 0;

        // load value and round at the frac_digits
        d = value;

        psh_impl_copy_fp(b, d);

        *bits = (u64)b & ((((u64)1) << 52) - 1);
        *expo = (i32)(((b >> 52) & 2047) - 1023);

        return (i32)((u64)b >> 63);
    }

    // power can be -323 to +350
    psh_internal void raise_to_power10(f64* ohi, f64* olo, f64 d, i32 power) psh_no_except {
        f64 ph, pl;
        if ((power >= 0) && (power <= 22)) {
            psh_impl_ddmulthi(ph, pl, d, BOT[power]);
        } else {
            i32 e, et, eb;
            f64 p2h, p2l;

            e = power;
            if (power < 0) {
                e = -e;
            }
            et = (e * 0x2c9) >> 14;  // %23
            if (et > 13) {
                et = 13;
            }
            eb = e - (et * 23);

            ph = d;
            pl = 0.0;
            if (power < 0) {
                if (eb) {
                    --eb;
                    psh_impl_ddmulthi(ph, pl, d, NEGATIVE_BOT[eb]);
                    psh_impl_ddmultlos(ph, pl, d, NEGATIVE_BOT_ERROR[eb]);
                }
                if (et) {
                    psh_impl_ddrenorm(ph, pl);
                    --et;
                    psh_impl_ddmulthi(p2h, p2l, ph, NEGATIVE_TOP[et]);
                    psh_impl_ddmultlo(p2h, p2l, ph, pl, NEGATIVE_TOP[et], NEGATIVE_TOP_ERROR[et]);
                    ph = p2h;
                    pl = p2l;
                }
            } else {
                if (eb) {
                    e = eb;
                    if (eb > 22) {
                        eb = 22;
                    }
                    e -= eb;
                    psh_impl_ddmulthi(ph, pl, d, BOT[eb]);
                    if (e) {
                        psh_impl_ddrenorm(ph, pl);
                        psh_impl_ddmulthi(p2h, p2l, ph, BOT[e]);
                        psh_impl_ddmultlos(p2h, p2l, BOT[e], pl);
                        ph = p2h;
                        pl = p2l;
                    }
                }
                if (et) {
                    psh_impl_ddrenorm(ph, pl);
                    --et;
                    psh_impl_ddmulthi(p2h, p2l, ph, TOP[et]);
                    psh_impl_ddmultlo(p2h, p2l, ph, pl, TOP[et], TOP_ERROR[et]);
                    ph = p2h;
                    pl = p2l;
                }
            }
        }
        psh_impl_ddrenorm(ph, pl);
        *ohi = ph;
        *olo = pl;
    }

    // Given an f32 value, returns the significant bits in bits, and the position of the decimal point
    // in decimal_pos.  +/-INF and NAN are specified by special values returned in the decimal_pos
    // parameter. frac_digits is absolute normally, but if you want from first significant digits (got
    // %g and %e), or in 0x80000000.
    psh_internal i32 real_to_str(
        cstring* start,
        u32*     len,
        char*    out,
        i32*     decimal_pos,
        f64      value,
        u32      frac_digits) psh_no_except {
        f64 d    = value;
        i64 bits = 0;
        psh_impl_copy_fp(bits, d);
        i32 expo = (i32)((bits >> 52) & 2047);
        i32 ng   = (i32)((u64)bits >> 63);
        if (ng) {
            d = -d;
        }

        // is nan or inf?
        if (expo == 2047) {
            *start       = ((u64)bits & ((((u64)1) << 52) - 1)) ? "NaN" : "Inf";
            *decimal_pos = SPECIAL;
            *len         = 3;
            return ng;
        }

        // is zero or denormal
        if (expo == 0) {
            // do zero
            if (((u64)bits << 1) == 0) {
                *decimal_pos = 1;
                *start       = out;
                out[0]       = '0';
                *len         = 1;
                return ng;
            }
            // find the right expo for denormals
            {
                i64 v = ((u64)1) << 51;
                while ((bits & v) == 0) {
                    --expo;
                    v >>= 1;
                }
            }
        }

        // find the decimal exponent as well as the decimal bits of the value
        i32 tens, e;
        {
            // log10 estimate - very specifically tweaked to hit or undershoot by no more than 1 of log10 of all expos 1..2046
            tens = expo - 1023;
            tens = (tens < 0) ? ((tens * 617) / 2048) : (((tens * 1233) / 4096) + 1);

            // move the significant bits into position and stick them into an int
            f64 ph, pl;
            raise_to_power10(&ph, &pl, d, 18 - tens);

            // get full as much precision from f64-f64 as possible
            psh_impl_dd_to_s64(bits, ph, pl);

            // check if we undershot
            if (((u64)bits) >= TEN_TO_19TH) {
                ++tens;
            }
        }

        // now do the rounding in integer land
        frac_digits = (frac_digits & 0x80000000) ? ((frac_digits & 0x7ffffff) + 1) : ((u32)tens + frac_digits);
        if (frac_digits < 24) {
            u32 dg = 1;
            if ((u64)bits >= POW_TEN[9]) {
                dg = 10;
            }
            while ((u64)bits >= POW_TEN[dg]) {
                ++dg;
                if (dg == 20) {
                    goto noround;
                }
            }
            if (frac_digits < dg) {
                // add 0.5 at the right position and round
                e = (i32)dg - (i32)frac_digits;
                if ((u32)e >= 24) {
                    goto noround;
                }
                u64 r = POW_TEN[e];
                bits  = bits + (i64)(r / 2);
                if ((u64)bits >= POW_TEN[dg]) {
                    ++tens;
                }
                bits /= r;
            }
        noround:;
        }

        // kill long trailing runs of zeros
        if (bits) {
            u32 n;
            for (;;) {
                if (bits <= 0xffffffff) {
                    break;
                }
                if (bits % 1000) {
                    goto donez;
                }
                bits /= 1000;
            }
            n = (u32)bits;
            while ((n % 1000) == 0) {
                n /= 1000;
            }
            bits = n;
        donez:;
        }

        // convert to string
        out += 64;
        e = 0;
        for (;;) {
            u32   n;
            char* o = out - 8;
            // do the conversion in chunks of U32s (avoid most 64-bit divides, worth it, constant denomiators be damned)
            if (bits >= 100000000) {
                n = (u32)(bits % 100000000);
                bits /= 100000000;
            } else {
                n    = (u32)bits;
                bits = 0;
            }
            while (n) {
                out -= 2;
                *(u16*)out = *(u16*)&DIGIT_PAIR.pair[(n % 100) * 2];
                n /= 100;
                e += 2;
            }
            if (bits == 0) {
                if ((e) && (out[0] == '0')) {
                    ++out;
                    --e;
                }
                break;
            }
            while (out != o) {
                --out;
                *out = '0';
                ++e;
            }
        }

        *decimal_pos = tens;
        *start       = out;
        *len         = (u32)e;
        return ng;
    }

    // -------------------------------------------------------------------------------------------------
    // String formatting main implementation.
    // -------------------------------------------------------------------------------------------------

    /// Convert into a buffer, calling back every PSH_IMPL_MIN_LENGTH_PER_CALLBACK chars. Your
    /// callback can then copy the chars out, print them or whatever. This function is actually the
    /// workhorse for everything else. The buffer you pass in must hold at least
    /// PSH_IMPL_MIN_LENGTH_PER_CALLBACK characters. You return the next buffer to use or 0 to stop
    /// converting
    psh_internal psh_attribute_disable_asan i32 string_format_list_with_callback(
        StringFormatCallbackFunction* callback,
        void*                         user,
        char*                         buf,
        cstring                       fmt,
        va_list                       va) psh_no_except {
        // macros for the callback buffer stuff
#define psh_impl_chk_cb_bufL(bytes)                                 \
    do {                                                            \
        i32 len = (i32)(bf - buf);                                  \
        if ((len + (bytes)) >= PSH_IMPL_MIN_LENGTH_PER_CALLBACK) {  \
            tlen += len;                                            \
            if ((bf = buf = callback(buf, user, len)) == nullptr) { \
                goto done;                                          \
            }                                                       \
        }                                                           \
    } while (0)

#define psh_impl_chk_cb_buf(bytes)       \
    do {                                 \
        if (callback) {                  \
            psh_impl_chk_cb_bufL(bytes); \
        }                                \
    } while (0)

// flush if there is even one byte in the buffer
#define psh_impl_flush_cb()                                         \
    do {                                                            \
        psh_impl_chk_cb_bufL(PSH_IMPL_MIN_LENGTH_PER_CALLBACK - 1); \
    } while (0)

#define psh_cb_buf_clamp(cl, v)                                          \
    do {                                                                 \
        cl = v;                                                          \
        if (callback) {                                                  \
            i32 lg = PSH_IMPL_MIN_LENGTH_PER_CALLBACK - (i32)(bf - buf); \
            if (cl > lg) {                                               \
                cl = lg;                                                 \
            }                                                            \
        }                                                                \
    } while (0)

        psh_internal constexpr char hex[]  = "0123456789abcdefxp";
        psh_internal constexpr char hexu[] = "0123456789ABCDEFXP";
        i32                         tlen   = 0;

        char*   bf = buf;
        cstring f  = fmt;
        for (;;) {
            i32 fw, pr, tz;
            u32 fl;

            // fast copy everything up to the next % (or end of string)
            for (;;) {
                while (((uptr)f) & 3) {
                schk1:
                    if (f[0] == '%') {
                        goto scandd;
                    }
                schk2:
                    if (f[0] == 0) {
                        goto endfmt;
                    }
                    psh_impl_chk_cb_buf(1);
                    *bf = f[0];
                    ++bf;
                    ++f;
                }
                for (;;) {
                    // Check if the next 4 bytes contain %(0x25) or end of string.
                    // Using the 'hasless' trick:
                    // https://graphics.stanford.edu/~seander/bithacks.html#HasLessInWord

                    // @NOTE: This can cause a buffer overflow, hence the need to use psh_attribute_disable_asan.
                    u32 v = *(u32*)f;

                    u32 c = (~v) & 0x80808080;
                    if (((v ^ 0x25252525) - 0x01010101) & c) {
                        goto schk1;
                    }
                    if ((v - 0x01010101) & c) {
                        goto schk2;
                    }
                    if (callback) {
                        if ((PSH_IMPL_MIN_LENGTH_PER_CALLBACK - (i32)(bf - buf)) < 4) {
                            goto schk1;
                        }
                    }
                    if (((uptr)bf) & 3) {
                        // @NOTE: These are unaligned memory accesses.
                        bf[0] = f[0];
                        bf[1] = f[1];
                        bf[2] = f[2];
                        bf[3] = f[3];
                    } else {
                        *(u32*)bf = v;
                    }
                    bf += 4;
                    f += 4;
                }
            }
        scandd:

            ++f;

            // ok, we have a percent, read the modifiers first
            fw = 0;
            pr = -1;
            fl = 0;
            tz = 0;

            // flags
            for (;;) {
                switch (f[0]) {
                    // if we have left justify
                    case '-': {
                        fl |= LEFTJUST;
                        ++f;
                        continue;
                    }
                    // if we have leading plus
                    case '+': {
                        fl |= LEADINGPLUS;
                        ++f;
                        continue;
                    }
                    // if we have leading space
                    case ' ': {
                        fl |= LEADINGSPACE;
                        ++f;
                        continue;
                    }
                    // if we have leading 0x
                    case '#': {
                        fl |= LEADING_0X;
                        ++f;
                        continue;
                    }
                    // if we have thousand commas
                    case '\'': {
                        fl |= TRIPLET_COMMA;
                        ++f;
                        continue;
                    }
                    // if we have kilo marker (none->kilo->kibi->jedec)
                    case '$': {
                        if (fl & METRIC_SUFFIX) {
                            if (fl & METRIC_1024) {
                                fl |= METRIC_JEDEC;
                            } else {
                                fl |= METRIC_1024;
                            }
                        } else {
                            fl |= METRIC_SUFFIX;
                        }
                        ++f;
                        continue;
                    }
                    // if we don't want space between metric suffix and number
                    case '_': {
                        fl |= METRIC_NOSPACE;
                        ++f;
                        continue;
                    }
                    // if we have leading zero
                    case '0': {
                        fl |= LEADINGZERO;
                        ++f;
                        goto flags_done;
                    }
                    default: goto flags_done;
                }
            }
        flags_done:

            // get the field width
            if (f[0] == '*') {
                fw = (i32)va_arg(va, u32);
                ++f;
            } else {
                while ((f[0] >= '0') && (f[0] <= '9')) {
                    fw = fw * 10 + f[0] - '0';
                    f++;
                }
            }
            // get the precision
            if (f[0] == '.') {
                ++f;
                if (f[0] == '*') {
                    pr = (i32)va_arg(va, u32);
                    ++f;
                } else {
                    pr = 0;
                    while ((f[0] >= '0') && (f[0] <= '9')) {
                        pr = pr * 10 + f[0] - '0';
                        f++;
                    }
                }
            }

            // handle integer size overrides
            switch (f[0]) {
                // are we halfwidth?
                case 'h': {
                    fl |= HALFWIDTH;
                    ++f;
                    if (f[0] == 'h') {
                        ++f;  // QUARTERWIDTH
                    }
                    break;
                }
                // are we 64-bit (unix style)
                case 'l': {
                    fl |= ((psh_usize_of(long) == 8) ? INTMAX : 0);
                    ++f;
                    if (f[0] == 'l') {
                        fl |= INTMAX;
                        ++f;
                    }
                    break;
                }
                // are we 64-bit on intmax? (c99)
                case 'j': {
                    fl |= (psh_usize_of(usize) == 8) ? INTMAX : 0;
                    ++f;
                    break;
                }
                // are we 64-bit on size_t or ptrdiff_t? (c99)
                case 'z': {
                    fl |= (psh_usize_of(isize) == 8) ? INTMAX : 0;
                    ++f;
                    break;
                }
                case 't': {
                    fl |= (psh_usize_of(isize) == 8) ? INTMAX : 0;
                    ++f;
                    break;
                }
                // are we 64-bit (msft style)
                case 'I': {
                    if ((f[1] == '6') && (f[2] == '4')) {
                        fl |= INTMAX;
                        f += 3;
                    } else if ((f[1] == '3') && (f[2] == '2')) {
                        f += 3;
                    } else {
                        fl |= ((psh_usize_of(void*) == 8) ? INTMAX : 0);
                        ++f;
                    }
                    break;
                }
                default: break;
            }

            constexpr usize NUMSZ = 512;  // big enough for e308 (with commas) or e-307
            char            num[NUMSZ];
            char            lead[8];
            char            tail[8];
            char*           s;
            cstring         h;
            u32             l, n, cs;
            u64             n64;
            f64             fv;
            i32             dp;
            cstring         sn;

            // handle each replacement
            switch (f[0]) {
                case 's': {
                    // get the string
                    s = va_arg(va, char*);
                    if (s == nullptr) {
                        s = (char*)"null";
                    }

                    // get the length
                    sn = s;
                    for (;;) {
                        if ((((uptr)sn) & 3) == 0) {
                            break;
                        }
                    lchk:
                        if (sn[0] == 0) {
                            goto ld;
                        }
                        ++sn;
                    }
                    n = 0xffffffff;
                    if (pr >= 0) {
                        n = (u32)(sn - s);
                        if (n >= (u32)pr) {
                            goto ld;
                        }
                        n = ((u32)pr - n) >> 2;
                    }
                    while (n) {
                        u32 v = *(u32*)sn;
                        if ((v - 0x01010101) & (~v) & 0x80808080UL) {
                            goto lchk;
                        }
                        sn += 4;
                        --n;
                    }
                    goto lchk;
                ld:

                    // get the length, limited to desired precision
                    // always limit to ~0u chars since our counts are 32b
                    l = (u32)(sn - s);
                    if (l > (u32)pr) {
                        l = (u32)pr;
                    }

                    lead[0] = 0;
                    tail[0] = 0;
                    pr      = 0;
                    dp      = 0;
                    cs      = 0;
                    // copy the string in
                    goto scopy;
                }

                    // // @TODO: Presheaf type addition.
                    // // psh::StringView
                    // case 'S': {
                    //     psh::StringView string = va_arg(va, psh::StringView);
                    //
                    //     // Get string length.
                    //     s  = (char*)string.buf;
                    //     sn = (char const*)(string.buf + string.count);
                    //     l  = string.count;
                    //
                    //     // Clamp to precision.
                    //     lead[0] = 0;
                    //     tail[0] = 0;
                    //     pr      = 0;
                    //     dp      = 0;
                    //     cs      = 0;
                    //
                    //     goto scopy;
                    //     }

                case 'c': {  // char
                    // get the character
                    s       = num + NUMSZ - 1;
                    *s      = (char)va_arg(va, i32);
                    l       = 1;
                    lead[0] = 0;
                    tail[0] = 0;
                    pr      = 0;
                    dp      = 0;
                    cs      = 0;
                    goto scopy;
                }
                // weird write-bytes specifier
                case 'n': {
                    i32* d = va_arg(va, i32*);
                    *d     = tlen + (i32)(bf - buf);
                    break;
                }

                case 'A': PSH_FALLTHROUGH;  // hex f32
                case 'a': {                 // hex f32
                    h  = (f[0] == 'A') ? hexu : hex;
                    fv = va_arg(va, f64);

                    if (pr == -1) {
                        pr = 6;  // default is 6
                    }

                    // read the f64 into a string
                    if (real_to_parts((i64*)&n64, &dp, fv)) {
                        fl |= NEGATIVE;
                    }

                    s = num + 64;

                    lead_sign(fl, lead);

                    if (dp == -1023) {
                        dp = (n64) ? -1022 : 0;
                    } else {
                        n64 |= (((u64)1) << 52);
                    }

                    n64 <<= (64 - 56);

                    // add leading chars
                    if (pr < 15) {
                        n64 += ((((u64)8) << 56) >> (pr * 4));
                    }

                    lead[1 + lead[0]] = '0';
                    lead[2 + lead[0]] = 'x';
                    lead[0] += 2;

                    *s = h[(n64 >> 60) & 15];
                    ++s;
                    n64 <<= 4;
                    if (pr) {
                        *s = PERIOD;
                        ++s;
                    }
                    sn = s;

                    // print the bits
                    n = (u32)pr;
                    if (n > 13) {
                        n = 13;
                    }
                    if (pr > (i32)n) {
                        tz = pr - (i32)n;
                    }
                    pr = 0;
                    while (n--) {
                        *s = h[(n64 >> 60) & 15];
                        ++s;
                        n64 <<= 4;
                    }

                    // print the expo
                    tail[1] = h[17];
                    if (dp < 0) {
                        tail[2] = '-';
                        dp      = -dp;
                    } else
                        tail[2] = '+';
                    n       = (dp >= 1000) ? 6 : ((dp >= 100) ? 5 : ((dp >= 10) ? 4 : 3));
                    tail[0] = (char)n;
                    for (;;) {
                        tail[n] = '0' + dp % 10;
                        if (n <= 3) {
                            break;
                        }
                        --n;
                        dp /= 10;
                    }

                    dp = (i32)(s - sn);
                    l  = (u32)(s - (num + 64));
                    s  = num + 64;
                    cs = 1 + (3 << 24);
                    goto scopy;
                }
                case 'G': PSH_FALLTHROUGH;  // f32
                case 'g': {                 // f32
                    h  = (f[0] == 'G') ? hexu : hex;
                    fv = va_arg(va, f64);
                    if (pr == -1) {
                        pr = 6;
                    } else if (pr == 0) {
                        pr = 1;  // default is 6
                    }

                    // read the f64 into a string
                    if (real_to_str(&sn, &l, num, &dp, fv, (u32)(pr - 1) | 0x80000000)) {
                        fl |= NEGATIVE;
                    }

                    // clamp the precision and delete extra zeros after clamp
                    n = (u32)pr;
                    if (l > (u32)pr) {
                        l = (u32)pr;
                    }
                    while ((l > 1) && (pr) && (sn[l - 1] == '0')) {
                        --pr;
                        --l;
                    }

                    // should we use %e
                    if ((dp <= -4) || (dp > (i32)n)) {
                        if (pr > (i32)l) {
                            pr = (i32)l - 1;
                        } else if (pr) {
                            --pr;  // when using %e, there is one digit before the decimal
                        }
                        goto doexpfromg;
                    }
                    // this is the insane action to get the pr to match %g semantics for %f
                    if (dp > 0) {
                        pr = (dp < (i32)l) ? ((i32)l - dp) : 0;
                    } else {
                        pr = -dp + ((pr > (i32)l) ? (i32)l : pr);
                    }
                    goto dof32fromg;
                }
                case 'E': PSH_FALLTHROUGH;  // f32
                case 'e': {                 // f32
                    h  = (f[0] == 'E') ? hexu : hex;
                    fv = va_arg(va, f64);

                    if (pr == -1) {
                        pr = 6;  // default is 6
                    }

                    // read the f64 into a string
                    if (real_to_str(&sn, &l, num, &dp, fv, (u32)pr | 0x80000000)) {
                        fl |= NEGATIVE;
                    }
                doexpfromg:
                    tail[0] = 0;
                    lead_sign(fl, lead);
                    if (dp == SPECIAL) {
                        s  = (char*)sn;
                        cs = 0;
                        pr = 0;
                        goto scopy;
                    }
                    s  = num + 64;
                    // handle leading chars
                    *s = sn[0];
                    ++s;

                    if (pr) {
                        *s = PERIOD;
                        ++s;
                    }

                    // handle after decimal
                    if ((l - 1) > (u32)pr) {
                        l = (u32)(pr + 1);
                    }

                    for (n = 1; n < l; n++) {
                        *s = sn[n];
                        ++s;
                    }

                    // trailing zeros
                    tz      = pr - ((i32)l - 1);
                    pr      = 0;
                    // dump expo
                    tail[1] = h[0xe];
                    dp -= 1;
                    if (dp < 0) {
                        tail[2] = '-';
                        dp      = -dp;
                    } else {
                        tail[2] = '+';
                    }

                    n = (dp >= 100) ? 5 : 4;

                    tail[0] = (char)n;
                    for (;;) {
                        tail[n] = '0' + dp % 10;
                        if (n <= 3) {
                            break;
                        }
                        --n;
                        dp /= 10;
                    }
                    cs = 1 + (3 << 24);  // how many tens
                    goto flt_lead;
                }
                case 'f': {  // f32
                    fv = va_arg(va, f64);
                doaf32:
                    // do kilos
                    if (fl & METRIC_SUFFIX) {
                        f64 divisor = 1000.0;
                        if (fl & METRIC_1024) {
                            divisor = 1024.0;
                        }
                        while (fl < 0x4000000) {
                            if ((fv < divisor) && (fv > -divisor)) {
                                break;
                            }
                            fv /= divisor;
                            fl += 0x1000000;
                        }
                    }

                    if (pr == -1) {
                        pr = 6;  // default is 6
                    }

                    // read the f64 into a string
                    if (real_to_str(&sn, &l, num, &dp, fv, (u32)pr)) {
                        fl |= NEGATIVE;
                    }
                dof32fromg:
                    tail[0] = 0;
                    lead_sign(fl, lead);
                    if (dp == SPECIAL) {
                        s  = (char*)sn;
                        cs = 0;
                        pr = 0;
                        goto scopy;
                    }
                    s = num + 64;

                    // handle the three decimal varieties
                    if (dp <= 0) {
                        i32 i;
                        // handle 0.000*000xxxx
                        *s = '0';
                        ++s;
                        if (pr) {
                            *s = PERIOD;
                            ++s;
                        }
                        n = (u32)(-dp);
                        if ((i32)n > pr) {
                            n = (u32)pr;
                        }
                        i = (i32)n;
                        while (i) {
                            if ((((uptr)s) & 3) == 0) {
                                break;
                            }
                            *s = '0';
                            ++s;
                            --i;
                        }
                        while (i >= 4) {
                            *(u32*)s = 0x30303030;
                            s += 4;
                            i -= 4;
                        }
                        while (i) {
                            *s = '0';
                            ++s;
                            --i;
                        }
                        if ((i32)(l + n) > pr) {
                            l = (u32)(pr - (i32)n);
                        }
                        i = (i32)l;
                        while (i) {
                            *s = *sn;
                            ++s;
                            ++sn;
                            --i;
                        }
                        tz = pr - (i32)(n + l);
                        cs = 1 + (3 << 24);  // how many tens did we write (for commas below)
                    } else {
                        cs = (fl & TRIPLET_COMMA) ? ((600 - (u32)dp) % 3) : 0;
                        if ((u32)dp >= l) {
                            // handle xxxx000*000.0
                            n = 0;
                            for (;;) {
                                if ((fl & TRIPLET_COMMA) && (++cs == 4)) {
                                    cs = 0;
                                    *s = COMMA;
                                    ++s;
                                } else {
                                    *s = sn[n];
                                    ++s;
                                    ++n;
                                    if (n >= l) {
                                        break;
                                    }
                                }
                            }
                            if (n < (u32)dp) {
                                n = (u32)dp - n;
                                if ((fl & TRIPLET_COMMA) == 0) {
                                    while (n) {
                                        if ((((uptr)s) & 3) == 0) {
                                            break;
                                        }
                                        *s = '0';
                                        ++s;
                                        --n;
                                    }
                                    while (n >= 4) {
                                        *(u32*)s = 0x30303030;
                                        s += 4;
                                        n -= 4;
                                    }
                                }
                                while (n) {
                                    if ((fl & TRIPLET_COMMA) && (++cs == 4)) {
                                        cs = 0;
                                        *s = COMMA;
                                        ++s;
                                    } else {
                                        *s = '0';
                                        ++s;
                                        --n;
                                    }
                                }
                            }
                            cs = (u32)((i32)(s - (num + 64)) + (3 << 24));  // cs is how many tens
                            if (pr) {
                                *s = PERIOD;
                                ++s;
                                tz = pr;
                            }
                        } else {
                            // handle xxxxx.xxxx000*000
                            n = 0;
                            for (;;) {
                                if ((fl & TRIPLET_COMMA) && (++cs == 4)) {
                                    cs = 0;
                                    *s = COMMA;
                                    ++s;
                                } else {
                                    *s = sn[n];
                                    ++s;
                                    ++n;
                                    if (n >= (u32)dp)
                                        break;
                                }
                            }
                            cs = (u32)((i32)(s - (num + 64)) + (3 << 24));  // cs is how many tens
                            if (pr) {
                                *s = PERIOD;
                                ++s;
                            }
                            if ((l - (u32)dp) > (u32)pr) {
                                l = (u32)(pr + dp);
                            }
                            while (n < l) {
                                *s = sn[n];
                                ++s;
                                ++n;
                            }
                            tz = pr - ((i32)l - dp);
                        }
                    }
                    pr = 0;

                    // handle k,m,g,t
                    if (fl & METRIC_SUFFIX) {
                        char idx;
                        idx = 1;
                        if (fl & METRIC_NOSPACE) {
                            idx = 0;
                        }
                        tail[0] = idx;
                        tail[1] = ' ';
                        {
                            if (fl >> 24) {  // SI kilo is 'k', JEDEC and SI kibits are 'K'.
                                if (fl & METRIC_1024) {
                                    tail[idx + 1] = "_KMGT"[fl >> 24];
                                } else {
                                    tail[idx + 1] = "_kMGT"[fl >> 24];
                                }
                                idx++;
                                // If printing kibits and not in jedec, add the 'i'.
                                if (fl & METRIC_1024 && !(fl & METRIC_JEDEC)) {
                                    tail[idx + 1] = 'i';
                                    idx++;
                                }
                                tail[0] = idx;
                            }
                        }
                    }

                flt_lead:
                    // get the length that we copied
                    l = (u32)(s - (num + 64));
                    s = num + 64;
                    goto scopy;
                }
                case 'B': PSH_FALLTHROUGH;  // upper binary
                case 'b': {                 // lower binary
                    h       = (f[0] == 'B') ? hexu : hex;
                    lead[0] = 0;
                    if (fl & LEADING_0X) {
                        lead[0] = 2;
                        lead[1] = '0';
                        lead[2] = h[0xb];
                    }
                    l = (8 << 4) | (1 << 8);
                    goto radixnum;
                }
                case 'o': {  // octal
                    h       = hexu;
                    lead[0] = 0;
                    if (fl & LEADING_0X) {
                        lead[0] = 1;
                        lead[1] = '0';
                    }
                    l = (3 << 4) | (3 << 8);
                    goto radixnum;
                }
                case 'p': {  // pointer
                    fl |= (psh_usize_of(void*) == 8) ? INTMAX : 0;
                    pr = psh_usize_of(void*) * 2;
                    fl &= ~LEADINGZERO;  // 'p' only prints the pointer with zeros

                    PSH_FALLTHROUGH;
                }
                case 'X': PSH_FALLTHROUGH;  // upper hex
                case 'x': {                 // lower hex
                    h       = (f[0] == 'X') ? hexu : hex;
                    l       = (4 << 4) | (4 << 8);
                    lead[0] = 0;
                    if (fl & LEADING_0X) {
                        lead[0] = 2;
                        lead[1] = '0';
                        lead[2] = h[16];
                    }
                radixnum:
                    // get the number
                    if (fl & INTMAX) {
                        n64 = va_arg(va, u64);
                    } else {
                        n64 = va_arg(va, u32);
                    }

                    s       = num + NUMSZ;
                    dp      = 0;
                    // clear tail, and clear leading if value is zero
                    tail[0] = 0;
                    if (n64 == 0) {
                        lead[0] = 0;
                        if (pr == 0) {
                            l  = 0;
                            cs = 0;
                            goto scopy;
                        }
                    }
                    // convert to string
                    for (;;) {
                        --s;
                        *s = h[n64 & ((1 << (l >> 8)) - 1)];
                        n64 >>= (l >> 8);
                        if (!((n64) || ((i32)((num + NUMSZ) - s) < pr))) {
                            break;
                        }
                        if (fl & TRIPLET_COMMA) {
                            ++l;
                            if ((l & 15) == ((l >> 4) & 15)) {
                                l &= (u32)(~15);
                                --s;
                                *s = COMMA;
                            }
                        }
                    }
                    // get the tens and the comma pos
                    cs = (u32)((num + NUMSZ) - s) + ((((l >> 4) & 15)) << 24);
                    // get the length that we copied
                    l  = (u32)((num + NUMSZ) - s);
                    // copy it
                    goto scopy;
                }
                case 'u': PSH_FALLTHROUGH;  // unsigned integer
                case 'i': PSH_FALLTHROUGH;  // signed integer
                case 'd': {                 // signed integer
                    // get the integer and abs it
                    if (fl & INTMAX) {
                        i64 i = va_arg(va, i64);
                        n64   = (u64)i;
                        if ((f[0] != 'u') && (i < 0)) {
                            n64 = (u64)(-i);
                            fl |= NEGATIVE;
                        }
                    } else {
                        i32 i = va_arg(va, i32);
                        n64   = (u32)i;
                        if ((f[0] != 'u') && (i < 0)) {
                            n64 = (u32)(-i);
                            fl |= NEGATIVE;
                        }
                    }

                    if (fl & METRIC_SUFFIX) {
                        if (n64 < 1024) {
                            pr = 0;
                        } else if (pr == -1) {
                            pr = 1;
                        }
                        fv = (f64)(i64)n64;
                        goto doaf32;
                    }

                    // convert to string
                    s = num + NUMSZ;
                    l = 0;

                    for (;;) {
                        // do in 32-bit chunks (avoid lots of 64-bit divides even with constant denominators)
                        char* o = s - 8;
                        if (n64 >= 100000000) {
                            n = (u32)(n64 % 100000000);
                            n64 /= 100000000;
                        } else {
                            n   = (u32)n64;
                            n64 = 0;
                        }
                        if ((fl & TRIPLET_COMMA) == 0) {
                            do {
                                s -= 2;
                                *(u16*)s = *(u16*)&DIGIT_PAIR.pair[(n % 100) * 2];
                                n /= 100;
                            } while (n);
                        }
                        while (n) {
                            if ((fl & TRIPLET_COMMA) && (l++ == 3)) {
                                l = 0;
                                --s;
                                *s = COMMA;
                                --o;
                            } else {
                                --s;
                                *s = (char)(n % 10) + '0';
                                n /= 10;
                            }
                        }
                        if (n64 == 0) {
                            if ((s[0] == '0') && (s != (num + NUMSZ))) {
                                ++s;
                            }
                            break;
                        }
                        while (s != o) {
                            if ((fl & TRIPLET_COMMA) && (l++ == 3)) {
                                l = 0;
                                --s;
                                *s = COMMA;
                                --o;
                            } else {
                                --s;
                                *s = '0';
                            }
                        }
                    }

                    tail[0] = 0;
                    lead_sign(fl, lead);

                    // get the length that we copied
                    l = (u32)((num + NUMSZ) - s);
                    if (l == 0) {
                        --s;
                        *s = '0';
                        l  = 1;
                    }
                    cs = l + (3 << 24);
                    if (pr < 0) {
                        pr = 0;
                    }

                scopy:
                    // get fw=leading/trailing space, pr=leading zeros
                    if (pr < (i32)l) {
                        pr = (i32)l;
                    }
                    n = (u32)(pr + lead[0] + tail[0] + tz);
                    if (fw < (i32)n) {
                        fw = (i32)n;
                    }
                    fw -= (i32)n;
                    pr -= l;

                    // handle right justify and leading zeros
                    if ((fl & LEFTJUST) == 0) {
                        // if leading zeros, everything is in pr
                        if (fl & LEADINGZERO) {
                            pr = (fw > pr) ? fw : pr;
                            fw = 0;
                        } else {
                            fl &= ~TRIPLET_COMMA;  // if no leading zeros, then no commas
                        }
                    }

                    // copy the spaces and/or zeros
                    if (fw + pr) {
                        // copy leading spaces (or when doing %8.4d stuff)
                        i32 i;
                        u32 c;
                        if ((fl & LEFTJUST) == 0) {
                            while (fw > 0) {
                                psh_cb_buf_clamp(i, fw);
                                fw -= i;
                                while (i) {
                                    if ((((uptr)bf) & 3) == 0) {
                                        break;
                                    }
                                    *bf = ' ';
                                    ++bf;
                                    --i;
                                }
                                while (i >= 4) {
                                    *(u32*)bf = 0x20202020;
                                    bf += 4;
                                    i -= 4;
                                }
                                while (i) {
                                    *bf = ' ';
                                    ++bf;
                                    --i;
                                }
                                psh_impl_chk_cb_buf(1);
                            }
                        }

                        // copy leader
                        sn = lead + 1;
                        while (lead[0]) {
                            psh_cb_buf_clamp(i, lead[0]);
                            lead[0] -= (char)i;
                            while (i) {
                                *bf = *sn;
                                ++bf;
                                ++sn;
                                --i;
                            }
                            psh_impl_chk_cb_buf(1);
                        }

                        // copy leading zeros
                        c = cs >> 24;
                        cs &= 0xffffff;
                        cs = (fl & TRIPLET_COMMA) ? ((u32)(c - (((u32)pr + cs) % (c + 1)))) : 0;
                        while (pr > 0) {
                            psh_cb_buf_clamp(i, pr);
                            pr -= i;
                            if ((fl & TRIPLET_COMMA) == 0) {
                                while (i) {
                                    if ((((uptr)bf) & 3) == 0) {
                                        break;
                                    }
                                    *bf = '0';
                                    ++bf;
                                    --i;
                                }
                                while (i >= 4) {
                                    *(u32*)bf = 0x30303030;
                                    bf += 4;
                                    i -= 4;
                                }
                            }
                            while (i) {
                                if ((fl & TRIPLET_COMMA) && (cs++ == c)) {
                                    cs  = 0;
                                    *bf = COMMA;
                                    ++bf;
                                } else {
                                    *bf = '0';
                                    ++bf;
                                }
                                --i;
                            }
                            psh_impl_chk_cb_buf(1);
                        }
                    }

                    // copy leader if there is still one
                    sn = lead + 1;
                    while (lead[0]) {
                        i32 i;
                        psh_cb_buf_clamp(i, lead[0]);
                        lead[0] -= (char)i;
                        while (i) {
                            *bf = *sn;
                            ++bf;
                            ++sn;
                            --i;
                        }
                        psh_impl_chk_cb_buf(1);
                    }

                    // copy the string
                    n = l;
                    while (n) {
                        i32 i;
                        psh_cb_buf_clamp(i, (i32)n);
                        n -= (u32)i;

                        while (i >= 4) {
                            // @NOTE: These are unaligned memory accesses.
                            *(u32 volatile*)bf = *(u32 volatile*)s;
                            bf += 4;
                            s += 4;
                            i -= 4;
                        }

                        while (i) {
                            *bf = *s;
                            ++bf;
                            ++s;
                            --i;
                        }
                        psh_impl_chk_cb_buf(1);
                    }

                    // copy trailing zeros
                    while (tz) {
                        i32 i;
                        psh_cb_buf_clamp(i, tz);
                        tz -= i;
                        while (i) {
                            if ((((uptr)bf) & 3) == 0) {
                                break;
                            }
                            *bf = '0';
                            ++bf;
                            --i;
                        }
                        while (i >= 4) {
                            *(u32*)bf = 0x30303030;
                            bf += 4;
                            i -= 4;
                        }
                        while (i) {
                            *bf = '0';
                            ++bf;
                            --i;
                        }
                        psh_impl_chk_cb_buf(1);
                    }

                    // copy tail if there is one
                    sn = tail + 1;
                    while (tail[0]) {
                        i32 i;
                        psh_cb_buf_clamp(i, tail[0]);
                        tail[0] -= (char)i;
                        while (i) {
                            *bf = *sn;
                            ++bf;
                            ++sn;
                            --i;
                        }
                        psh_impl_chk_cb_buf(1);
                    }

                    // handle the left justify
                    if (fl & LEFTJUST) {
                        if (fw > 0) {
                            while (fw) {
                                i32 i;
                                psh_cb_buf_clamp(i, fw);
                                fw -= i;
                                while (i) {
                                    if ((((uptr)bf) & 3) == 0) {
                                        break;
                                    }
                                    *bf = ' ';
                                    ++bf;
                                    --i;
                                }
                                while (i >= 4) {
                                    *(u32*)bf = 0x20202020;
                                    bf += 4;
                                    i -= 4;
                                }
                                while (i--) {
                                    *bf = ' ';
                                    ++bf;
                                }
                                psh_impl_chk_cb_buf(1);
                            }
                        }
                    }
                    break;
                }
                default: {  // unknown, just copy code
                    s  = num + NUMSZ - 1;
                    *s = f[0];
                    l  = 1;
                    fw = fl = 0;
                    lead[0] = 0;
                    tail[0] = 0;
                    pr      = 0;
                    dp      = 0;
                    cs      = 0;
                    goto scopy;
                }
            }
            ++f;
        }
    endfmt:

        if (!callback) {
            *bf = 0;
        } else {
            psh_impl_flush_cb();
        }

    done:
        return tlen + (i32)(bf - buf);
    }

    psh_internal char* clamp_callback(cstring buf, void* user, i32 len) {
        SprintfContext* c = (SprintfContext*)user;
        c->length += len;

        if (len > c->count) {
            len = c->count;
        }

        if (len) {
            if (buf != c->buf) {
                const char *s, *se;
                char*       d;
                d  = c->buf;
                s  = buf;
                se = buf + len;
                do {
                    *d = *s;
                    ++d;
                    ++s;
                } while (s < se);
            }
            c->buf += len;
            c->count -= len;
        }

        if (c->count <= 0) {
            return c->tmp;
        }
        return (c->count >= PSH_IMPL_MIN_LENGTH_PER_CALLBACK) ? c->buf : c->tmp;  // go direct into buffer if you can
    }
}  // namespace psh::impl

#if PSH_COMPILER_CLANG
#    pragma clang diagnostic pop
#endif
