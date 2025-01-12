/// ------------------------------------------------------------------------------------------------
/// Disclaimer: This file is part of the Presheaf library and is a highly modified version of the
///             original library code. As the original, we still respect its public domain license.
///             Note, however, that the Presheaf code used here isn't licensed under public domain.
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
/// ------------------------------------------------------------------------------------------------

#if PSH_ENABLE_USE_STB_SPRINTF

#    include "psh_sprintf.hpp"
#    include "psh_core.hpp"
#    include "psh_platform.hpp"

#    if PSH_COMPILER_CLANG
#        pragma clang diagnostic push
#        pragma clang diagnostic ignored "-Wcast-align"
#        pragma clang diagnostic ignored "-Wcast-qual"
#        pragma clang diagnostic ignored "-Wold-style-cast"
#        pragma clang diagnostic ignored "-Wsign-conversion"
#        pragma clang diagnostic ignored "-Wdouble-promotion"
#        pragma clang diagnostic ignored "-Wconditional-uninitialized"
#    endif

#    if !defined(STB_SPRINTF_MIN_LENGTH_PER_CALLBACK)
#        define STB_SPRINTF_MIN_LENGTH_PER_CALLBACK 512  // how many characters per callback
#    endif

// internal f32 utility functions
psh_internal i32 stbsp_impl_real_to_str(cstring* start, u32* len, char* out, i32* decimal_pos, f64 value, u32 frac_digits);
psh_internal i32 stbsp_impl_real_to_parts(i64* bits, i32* expo, f64 value);

psh_internal constexpr i32 STBSP_IMPL_SPECIAL = 0x7000;

psh_internal constexpr char STBSP_IMPL_PERIOD = '.';
psh_internal constexpr char STBSP_IMPL_COMMA  = ',';

psh_internal constexpr struct {
    i16  unused_16_bits;  // force next field to be 2-byte aligned
    char pair[201];
} STBSP_IMPL_DIGITPAIR = {
    0,
    "00010203040506070809101112131415161718192021222324"
    "25262728293031323334353637383940414243444546474849"
    "50515253545556575859606162636465666768697071727374"
    "75767778798081828384858687888990919293949596979899",
};

psh_internal constexpr u32 STBSP_IMPL_LEFTJUST       = 1;
psh_internal constexpr u32 STBSP_IMPL_LEADINGPLUS    = 2;
psh_internal constexpr u32 STBSP_IMPL_LEADINGSPACE   = 4;
psh_internal constexpr u32 STBSP_IMPL_LEADING_0X     = 8;
psh_internal constexpr u32 STBSP_IMPL_LEADINGZERO    = 16;
psh_internal constexpr u32 STBSP_IMPL_INTMAX         = 32;
psh_internal constexpr u32 STBSP_IMPL_TRIPLET_COMMA  = 64;
psh_internal constexpr u32 STBSP_IMPL_NEGATIVE       = 128;
psh_internal constexpr u32 STBSP_IMPL_METRIC_SUFFIX  = 256;
psh_internal constexpr u32 STBSP_IMPL_HALFWIDTH      = 512;
psh_internal constexpr u32 STBSP_IMPL_METRIC_NOSPACE = 1024;
psh_internal constexpr u32 STBSP_IMPL_METRIC_1024    = 2048;
psh_internal constexpr u32 STBSP_IMPL_METRIC_JEDEC   = 4096;

psh_internal void stbsp_impl_lead_sign(u32 fl, char* sign) {
    sign[0] = 0;
    if (fl & STBSP_IMPL_NEGATIVE) {
        sign[0] = 1;
        sign[1] = '-';
    } else if (fl & STBSP_IMPL_LEADINGSPACE) {
        sign[0] = 1;
        sign[1] = ' ';
    } else if (fl & STBSP_IMPL_LEADINGPLUS) {
        sign[0] = 1;
        sign[1] = '+';
    }
}

using StbspSprintfCallback = char*(cstring buf, void* user, i32 len);

psh_internal psh_attribute_disable_asan i32 psh_stbsp_impl_vsprintfcb(StbspSprintfCallback* callback, void* user, char* buf, cstring fmt, va_list va) {
    // macros for the callback buffer stuff
#    define stbsp_impl_chk_cb_bufL(bytes)                                 \
        do {                                                              \
            i32 len = (i32)(bf - buf);                                    \
            if ((len + (bytes)) >= STB_SPRINTF_MIN_LENGTH_PER_CALLBACK) { \
                tlen += len;                                              \
                if ((bf = buf = callback(buf, user, len)) == nullptr) {   \
                    goto done;                                            \
                }                                                         \
            }                                                             \
        } while (0)
#    define stbsp_impl_chk_cb_buf(bytes)       \
        do {                                   \
            if (callback) {                    \
                stbsp_impl_chk_cb_bufL(bytes); \
            }                                  \
        } while (0)
// flush if there is even one byte in the buffer
#    define stbsp_impl_flush_cb()                                            \
        do {                                                                 \
            stbsp_impl_chk_cb_bufL(STB_SPRINTF_MIN_LENGTH_PER_CALLBACK - 1); \
        } while (0)
#    define stbsp_impl_cb_buf_clamp(cl, v)                                      \
        do {                                                                    \
            cl = v;                                                             \
            if (callback) {                                                     \
                i32 lg = STB_SPRINTF_MIN_LENGTH_PER_CALLBACK - (i32)(bf - buf); \
                if (cl > lg) {                                                  \
                    cl = lg;                                                    \
                }                                                               \
            }                                                                   \
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
                stbsp_impl_chk_cb_buf(1);
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
                    if ((STB_SPRINTF_MIN_LENGTH_PER_CALLBACK - (i32)(bf - buf)) < 4) {
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
                    fl |= STBSP_IMPL_LEFTJUST;
                    ++f;
                    continue;
                }
                // if we have leading plus
                case '+': {
                    fl |= STBSP_IMPL_LEADINGPLUS;
                    ++f;
                    continue;
                }
                // if we have leading space
                case ' ': {
                    fl |= STBSP_IMPL_LEADINGSPACE;
                    ++f;
                    continue;
                }
                // if we have leading 0x
                case '#': {
                    fl |= STBSP_IMPL_LEADING_0X;
                    ++f;
                    continue;
                }
                // if we have thousand commas
                case '\'': {
                    fl |= STBSP_IMPL_TRIPLET_COMMA;
                    ++f;
                    continue;
                }
                // if we have kilo marker (none->kilo->kibi->jedec)
                case '$': {
                    if (fl & STBSP_IMPL_METRIC_SUFFIX) {
                        if (fl & STBSP_IMPL_METRIC_1024) {
                            fl |= STBSP_IMPL_METRIC_JEDEC;
                        } else {
                            fl |= STBSP_IMPL_METRIC_1024;
                        }
                    } else {
                        fl |= STBSP_IMPL_METRIC_SUFFIX;
                    }
                    ++f;
                    continue;
                }
                // if we don't want space between metric suffix and number
                case '_': {
                    fl |= STBSP_IMPL_METRIC_NOSPACE;
                    ++f;
                    continue;
                }
                // if we have leading zero
                case '0': {
                    fl |= STBSP_IMPL_LEADINGZERO;
                    ++f;
                    goto flags_done;
                }
                default: goto flags_done;
            }
        }
    flags_done:

        // get the field width
        if (f[0] == '*') {
            fw = va_arg(va, u32);
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
                pr = va_arg(va, u32);
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
                fl |= STBSP_IMPL_HALFWIDTH;
                ++f;
                if (f[0] == 'h') {
                    ++f;  // QUARTERWIDTH
                }
                break;
            }
            // are we 64-bit (unix style)
            case 'l': {
                fl |= ((psh_usize_of(long) == 8) ? STBSP_IMPL_INTMAX : 0);
                ++f;
                if (f[0] == 'l') {
                    fl |= STBSP_IMPL_INTMAX;
                    ++f;
                }
                break;
            }
            // are we 64-bit on intmax? (c99)
            case 'j': {
                fl |= (psh_usize_of(usize) == 8) ? STBSP_IMPL_INTMAX : 0;
                ++f;
                break;
            }
            // are we 64-bit on size_t or ptrdiff_t? (c99)
            case 'z': {
                fl |= (psh_usize_of(isize) == 8) ? STBSP_IMPL_INTMAX : 0;
                ++f;
                break;
            }
            case 't': {
                fl |= (psh_usize_of(isize) == 8) ? STBSP_IMPL_INTMAX : 0;
                ++f;
                break;
            }
            // are we 64-bit (msft style)
            case 'I': {
                if ((f[1] == '6') && (f[2] == '4')) {
                    fl |= STBSP_IMPL_INTMAX;
                    f += 3;
                } else if ((f[1] == '3') && (f[2] == '2')) {
                    f += 3;
                } else {
                    fl |= ((psh_usize_of(void*) == 8) ? STBSP_IMPL_INTMAX : 0);
                    ++f;
                }
                break;
            }
            default: break;
        }

        // handle each replacement
        switch (f[0]) {
#    define STBSP_IMPL_NUMSZ 512  // big enough for e308 (with commas) or e-307
            char    num[STBSP_IMPL_NUMSZ];
            char    lead[8];
            char    tail[8];
            char*   s;
            cstring h;
            u32     l, n, cs;
            u64     n64;
            f64     fv;
            i32     dp;
            cstring sn;

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
                    n = ((u32)(pr - n)) >> 2;
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
                    l = pr;
                }

                lead[0] = 0;
                tail[0] = 0;
                pr      = 0;
                dp      = 0;
                cs      = 0;
                // copy the string in
                goto scopy;
            }

                // // @NOTE: Presheaf type addition.
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
                s       = num + STBSP_IMPL_NUMSZ - 1;
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
                if (stbsp_impl_real_to_parts((i64*)&n64, &dp, fv)) {
                    fl |= STBSP_IMPL_NEGATIVE;
                }

                s = num + 64;

                stbsp_impl_lead_sign(fl, lead);

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
                    *s = STBSP_IMPL_PERIOD;
                    ++s;
                }
                sn = s;

                // print the bits
                n = pr;
                if (n > 13) {
                    n = 13;
                }
                if (pr > (i32)n) {
                    tz = pr - n;
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
                l  = (i32)(s - (num + 64));
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
                if (stbsp_impl_real_to_str(&sn, &l, num, &dp, fv, (pr - 1) | 0x80000000)) {
                    fl |= STBSP_IMPL_NEGATIVE;
                }

                // clamp the precision and delete extra zeros after clamp
                n = pr;
                if (l > (u32)pr) {
                    l = pr;
                }
                while ((l > 1) && (pr) && (sn[l - 1] == '0')) {
                    --pr;
                    --l;
                }

                // should we use %e
                if ((dp <= -4) || (dp > (i32)n)) {
                    if (pr > (i32)l) {
                        pr = l - 1;
                    } else if (pr) {
                        --pr;  // when using %e, there is one digit before the decimal
                    }
                    goto doexpfromg;
                }
                // this is the insane action to get the pr to match %g semantics for %f
                if (dp > 0) {
                    pr = (dp < (i32)l) ? l - dp : 0;
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
                if (stbsp_impl_real_to_str(&sn, &l, num, &dp, fv, pr | 0x80000000)) {
                    fl |= STBSP_IMPL_NEGATIVE;
                }
            doexpfromg:
                tail[0] = 0;
                stbsp_impl_lead_sign(fl, lead);
                if (dp == STBSP_IMPL_SPECIAL) {
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
                    *s = STBSP_IMPL_PERIOD;
                    ++s;
                }

                // handle after decimal
                if ((l - 1) > (u32)pr) {
                    l = pr + 1;
                }

                for (n = 1; n < l; n++) {
                    *s = sn[n];
                    ++s;
                }

                // trailing zeros
                tz      = pr - (l - 1);
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
                if (fl & STBSP_IMPL_METRIC_SUFFIX) {
                    f64 divisor;
                    divisor = 1000.0f;
                    if (fl & STBSP_IMPL_METRIC_1024) {
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
                if (stbsp_impl_real_to_str(&sn, &l, num, &dp, fv, pr)) {
                    fl |= STBSP_IMPL_NEGATIVE;
                }
            dof32fromg:
                tail[0] = 0;
                stbsp_impl_lead_sign(fl, lead);
                if (dp == STBSP_IMPL_SPECIAL) {
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
                        *s = STBSP_IMPL_PERIOD;
                        ++s;
                    }
                    n = -dp;
                    if ((i32)n > pr) {
                        n = pr;
                    }
                    i = n;
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
                        l = pr - n;
                    }
                    i = l;
                    while (i) {
                        *s = *sn;
                        ++s;
                        ++sn;
                        --i;
                    }
                    tz = pr - (n + l);
                    cs = 1 + (3 << 24);  // how many tens did we write (for commas below)
                } else {
                    cs = (fl & STBSP_IMPL_TRIPLET_COMMA) ? ((600 - (u32)dp) % 3) : 0;
                    if ((u32)dp >= l) {
                        // handle xxxx000*000.0
                        n = 0;
                        for (;;) {
                            if ((fl & STBSP_IMPL_TRIPLET_COMMA) && (++cs == 4)) {
                                cs = 0;
                                *s = STBSP_IMPL_COMMA;
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
                            n = dp - n;
                            if ((fl & STBSP_IMPL_TRIPLET_COMMA) == 0) {
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
                                if ((fl & STBSP_IMPL_TRIPLET_COMMA) && (++cs == 4)) {
                                    cs = 0;
                                    *s = STBSP_IMPL_COMMA;
                                    ++s;
                                } else {
                                    *s = '0';
                                    ++s;
                                    --n;
                                }
                            }
                        }
                        cs = (i32)(s - (num + 64)) + (3 << 24);  // cs is how many tens
                        if (pr) {
                            *s = STBSP_IMPL_PERIOD;
                            ++s;
                            tz = pr;
                        }
                    } else {
                        // handle xxxxx.xxxx000*000
                        n = 0;
                        for (;;) {
                            if ((fl & STBSP_IMPL_TRIPLET_COMMA) && (++cs == 4)) {
                                cs = 0;
                                *s = STBSP_IMPL_COMMA;
                                ++s;
                            } else {
                                *s = sn[n];
                                ++s;
                                ++n;
                                if (n >= (u32)dp)
                                    break;
                            }
                        }
                        cs = (i32)(s - (num + 64)) + (3 << 24);  // cs is how many tens
                        if (pr) {
                            *s = STBSP_IMPL_PERIOD;
                            ++s;
                        }
                        if ((l - dp) > (u32)pr) {
                            l = pr + dp;
                        }
                        while (n < l) {
                            *s = sn[n];
                            ++s;
                            ++n;
                        }
                        tz = pr - (l - dp);
                    }
                }
                pr = 0;

                // handle k,m,g,t
                if (fl & STBSP_IMPL_METRIC_SUFFIX) {
                    char idx;
                    idx = 1;
                    if (fl & STBSP_IMPL_METRIC_NOSPACE) {
                        idx = 0;
                    }
                    tail[0] = idx;
                    tail[1] = ' ';
                    {
                        if (fl >> 24) {  // SI kilo is 'k', JEDEC and SI kibits are 'K'.
                            if (fl & STBSP_IMPL_METRIC_1024) {
                                tail[idx + 1] = "_KMGT"[fl >> 24];
                            } else {
                                tail[idx + 1] = "_kMGT"[fl >> 24];
                            }
                            idx++;
                            // If printing kibits and not in jedec, add the 'i'.
                            if (fl & STBSP_IMPL_METRIC_1024 && !(fl & STBSP_IMPL_METRIC_JEDEC)) {
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
                if (fl & STBSP_IMPL_LEADING_0X) {
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
                if (fl & STBSP_IMPL_LEADING_0X) {
                    lead[0] = 1;
                    lead[1] = '0';
                }
                l = (3 << 4) | (3 << 8);
                goto radixnum;
            }
            case 'p': {  // pointer
                fl |= (psh_usize_of(void*) == 8) ? STBSP_IMPL_INTMAX : 0;
                pr = psh_usize_of(void*) * 2;
                fl &= ~STBSP_IMPL_LEADINGZERO;  // 'p' only prints the pointer with zeros

                PSH_FALLTHROUGH;
            }
            case 'X': PSH_FALLTHROUGH;  // upper hex
            case 'x': {                 // lower hex
                h       = (f[0] == 'X') ? hexu : hex;
                l       = (4 << 4) | (4 << 8);
                lead[0] = 0;
                if (fl & STBSP_IMPL_LEADING_0X) {
                    lead[0] = 2;
                    lead[1] = '0';
                    lead[2] = h[16];
                }
            radixnum:
                // get the number
                if (fl & STBSP_IMPL_INTMAX) {
                    n64 = va_arg(va, u64);
                } else {
                    n64 = va_arg(va, u32);
                }

                s       = num + STBSP_IMPL_NUMSZ;
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
                    if (!((n64) || ((i32)((num + STBSP_IMPL_NUMSZ) - s) < pr))) {
                        break;
                    }
                    if (fl & STBSP_IMPL_TRIPLET_COMMA) {
                        ++l;
                        if ((l & 15) == ((l >> 4) & 15)) {
                            l &= ~15;
                            --s;
                            *s = STBSP_IMPL_COMMA;
                        }
                    }
                }
                // get the tens and the comma pos
                cs = (u32)((num + STBSP_IMPL_NUMSZ) - s) + ((((l >> 4) & 15)) << 24);
                // get the length that we copied
                l  = (u32)((num + STBSP_IMPL_NUMSZ) - s);
                // copy it
                goto scopy;
            }
            case 'u': PSH_FALLTHROUGH;  // unsigned integer
            case 'i': PSH_FALLTHROUGH;  // signed integer
            case 'd': {                 // signed integer
                // get the integer and abs it
                if (fl & STBSP_IMPL_INTMAX) {
                    i64 i = va_arg(va, i64);
                    n64   = (u64)i;
                    if ((f[0] != 'u') && (i < 0)) {
                        n64 = (u64)(-i);
                        fl |= STBSP_IMPL_NEGATIVE;
                    }
                } else {
                    i32 i = va_arg(va, i32);
                    n64   = (u32)i;
                    if ((f[0] != 'u') && (i < 0)) {
                        n64 = (u32)(-i);
                        fl |= STBSP_IMPL_NEGATIVE;
                    }
                }

                if (fl & STBSP_IMPL_METRIC_SUFFIX) {
                    if (n64 < 1024) {
                        pr = 0;
                    } else if (pr == -1) {
                        pr = 1;
                    }
                    fv = (f64)(i64)n64;
                    goto doaf32;
                }

                // convert to string
                s = num + STBSP_IMPL_NUMSZ;
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
                    if ((fl & STBSP_IMPL_TRIPLET_COMMA) == 0) {
                        do {
                            s -= 2;
                            *(u16*)s = *(u16*)&STBSP_IMPL_DIGITPAIR.pair[(n % 100) * 2];
                            n /= 100;
                        } while (n);
                    }
                    while (n) {
                        if ((fl & STBSP_IMPL_TRIPLET_COMMA) && (l++ == 3)) {
                            l = 0;
                            --s;
                            *s = STBSP_IMPL_COMMA;
                            --o;
                        } else {
                            --s;
                            *s = (char)(n % 10) + '0';
                            n /= 10;
                        }
                    }
                    if (n64 == 0) {
                        if ((s[0] == '0') && (s != (num + STBSP_IMPL_NUMSZ))) {
                            ++s;
                        }
                        break;
                    }
                    while (s != o) {
                        if ((fl & STBSP_IMPL_TRIPLET_COMMA) && (l++ == 3)) {
                            l = 0;
                            --s;
                            *s = STBSP_IMPL_COMMA;
                            --o;
                        } else {
                            --s;
                            *s = '0';
                        }
                    }
                }

                tail[0] = 0;
                stbsp_impl_lead_sign(fl, lead);

                // get the length that we copied
                l = (u32)((num + STBSP_IMPL_NUMSZ) - s);
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
                    pr = l;
                }
                n = pr + lead[0] + tail[0] + tz;
                if (fw < (i32)n) {
                    fw = n;
                }
                fw -= n;
                pr -= l;

                // handle right justify and leading zeros
                if ((fl & STBSP_IMPL_LEFTJUST) == 0) {
                    // if leading zeros, everything is in pr
                    if (fl & STBSP_IMPL_LEADINGZERO) {
                        pr = (fw > pr) ? fw : pr;
                        fw = 0;
                    } else {
                        fl &= ~STBSP_IMPL_TRIPLET_COMMA;  // if no leading zeros, then no commas
                    }
                }

                // copy the spaces and/or zeros
                if (fw + pr) {
                    // copy leading spaces (or when doing %8.4d stuff)
                    i32 i;
                    u32 c;
                    if ((fl & STBSP_IMPL_LEFTJUST) == 0) {
                        while (fw > 0) {
                            stbsp_impl_cb_buf_clamp(i, fw);
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
                            stbsp_impl_chk_cb_buf(1);
                        }
                    }

                    // copy leader
                    sn = lead + 1;
                    while (lead[0]) {
                        stbsp_impl_cb_buf_clamp(i, lead[0]);
                        lead[0] -= (char)i;
                        while (i) {
                            *bf = *sn;
                            ++bf;
                            ++sn;
                            --i;
                        }
                        stbsp_impl_chk_cb_buf(1);
                    }

                    // copy leading zeros
                    c = cs >> 24;
                    cs &= 0xffffff;
                    cs = (fl & STBSP_IMPL_TRIPLET_COMMA) ? ((u32)(c - ((pr + cs) % (c + 1)))) : 0;
                    while (pr > 0) {
                        stbsp_impl_cb_buf_clamp(i, pr);
                        pr -= i;
                        if ((fl & STBSP_IMPL_TRIPLET_COMMA) == 0) {
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
                            if ((fl & STBSP_IMPL_TRIPLET_COMMA) && (cs++ == c)) {
                                cs  = 0;
                                *bf = STBSP_IMPL_COMMA;
                                ++bf;
                            } else {
                                *bf = '0';
                                ++bf;
                            }
                            --i;
                        }
                        stbsp_impl_chk_cb_buf(1);
                    }
                }

                // copy leader if there is still one
                sn = lead + 1;
                while (lead[0]) {
                    i32 i;
                    stbsp_impl_cb_buf_clamp(i, lead[0]);
                    lead[0] -= (char)i;
                    while (i) {
                        *bf = *sn;
                        ++bf;
                        ++sn;
                        --i;
                    }
                    stbsp_impl_chk_cb_buf(1);
                }

                // copy the string
                n = l;
                while (n) {
                    i32 i;
                    stbsp_impl_cb_buf_clamp(i, n);
                    n -= i;

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
                    stbsp_impl_chk_cb_buf(1);
                }

                // copy trailing zeros
                while (tz) {
                    i32 i;
                    stbsp_impl_cb_buf_clamp(i, tz);
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
                    stbsp_impl_chk_cb_buf(1);
                }

                // copy tail if there is one
                sn = tail + 1;
                while (tail[0]) {
                    i32 i;
                    stbsp_impl_cb_buf_clamp(i, tail[0]);
                    tail[0] -= (char)i;
                    while (i) {
                        *bf = *sn;
                        ++bf;
                        ++sn;
                        --i;
                    }
                    stbsp_impl_chk_cb_buf(1);
                }

                // handle the left justify
                if (fl & STBSP_IMPL_LEFTJUST) {
                    if (fw > 0) {
                        while (fw) {
                            i32 i;
                            stbsp_impl_cb_buf_clamp(i, fw);
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
                            stbsp_impl_chk_cb_buf(1);
                        }
                    }
                }
                break;
            }
            default: {  // unknown, just copy code
                s  = num + STBSP_IMPL_NUMSZ - 1;
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
        stbsp_impl_flush_cb();
    }

done:
    return tlen + (i32)(bf - buf);
}

// -------------------------------------------------------------------------------------------------
// Wrapper functions.
// -------------------------------------------------------------------------------------------------

psh_proc psh_attribute_disable_asan i32 psh_stbsp_sprintf(char* buf, cstring fmt, ...) {
    i32     result;
    va_list va;
    va_start(va, fmt);
    result = psh_stbsp_impl_vsprintfcb(nullptr, nullptr, buf, fmt, va);
    va_end(va);
    return result;
}

struct stbsp_impl_context {
    char* buf;
    i32   count;
    i32   length;
    char  tmp[STB_SPRINTF_MIN_LENGTH_PER_CALLBACK];
};

psh_internal char* stbsp_impl_clamp_callback(cstring buf, void* user, i32 len) {
    stbsp_impl_context* c = (stbsp_impl_context*)user;
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
    return (c->count >= STB_SPRINTF_MIN_LENGTH_PER_CALLBACK) ? c->buf : c->tmp;  // go direct into buffer if you can
}

psh_internal char* stbsp_impl_count_clamp_callback(cstring buf, void* user, i32 len) {
    stbsp_impl_context* c = (stbsp_impl_context*)user;
    psh_discard_value(buf);

    c->length += len;
    return c->tmp;  // go direct into buffer if you can
}

psh_proc psh_attribute_disable_asan i32 psh_stbsp_vsnprintf(char* buf, i32 count, cstring fmt, va_list va) {
    stbsp_impl_context c;

    if ((count == 0) && !buf) {
        c.length = 0;

        psh_stbsp_impl_vsprintfcb(stbsp_impl_count_clamp_callback, &c, c.tmp, fmt, va);
    } else {
        c.buf    = buf;
        c.count  = count;
        c.length = 0;

        psh_stbsp_impl_vsprintfcb(stbsp_impl_clamp_callback, &c, stbsp_impl_clamp_callback(nullptr, &c, 0), fmt, va);

        // zero-terminate
        i32 l = (i32)(c.buf - buf);
        if (l >= count) {
            // should never be greater, only equal (or less) than count
            l = count - 1;
        }
        buf[l] = 0;
    }

    return c.length;
}

psh_proc psh_attribute_disable_asan i32 psh_stbsp_snprintf(char* buf, i32 count, cstring fmt, ...) {
    va_list va;
    va_start(va, fmt);

    i32 result = psh_stbsp_vsnprintf(buf, count, fmt, va);

    va_end(va);
    return result;
}

// -------------------------------------------------------------------------------------------------
// Low level f32 utility functions
// -------------------------------------------------------------------------------------------------

// copies d to bits w/ strict aliasing (this compiles to nothing on /Ox)
#    define STBSP_IMPL_COPYFP(dest, src)                \
        do {                                            \
            for (i32 cn = 0; cn < 8; ++cn) {            \
                ((char*)&dest)[cn] = ((char*)&src)[cn]; \
            }                                           \
        } while (0)

// get f32 info
psh_internal i32 stbsp_impl_real_to_parts(i64* bits, i32* expo, f64 value) {
    f64 d;
    i64 b = 0;

    // load value and round at the frac_digits
    d = value;

    STBSP_IMPL_COPYFP(b, d);

    *bits = b & ((((u64)1) << 52) - 1);
    *expo = (i32)(((b >> 52) & 2047) - 1023);

    return (i32)((u64)b >> 63);
}

psh_internal constexpr f64 stbsp_impl_bot[23] = {
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
psh_internal constexpr f64 stbsp_impl_negbot[22] = {
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
psh_internal constexpr f64 stbsp_impl_negboterr[22] = {
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
psh_internal constexpr f64 stbsp_impl_top[13] = {
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
psh_internal constexpr f64 stbsp_impl_negtop[13] = {
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
psh_internal constexpr f64 stbsp_impl_toperr[13] = {
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
psh_internal constexpr f64 stbsp_impl_negtoperr[13] = {
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

psh_internal constexpr u64 stbsp_impl_powten[20] = {
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
psh_internal constexpr u64 stbsp_impl_tento19th = 1000000000000000000;

#    define stbsp_impl_ddmulthi(oh, ol, xh, yh)                               \
        do {                                                                  \
            oh = xh * yh;                                                     \
            i64 bt;                                                           \
            STBSP_IMPL_COPYFP(bt, xh);                                        \
            bt &= ((~(u64)0) << 27);                                          \
            f64 ahi = 0;                                                      \
            STBSP_IMPL_COPYFP(ahi, bt);                                       \
            f64 alo = xh - ahi;                                               \
            STBSP_IMPL_COPYFP(bt, yh);                                        \
            bt &= ((~(u64)0) << 27);                                          \
            f64 bhi = 0;                                                      \
            STBSP_IMPL_COPYFP(bhi, bt);                                       \
            f64 blo = yh - bhi;                                               \
            ol      = ((ahi * bhi - oh) + ahi * blo + alo * bhi) + alo * blo; \
        } while (0)
#    define stbsp_impl_ddtoS64(ob, xh, xl)         \
        do {                                       \
            f64 ahi = 0;                           \
            ob      = (i64)xh;                     \
            f64 vh  = (f64)ob;                     \
            ahi     = (xh - vh);                   \
            f64 t   = (ahi - xh);                  \
            f64 alo = (xh - (ahi - t)) - (vh + t); \
            ob += (i64)(ahi + alo + xl);           \
        } while (0)
#    define stbsp_impl_ddrenorm(oh, ol) \
        do {                            \
            f64 s = oh + ol;            \
            ol    = ol - (s - oh);      \
            oh    = s;                  \
        } while (0)
#    define stbsp_impl_ddmultlo(oh, ol, xh, xl, yh, yl) \
        do {                                            \
            ol = ol + (xh * yl + xl * yh);              \
        } while (0)
#    define stbsp_impl_ddmultlos(oh, ol, xh, yl) \
        do {                                     \
            ol = ol + (xh * yl);                 \
        } while (0)

psh_internal void stbsp_impl_raise_to_power10(f64* ohi, f64* olo, f64 d, i32 power)  // power can be -323 to +350
{
    f64 ph, pl;
    if ((power >= 0) && (power <= 22)) {
        stbsp_impl_ddmulthi(ph, pl, d, stbsp_impl_bot[power]);
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
                stbsp_impl_ddmulthi(ph, pl, d, stbsp_impl_negbot[eb]);
                stbsp_impl_ddmultlos(ph, pl, d, stbsp_impl_negboterr[eb]);
            }
            if (et) {
                stbsp_impl_ddrenorm(ph, pl);
                --et;
                stbsp_impl_ddmulthi(p2h, p2l, ph, stbsp_impl_negtop[et]);
                stbsp_impl_ddmultlo(p2h, p2l, ph, pl, stbsp_impl_negtop[et], stbsp_impl_negtoperr[et]);
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
                stbsp_impl_ddmulthi(ph, pl, d, stbsp_impl_bot[eb]);
                if (e) {
                    stbsp_impl_ddrenorm(ph, pl);
                    stbsp_impl_ddmulthi(p2h, p2l, ph, stbsp_impl_bot[e]);
                    stbsp_impl_ddmultlos(p2h, p2l, stbsp_impl_bot[e], pl);
                    ph = p2h;
                    pl = p2l;
                }
            }
            if (et) {
                stbsp_impl_ddrenorm(ph, pl);
                --et;
                stbsp_impl_ddmulthi(p2h, p2l, ph, stbsp_impl_top[et]);
                stbsp_impl_ddmultlo(p2h, p2l, ph, pl, stbsp_impl_top[et], stbsp_impl_toperr[et]);
                ph = p2h;
                pl = p2l;
            }
        }
    }
    stbsp_impl_ddrenorm(ph, pl);
    *ohi = ph;
    *olo = pl;
}

// Given an f32 value, returns the significant bits in bits, and the position of the decimal point
// in decimal_pos.  +/-INF and NAN are specified by special values returned in the decimal_pos
// parameter. frac_digits is absolute normally, but if you want from first significant digits (got
// %g and %e), or in 0x80000000.
psh_internal i32 stbsp_impl_real_to_str(
    cstring* start,
    u32*     len,
    char*    out,
    i32*     decimal_pos,
    f64      value,
    u32      frac_digits) {
    f64 d    = value;
    i64 bits = 0;
    STBSP_IMPL_COPYFP(bits, d);
    i32 expo = (i32)((bits >> 52) & 2047);
    i32 ng   = (i32)((u64)bits >> 63);
    if (ng) {
        d = -d;
    }

    // is nan or inf?
    if (expo == 2047) {
        *start       = (bits & ((((u64)1) << 52) - 1)) ? "NaN" : "Inf";
        *decimal_pos = STBSP_IMPL_SPECIAL;
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
        stbsp_impl_raise_to_power10(&ph, &pl, d, 18 - tens);

        // get full as much precision from f64-f64 as possible
        stbsp_impl_ddtoS64(bits, ph, pl);

        // check if we undershot
        if (((u64)bits) >= stbsp_impl_tento19th) {
            ++tens;
        }
    }

    // now do the rounding in integer land
    frac_digits = (frac_digits & 0x80000000) ? ((frac_digits & 0x7ffffff) + 1) : (tens + frac_digits);
    if (frac_digits < 24) {
        u32 dg = 1;
        if ((u64)bits >= stbsp_impl_powten[9]) {
            dg = 10;
        }
        while ((u64)bits >= stbsp_impl_powten[dg]) {
            ++dg;
            if (dg == 20) {
                goto noround;
            }
        }
        if (frac_digits < dg) {
            // add 0.5 at the right position and round
            e = dg - frac_digits;
            if ((u32)e >= 24) {
                goto noround;
            }
            u64 r = stbsp_impl_powten[e];
            bits  = bits + (r / 2);
            if ((u64)bits >= stbsp_impl_powten[dg]) {
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
            *(u16*)out = *(u16*)&STBSP_IMPL_DIGITPAIR.pair[(n % 100) * 2];
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
    *len         = e;
    return ng;
}

#    if PSH_COMPILER_CLANG
#        pragma clang diagnostic pop
#    endif

#endif  // PSH_ENABLE_USE_STB_SPRINTF
