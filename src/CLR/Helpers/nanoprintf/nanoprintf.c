//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006 - 2021 Skirrid Systems. All rights reserved.
// See LICENSE file in the project root for full license information.
//

// clang-format off

// System and config header files are included by the parent stub file.
#include <stdarg.h>
#include <math.h>
#include "nanoprintf.h"
#include <float.h>

/* Define default macro to access the format string using a pointer. */
#ifndef GET_FORMAT
    #define GET_FORMAT(p)   (*(p))
#endif

/* Define default function for printf output. */
#ifndef PUTCHAR_FUNC
    #define PUTCHAR_FUNC    putchar
#endif

/* Renames the functions if they have been defined as macros in printf.h */
#ifdef printf
    #undef  printf
    #define printf _prntf
#endif

#ifdef sprintf
    #undef  sprintf
    #define sprintf _sprntf
#endif

// Macro used to check presence of a feature flag.
// These are defined in print_cfg.h
#define FEATURE(flag)   ((FEATURE_FLAGS) & (flag))

// Size of buffer for formatting numbers into.
// Use the smallest buffer we can get away with to conserve RAM.
#if FEATURE(USE_BINARY)
    #if FEATURE(USE_LONG)
        #define BUFMAX  32
    #elif FEATURE(USE_FLOAT)
        #define BUFMAX  30
    #else
        #define BUFMAX  16
    #endif
#else
    #if FEATURE(USE_FLOAT)
        #define BUFMAX  30
    #elif FEATURE(USE_LONG_LONG)
        #define BUFMAX  22
    #else
        #define BUFMAX  16
    #endif
#endif

// Bit definitions in the flags variable (integer and general)
#if FEATURE(USE_LEFT_JUST)
  #define FL_LEFT_JUST  (1<<0)
#else
  #define FL_LEFT_JUST  0
#endif
#define FL_ZERO_PAD     (1<<1)
#define FL_SPECIAL      (1<<2)
#define FL_PLUS         (1<<3)
#define FL_SPACE        (1<<4)
#define FL_NEG          (1<<5)
#define FL_LONG         (1<<6)
#define FL_FSTR         (1<<7)

// Bit definitions in the fflags variable (mostly floating point)
#define FF_UCASE        (1<<0)
#define FF_ECVT         (1<<1)
#define FF_FCVT         (1<<2)
#define FF_GCVT         (1<<3)
#define FF_NRND         (1<<4)
#define FF_XLONG        (1<<5)

// Check whether integer or octal support is needed.
#define HEX_CONVERT_ONLY    !(FEATURE(USE_SIGNED) || FEATURE(USE_SIGNED_I) || FEATURE(USE_UNSIGNED) || \
                              FEATURE(USE_OCTAL) || FEATURE(USE_BINARY))

/*****************************************************************************
Floating point 
******************************************************************************/
#if FEATURE(USE_FLOAT)

// Most compilers can support double precision
#ifndef NO_DOUBLE_PRECISION
  #define DP_LIMIT      310
  #define MAX_POWER     256
// [NF_CHANGE]
  #define FLOAT_DIGITS  19
// [END_NF_CHANGE]
#else
  #define DP_LIMIT      40
  #define MAX_POWER     32
  #define FLOAT_DIGITS  8
#endif

#if !FEATURE(USE_SMALL_FLOAT)
    // Floating point normalisation tables for fast normalisation.
    // smalltable[] is used for value < 1.0
    static const double smalltable[] = {
    #ifndef NO_DOUBLE_PRECISION
        1e-256, 1e-128, 1e-64,
    #endif
        1e-32, 1e-16, 1e-8, 1e-4, 1e-2, 1e-1, 1.0
    };
    // large table[] is used for value >= 10.0
    static const double largetable[] = {
    #ifndef NO_DOUBLE_PRECISION
        1e+256, 1e+128, 1e+64,
    #endif
        1e+32, 1e+16, 1e+8, 1e+4, 1e+2, 1e+1
    };
#endif

#ifdef NO_DOUBLE_PRECISION
    // Double precision not supported by compiler.
    // Single precision numbers up to 10^38 require only 8-bit exponent.
    typedef signed char flt_width_t;
#else
    // Double precision numbers up to 10^308 require 16-bit exponents.
    typedef signed short flt_width_t;
#endif

// These define the maximum number of digits that can be output into the buffer.
// %f requires space for sign, point and rounding digit.
#define DIGITS_MAX_F    (BUFMAX-3)
// %e requires space for sign, point and up to 3 digit exponent (E+123).
// Rounding happens in the place where the exponent will go.
#define DIGITS_MAX_E    (BUFMAX-7)

// Function to trim trailing zeros and DP in 'g' mode.
// Return pointer to new string terminator position.
static char *trim_zeros(char *p)
{
    // Trim trailing 0's in 'g' mode.
    while (*p == '0')
        p--;
    // If last non-zero is DP, trim that as well.
    if (*p != '.') ++p;
    return p;
}

/* ---------------------------------------------------------------------------
Function: format_float()
Called from the main doprnt function to handle formatting of floating point
values. The general process is:
1. Check for non-numbers and just display the error code.
2. Convert number to positive form.
3. Normalise the number to lie in the range 1.0 <= number < 10.0
4. Work out where the DP lies and optimum format.
5. Output the digits in reverse order.
--------------------------------------------------------------------------- */
static char *format_float(double number, flt_width_t ndigits, flt_width_t width,
                          unsigned char flags, unsigned char fflags, char *buf)
{
    flt_width_t decpt;
    flt_width_t nzero;
    unsigned char i;
    char *p = buf + 2;
    char *pend;
    
#ifndef NO_ISNAN_ISINF
    // Handle special values which need no formatting
    if (isinf(number))
    {
        buf[0] = 'I';
        buf[1] = 'n';
        buf[2] = 'f';
        buf[3] = '\0';
        return buf;
    }
    if (isnan(number))
    {
        buf[0] = 'N';
        buf[1] = 'a';
        buf[2] = 'N';
        buf[3] = '\0';
        return buf;
    }
#endif

    // Handle all numbers as if they were positive.
    if (number < 0)
    {
        number = -number;
        flags |= FL_NEG;
    }

    if (fflags & FF_FCVT)
    {
        // Number of DP cannot be negative.
        if (ndigits < 0)
            ndigits = 0;
    }
    else
    {
        // Significant digits must be at least 1.
        if (ndigits < 1)
            ndigits = 1;
    }

    // Prefill buffer with zeros.
    for (i = 0; i < BUFMAX; i++)
        buf[i] = '0';

    if (number == 0)
    {
        // Special case to correct number of decimals, significant figures,
        // and avoid printing 0E-00.
        decpt = 1;
    }
    else
    {
        // Normalise the number such that it lies in the range 1 <= n < 10.
#if FEATURE(USE_SMALL_FLOAT)
        /* Normalise using a linear search. This code is simple and uses
         * least code space. It also eliminates the lookup tables which may
         * otherwise take up RAM space. However, it can take many more operations
         * to execute for the full range of floating point values, and may
         * introduce rounding errors. Use only when space is critical.
         */
        // First make small numbers bigger.
        decpt = 1;
        while (number < 1.0)
        {
            number *= 10.0;
            --decpt;
        }
        // Then make big numbers smaller.
        while (number >= 10.0)
        {
            number /= 10.0;
            ++decpt;
#ifdef NO_ISNAN_ISINF
            // Avoid this loop hanging on infinity.
            if (decpt > DP_LIMIT)
            {
                buf[0] = 'I';
                buf[1] = 'n';
                buf[2] = 'f';
                buf[3] = '\0';
                return buf;
            }
#endif
        }
#else
        flt_width_t power10 = MAX_POWER;
        decpt = 1;

        if(number == DBL_MAX)
        {
            number = 1.7976931348623157;
            decpt = 309;
        }
        else if(number == DBL_MIN)
        {
            number = -1.7976931348623157;
			decpt = -309;
		}
        else
        {
            /* Normalise using a binary search, making the largest possible
             * adjustment first and getting progressively smaller. This gets
             * to the answer in the fastest time, with the minimum number of
             * operations to introduce rounding errors.
             */

            // Normalise the number such that it lies in the range 1 <= n < 10.
            // First make small numbers bigger.

            i = 0;
            while (number < 1.0)
            {
                while (number < smalltable[i + 1])
                {
                    number /= smalltable[i];
                    decpt -= power10;
                }
                power10 >>= 1;
                i++;
            }
            // Then make big numbers smaller.
            power10 = MAX_POWER;
            i = 0;
            while (number >= 10.0)
            {
                while (number >= largetable[i])
                {
                    number /= largetable[i];
                    decpt += power10;
#ifdef NO_ISNAN_ISINF
                    // Avoid this loop hanging on infinity.
                    if (decpt > DP_LIMIT)
                    {
                        buf[0] = 'I';
                        buf[1] = 'n';
                        buf[2] = 'f';
                        buf[3] = '\0';
                        return buf;
                    }
#endif
                }
                power10 >>= 1;
                i++;
            }
        }
#endif
    }
    
    // For g conversions determine whether to use e or f mode.
    if (fflags & FF_GCVT)
    {
        /* 'g' format uses 'f' notation where it can and
         * 'e' notation where the exponent is more extreme.
         * Some references indicate that it uses the more
         * compact form but the ANSI standard give an explict
         * definition: Use 'e' when the exponent is < -4
         * or where the exponent is >= ndigits.
         * The exponent is equal to decpt - 1 so this translates
         * to decpt <= -4 || decpt > ndigits
         * http://www.acm.uiuc.edu/webmonkeys/book/c_guide/2.12.html#printf
         * http://www.mkssoftware.com/docs/man3/printf.3.asp
         */
        if (decpt > -4 && decpt <= ndigits)
            fflags |= FF_FCVT;
    }
    
    // Sanitise ndigits making sure it fits buffer space.
    if (fflags & FF_FCVT)
    {
        if (!(fflags & FF_GCVT))
        {
            // For fcvt operation the number of digits is used to
            // refer to decimal places rather than significant digits.
            ndigits += decpt;
            // When there are no significant digits,
            // avoid printing too many 0's.
            if (ndigits < 0)
            {
                decpt -= ndigits;
            }
        }
        /* For 'f' conversions with positive DP value that would overflow
         * the buffer space, there is no way to display this so fall back
         * to 'e' format. 'f' conversion needs space for sign, point and
         * rounding digit. The point may be forced using the special flag
         * and the rounding digit may be included in the case of maximum
         * overflow.
         */
        if (decpt > DIGITS_MAX_F)
        {
            fflags &= ~FF_FCVT;
        }
    }

    nzero = 0;
    if (fflags & FF_FCVT)
    {
        if (decpt < 1)
        {
            // Values < 1 require leading zeros before the real digits.
            nzero = 1 - decpt;
        }
        // Check for buffer fit. Zeros take precedence.
        if (nzero > DIGITS_MAX_F)
        {
            nzero = DIGITS_MAX_F;
            ndigits = -1;
        }
        if (ndigits < 0)
        {
            // First significant digit is below rounding range.
            fflags |= FF_NRND;
            ndigits = 0;
        }
        else if (ndigits + nzero > DIGITS_MAX_F)
        {
            ndigits = DIGITS_MAX_F - nzero;
        }
        p += nzero;
    }
    else
    {
        // Allow space for sign, point exponent.
        if (ndigits > DIGITS_MAX_E)
            ndigits = DIGITS_MAX_E;
    }
    
    // Format digits one-by-one into the output string.
    // One extra digit is required for rounding.
    for (i = 0; i <= ndigits; i++)
    {
        // Ignore digits beyond the supported precision.
        if (i >= FLOAT_DIGITS)
        {
            *p++ = '0';
        }
        else
        {
            // number is normalised to a positive value between 0 and 9.
            int n = number;
            *p++ = n + '0';
            number = (number - n) * 10;
        }
    }
    // Store a pointer to the last (rounding) digit.
    pend = --p;
    // Round the result directly in the string buffer.
    // Only use the calculated digits, not trailing 0's.
    if (i > FLOAT_DIGITS)
    {
        p -= (i - FLOAT_DIGITS);
    }
    if (!(fflags & FF_NRND) && *p >= '5')
    {
        for (;;)
        {
            if (i == 0)
            {
                // The rounding has rippled all the way through to
                // the first digit. i.e. 9.999..9 -> 10.0
                // Just replace the first 0 with a 1 and shift the DP.
                *p = '1';
                ++decpt;
                // This increases the displayed digits for 'f' only.
                if ((fflags & (FF_FCVT|FF_GCVT)) == FF_FCVT)
                {
                    ++ndigits;
                    ++pend;
                }
                break;
            }
            // Previous digit was a rollover
            *p-- = '0';
            // Increment next digit and break out unless there is a rollover.
            if (*p != '9')
            {
                (*p)++;
                break;
            }
        }
    }

    // Insert the decimal point
    if (fflags & FF_FCVT)
    {
        flt_width_t num;
        num = (decpt > 1) ? decpt : 1;
        p = buf + 1;
        for (i = 0; i < num; i++)
        {
            *p = *(p+1);
            ++p;
        }
        if (p + 1 < pend || (flags & FL_SPECIAL))
        {
            // There are digits after the DP or DP is forced.
            *p = '.';
            // Trim trailing 0's in 'g' mode.
            if ((fflags & FF_GCVT) && !(flags & FL_SPECIAL))
            {
                pend = trim_zeros(pend - 1);
            }
        }
        else
        {
            // DP at end is not required.
            --pend;
        }
        // Leave p pointing to start of digit string.
        p = buf + 1;
    }
    else
    {
        // Leave p pointing to start of digit string.
        if (ndigits > 1 || (flags & FL_SPECIAL))
        {
            // Decimal point is always after first digit.
            // Shift digit and insert point.
            buf[1] = buf[2];
            buf[2] = '.';
            p = buf + 1;
        }
        else
        {
            // No decimal point needed.
            p = buf + 2;
        }
        // Trim trailing 0's in 'g' mode.
        if ((fflags & FF_GCVT) && !(flags & FL_SPECIAL))
        {
            pend = trim_zeros(pend - 1);
        }
        // Add exponent
        *pend++ = (fflags & FF_UCASE) ? 'E' : 'e';
        if (--decpt < 0)
        {
            *pend++ = '-';
            decpt = -decpt;
        }
        else
        {
            *pend++ = '+';
        }
#ifndef EXP_3_DIGIT
        // Optional 3rd digit of exponent
        if (decpt > 99)
#endif
        {
            *pend++ = decpt / 100 + '0';
            decpt %= 100;
        }
        // Always print at least 2 digits of exponent.
        *pend++ = decpt / 10 + '0';
        *pend++ = decpt % 10 + '0';
    }
    // Add the sign prefix.
    if      (flags & FL_NEG)    *--p = '-';
#if FEATURE(USE_PLUS_SIGN)
    else if (flags & FL_PLUS)   *--p = '+';
#endif
#if FEATURE(USE_SPACE_SIGN)
    else if (flags & FL_SPACE)  *--p = ' ';
#endif

    *pend = '\0';   // Add null terminator
    
#if FEATURE(USE_ZERO_PAD)
    if (flags & FL_ZERO_PAD)
    {
        /* Implement leading zero padding by shifting the formatted buffer.
         * This is not the most efficient but at any other point it is hard
         * to know what the exact buffer length will be. The processing time
         * pales into insignificance next to the floating point operations.
         */
        flt_width_t fwidth = pend - p;
        if (width > BUFMAX)
            width = BUFMAX;
        if (fwidth < width)
        {
            // Set pointer to location after the new end point.
            // It will be predecremented.
            char *pnew = pend + width - fwidth + 1;
            ++pend;
            // Do not shift the sign/space if used.
            if (flags & (FL_NEG | FL_PLUS | FL_SPACE))
                ++p;
            // p now points to the last character to move.
            // Shift the buffer rightwards
            do *--pnew = *--pend;
            while (pend != p);
            // Fill the remainder with 0s.
            do *--pnew = '0';
            while (pnew != p);
            // Restore the former value of p.
            if (flags & (FL_NEG | FL_PLUS | FL_SPACE))
                --p;
        }
    }
#endif

    return p;       // Start of string
}
#endif  // End of floating point section

/*****************************************************************************
Integer, character and string
******************************************************************************/

#if FEATURE(USE_SPACE_PAD)
/* ---------------------------------------------------------------------------
Function: p_len()
Helper function to find length of string.
This offers a small space saving over strlen and allows for reading strings
from flash where the micro uses different semantics to access program memory.
This is used on the AVR processor.
--------------------------------------------------------------------------- */
#if FEATURE(USE_FSTRING)
static width_t p_len(char *p, unsigned char flags)
#else
static width_t p_len(char *p)
#endif
{
    width_t len = 0;
#if FEATURE(USE_FSTRING)
    if (flags & FL_FSTR)
    {
        while (GET_FORMAT(p))
        {
            ++p;
            ++len;
        }
    }
    else
#endif
    {
        while (*p++) ++len;
    }
    return len;
}
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
// -Wimplicit-fallthrough option was added in GCC 7
#if (__GNUC__ >= 7)
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
#endif

/* ---------------------------------------------------------------------------
Function: doprnt()
This is the main worker function which does all the formatting.
The output function must always be provided.
Unless BASIC_PRINTF is defined it also needs the context variable,
which tells the output function where to write.
--------------------------------------------------------------------------- */
#ifdef BASIC_PRINTF_ONLY
static printf_t doprnt(void (*func)(char c), const char *fmt, va_list ap)
#else
static printf_t doprnt(void *context, void (*func)(char c, void *context), size_t n, const char *fmt, va_list ap)
#endif
{
#if FEATURE(USE_LONG_LONG)
    unsigned long long uvalue;
#elif FEATURE(USE_LONG)
    unsigned long uvalue;
#else
    unsigned uvalue;
#endif
#if !HEX_CONVERT_ONLY
    unsigned base;
#endif
#if FEATURE(USE_SPACE_PAD) || FEATURE(USE_ZERO_PAD) || FEATURE(USE_FLOAT)
    width_t width;
    width_t fwidth;
#endif
#if FEATURE(USE_PRECISION) || FEATURE(USE_FLOAT)
    width_t precision;
#else
    #define precision -1
#endif
    char convert, c;
    char *p;
    char buffer[BUFMAX+1];
#ifdef PRINTF_T
    printf_t count = 0;
#endif
    unsigned char flags;
#if FEATURE(USE_FLOAT) || FEATURE(USE_LONG_LONG)
    unsigned char fflags;
#endif
#if FEATURE(USE_FLOAT)
    double fvalue;
#endif

    buffer[BUFMAX] = '\0';

    for (;;)
    {
        convert = GET_FORMAT(fmt);
        if (convert == 0) break;
        if (convert == '%')
        {
            p = buffer + BUFMAX;
#if FEATURE(USE_PRECISION) || FEATURE(USE_FLOAT)
            precision = -1;
#endif
#if FEATURE(USE_SPACE_PAD) || FEATURE(USE_ZERO_PAD) || FEATURE(USE_FLOAT)
            width = 0;
#endif
#if FEATURE(USE_FLOAT) || FEATURE(USE_LONG_LONG)
            fflags = 0;
#endif
            flags = 0;

            // Extract flag chars
            for (;;)
            {
                convert = GET_FORMAT(++fmt);
#if FEATURE(USE_ZERO_PAD)
                if (convert == '0')
                {
                    flags |= FL_ZERO_PAD;
                }
                else
#endif
#if FEATURE(USE_PLUS_SIGN)
                if (convert == '+')
                {
                    flags |= FL_PLUS;
                }
                else
#endif
#if FEATURE(USE_LEFT_JUST)
                if (convert == '-')
                {
                    flags |= FL_LEFT_JUST;
                }
                else
#endif
#if FEATURE(USE_SPACE_SIGN)
                if (convert == ' ')
                {
                    flags |= FL_SPACE;
                }
                else
#endif
#if FEATURE(USE_SPECIAL)
                if (convert == '#')
                {
                    flags |= FL_SPECIAL;
                }
                else
#endif
                    break;
            }
#if FEATURE(USE_SPACE_PAD) || FEATURE(USE_ZERO_PAD)
            // Extract width
    #if FEATURE(USE_INDIRECT)
            if (convert == '*')
            {
                width = va_arg(ap, int);
                convert = GET_FORMAT(++fmt);
            }
            else
    #endif
            // cppcheck-suppress knownConditionTrueFalse    // False positive
            while (convert >= '0' && convert <= '9')
            {
                width = width * 10 + convert - '0';
                convert = GET_FORMAT(++fmt);
            }
#endif
#if FEATURE(USE_PRECISION)
            // Extract precision
            if (convert == '.')
            {
                precision = 0;
                convert = GET_FORMAT(++fmt);
    #if FEATURE(USE_INDIRECT)
                if (convert == '*')
                {
                    precision = va_arg(ap, int);
                    convert = GET_FORMAT(++fmt);
                }
                else
    #endif
                while (convert >= '0' && convert <= '9')
                {
                    precision = precision * 10 + convert - '0';
                    convert = GET_FORMAT(++fmt);
                }
            }
#endif
#if FEATURE(USE_LONG)
            // Extract length modifier
            if (convert == 'l')
            {
                convert = GET_FORMAT(++fmt);
    #if FEATURE(USE_LONG_LONG)
                if (convert == 'l')
                {
                    fflags |= FF_XLONG;
                    convert = GET_FORMAT(++fmt);
                }
                else
    #endif
                flags |= FL_LONG;
            }
#endif
            switch (convert)
            {
#if FEATURE(USE_CHAR)
            case 'c':
    #if FEATURE(USE_SPACE_PAD)
                width = 0;
    #endif
                *--p = (char) va_arg(ap, int);
                break;
#endif
#if FEATURE(USE_SIGNED)
            case 'd':
#endif
#if FEATURE(USE_SIGNED_I)
            case 'i':
#endif
#if FEATURE(USE_SIGNED) || FEATURE(USE_SIGNED_I)
                flags |= FL_NEG;    // Flag possible negative value, to be determined later
                base = 10;
                goto number;
#endif
#if FEATURE(USE_UNSIGNED)
            case 'u':
                base = 10;
                goto number;
#endif
#if FEATURE(USE_OCTAL)
            case 'o':
                base = 8;
                goto number;
#endif
#if FEATURE(USE_BINARY)
            case 'b':
                base = 2;
                goto number;
#endif
#if FEATURE(USE_HEX_LOWER)
            case 'x':
#endif
#if FEATURE(USE_HEX_UPPER)
            case 'X':
#endif
#if FEATURE(USE_HEX_LOWER) || FEATURE(USE_HEX_UPPER)
    #if !HEX_CONVERT_ONLY
                base = 16;
    #endif
#endif
#if !HEX_CONVERT_ONLY
            number:
#endif
                /* Using separate va_arg() calls for signed and unsigned types is expensive.
                   Instead, values are read as unsigned, regardless of signed/unsigned type.
                   Signed values then need to be sign-extended
                   and this is fixed after the check for negative numbers.
                */
#if FEATURE(USE_LONG)
    #if FEATURE(USE_LONG_LONG)
                if (fflags & FF_XLONG)
                    uvalue = va_arg(ap, unsigned long long);
                else
    #endif
                if (flags & FL_LONG)
                    uvalue = va_arg(ap, unsigned long);
                else
#endif
                    uvalue = va_arg(ap, unsigned int);
#if FEATURE(USE_SIGNED) || FEATURE(USE_SIGNED_I)
                // FL_NEG was used temporarily to indicate signed type
                if (flags & FL_NEG)
                {
                    // Values may need to be sign extended if not the widest type.
    #if FEATURE(USE_LONG)
        #if FEATURE(USE_LONG_LONG)
                    if (!(fflags & FF_XLONG))
                    {
                        if (!(flags & FL_LONG))
                            uvalue = (int) uvalue;
                        else
                            uvalue = (long) uvalue;
                    }
        #else
                    if (!(flags & FL_LONG))
                        uvalue = (int) uvalue;
        #endif
    #endif
                    // Check whether this is a negative value
    #if FEATURE(USE_LONG)
        #if FEATURE(USE_LONG_LONG)
                    if ((long long) uvalue < 0)
        #else
                    if ((long) uvalue < 0)
        #endif
    #else
                    if ((int) uvalue < 0)
    #endif
                    {
                        uvalue = -uvalue;   // Yes, it's negative
                    }
                    else
                    {
                        flags &= ~FL_NEG;   // No, it's positive
                    }
                }
#endif
#if FEATURE(USE_PRECISION)
                // Set default precision
                if (precision == -1) precision = 1;
#endif
                // Make sure options are valid.
#if HEX_CONVERT_ONLY
    #if FEATURE(USE_PLUS_SIGN) || FEATURE(USE_SPACE_SIGN)
                flags &= ~(FL_PLUS|FL_SPACE);
    #endif
#else
                if (base != 10) flags &= ~(FL_PLUS|FL_NEG|FL_SPACE);
    #if FEATURE(USE_SPECIAL)
                else            flags &= ~FL_SPECIAL;
    #endif
#endif
                // Generate the number without any prefix yet.
#if FEATURE(USE_ZERO_PAD)
                fwidth = width;
                // Avoid formatting buffer overflow.
                if (fwidth > BUFMAX) fwidth = BUFMAX;
#endif
#if FEATURE(USE_LONG_LONG) && FEATURE(USE_BINARY)
                // 64-bit binary output is impractical for reading and requires a huge buffer.
                // Restrict to 32 bits in binary mode.
                if ((base == 2) && (fflags & FF_XLONG))
                {
                    uvalue = (unsigned long) uvalue;
                }
#endif
#if FEATURE(USE_PRECISION)
                while (uvalue || precision > 0)
#else
                if (uvalue == 0)
                {
                    // Avoid printing 0 as ' '
                    *--p = '0';
    #if FEATURE(USE_ZERO_PAD)
                    --fwidth;
    #endif
                }
                while (uvalue)
#endif
                {
#if HEX_CONVERT_ONLY
                    c = (char) ((uvalue & 0x0f) + '0');
#else
                    c = (char) ((uvalue % base) + '0');
#endif
#if FEATURE(USE_HEX_LOWER) || FEATURE(USE_HEX_UPPER)
                    if (c > '9')
                    {
                        // Hex digits
    #if FEATURE(USE_HEX_LOWER) && FEATURE(USE_HEX_UPPER)
                        if (convert == 'X') c += 'A' - '0' - 10;
                        else                c += 'a' - '0' - 10;
    #elif FEATURE(USE_HEX_UPPER) || FEATURE(USE_HEX_UPPER_L)
                        c += 'A' - '0' - 10;
    #else
                        c += 'a' - '0' - 10;
    #endif
                    }
#endif
                    *--p = c;
#if HEX_CONVERT_ONLY
                    uvalue >>= 4;
#else
                    uvalue /= base;
#endif
#if FEATURE(USE_ZERO_PAD)
                    --fwidth;
#endif
#if FEATURE(USE_PRECISION)
                    --precision;
#endif
                }
#if FEATURE(USE_ZERO_PAD)
                // Allocate space for the sign bit.
                if (flags & (FL_PLUS|FL_NEG|FL_SPACE)) --fwidth;
    #if FEATURE(USE_SPECIAL)
                // Allocate space for special chars if required.
                if (flags & FL_SPECIAL)
                {
                    if (convert == 'o') fwidth -= 1;
                    else fwidth -= 2;
                }
    #endif
                // Add leading zero padding if required.
                if ((flags & FL_ZERO_PAD) && !(flags & FL_LEFT_JUST))
                {
                    while (fwidth > 0)
                    {
                        *--p = '0';
                        --fwidth;
                    }
                }
#endif
#if FEATURE(USE_SPECIAL)
                // Add special prefix if required.
                if (flags & FL_SPECIAL)
                {
                    if (convert != 'o') *--p = convert;
                    *--p = '0';
                }
#endif
                // Add the sign prefix
                if      (flags & FL_NEG)    *--p = '-';
#if FEATURE(USE_PLUS_SIGN)
                else if (flags & FL_PLUS)   *--p = '+';
#endif
#if FEATURE(USE_SPACE_SIGN)
                else if (flags & FL_SPACE)  *--p = ' ';
#endif
#if FEATURE(USE_PRECISION)
                // Precision is not used to limit number output.
                precision = -1;
#endif
                break;
#if FEATURE(USE_FLOAT)
            case 'f':
                fflags = FF_FCVT;
                goto fp_number;
            case 'E':
                fflags = FF_UCASE;
            case 'e':
                fflags |= FF_ECVT;
                goto fp_number;
            case 'G':
                fflags = FF_UCASE;
            case 'g':
                fflags |= FF_GCVT;
            fp_number:
                // Set default precision
                if (precision == -1) precision = 6;
                // Need one extra digit precision in E mode
                if (fflags & FF_ECVT) ++precision;
                fvalue = va_arg(ap, double);
                p = format_float(fvalue, precision, width, flags, fflags, buffer);
                // Precision is not used to limit number output.
                precision = -1;
                break;
#endif
#if FEATURE(USE_STRING)
    #if FEATURE(USE_FSTRING)
            case 'S':
                flags |= FL_FSTR;
                // fall through
    #endif
            case 's':
                p = va_arg(ap, char *);
                break;
#endif
            default:
                *--p = convert;
                break;
            }

#if FEATURE(USE_SPACE_PAD)
            // Check width of formatted text.
    #if FEATURE(USE_FSTRING)
            fwidth = p_len(p, flags);
    #else
            fwidth = p_len(p);
    #endif
            // Copy formatted text with leading or trailing space.
            // A positive value for precision will limit the length of p used.
            for (;;)
            {
    #if FEATURE(USE_FSTRING)
                if (flags & FL_FSTR)
                    c = GET_FORMAT(p);
                else
    #endif
                c = *p;
                if ((c == '\0' || precision == 0) && width <= 0) break;
                if (((flags & FL_LEFT_JUST) || width <= fwidth) && c && precision != 0) ++p;
                else c = ' ';
                // for loop continues after #endif
#else
            for (;;)
            {
    #if FEATURE(USE_FSTRING)
                if (flags & FL_FSTR)
                    c = GET_FORMAT(p);
                else
    #endif
                c = *p;
    #if FEATURE(USE_PRECISION)
                // A positive value for precision will limit the length of p used.
                if (c == '\0' || precision == 0) break;
    #else
                if (c == '\0') break;
    #endif
                ++p;
                // for loop continues after #endif
#endif
                // for loop continues here from either of the USE_SPACE_PAD cases.
#ifdef BASIC_PRINTF_ONLY
                func(c);            // Basic output function.
#else
// [NF_CHANGE]
                // only print if there is space in the buffer
                if (count < n)
                {
                    func(c, context);   // Output function.
                }
// [END NF_CHANGE]

#endif
#ifdef PRINTF_T
                ++count;

// [NF_CHANGE]
                if (count >= n)
                {
                    break;
                }
// [END NF_CHANGE]

#endif
#if FEATURE(USE_SPACE_PAD)
                --width;
#endif
#if FEATURE(USE_PRECISION)
                if (precision > 0) --precision;
#endif
            }
        }
        else
        {
#ifdef BASIC_PRINTF_ONLY
            func(convert);              // Basic output function.
#else
// [NF_CHANGE]
        // only print if there is space in the buffer
        if (count < n)
        {
            func(convert, context);     // Output function.
        }
// [END NF_CHANGE]
#endif
#ifdef PRINTF_T
            ++count;
#endif
        }
        ++fmt;
    }

#ifdef PRINTF_T
    return count;
#endif
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

/* ---------------------------------------------------------------------------
Function: putout()
This is the output function used for printf.
The context is normally required, but is not used at present. It could be
extended to include streams or to avoid output mixing in multi-threaded use.
If using BASIC_PRINTF, context is not supported.
--------------------------------------------------------------------------- */
#ifdef BASIC_PRINTF_ONLY
static void putout(char c)
{
#else
static void putout(char c, void *context)
{
    (void) context;     // Suppress compiler warning about unused argument.
#endif
    PUTCHAR_FUNC(c);
}

/* ---------------------------------------------------------------------------
Function: printf()
Replacement for library printf - writes to output (normally serial)
It uses the output function putout() to update the serial output.
If PRINTF_T is defined then the number of characters generated is returned.
--------------------------------------------------------------------------- */
printf_t printf_(const char *fmt, ...)
{
    va_list ap;
#ifdef PRINTF_T
    int Count;
#endif

    va_start(ap, fmt);
#ifdef PRINTF_T
  #ifdef BASIC_PRINTF_ONLY
    Count = doprnt(putout, fmt, ap);
  #else
    Count = doprnt((void *)0, putout, BUFMAX, fmt, ap);
  #endif
#else
  #ifdef BASIC_PRINTF_ONLY
    doprnt(putout, fmt, ap);
  #else
    doprnt((void *)0, putout, fmt, ap);
  #endif
#endif
    va_end(ap);
    
#ifdef PRINTF_T
    return Count;
#endif
}

#ifndef BASIC_PRINTF_ONLY
/* ---------------------------------------------------------------------------
Function: putbuf()
This is the output function used for sprintf.
Here the context is a pointer to a pointer to the buffer.
Double indirection allows the function to increment the buffer pointer.
--------------------------------------------------------------------------- */
static void putbuf(char c, void *context)
{
    char *buf = *((char **) context);
    *buf++ = c;
    *((char **) context) = buf;
}

/* ---------------------------------------------------------------------------
Function: sprintf()
Replacement for library sprintf - writes into buffer supplied.
Normally it uses the output function putout() to update the buffer.
sprintf is not supported when using BASIC_PRINTF
If PRINTF_T is defined then the number of characters generated is returned.
--------------------------------------------------------------------------- */
printf_t sprintf_(char *buf, const char *fmt, ... )
{
    va_list ap;
#ifdef PRINTF_T
    int Count;
#endif

    va_start(ap, fmt);
#ifdef PRINTF_T
    Count = doprnt(&buf, putbuf, BUFMAX, fmt, ap);
#else
    doprnt(&buf, putbuf, fmt, ap);
#endif
    va_end(ap);
    // Append null terminator.
    *buf = '\0';
    
#ifdef PRINTF_T
    return Count;
#endif
}
#endif

// [NF_CHANGE]
printf_t snprintf_(char *buf, size_t n,const char *fmt, ... )
{
    va_list ap;
    int count;

    va_start(ap, fmt);
    count = doprnt(&buf, putbuf, n - 1, fmt, ap);
    va_end(ap);

    // Append null terminator if there's space.
    if (n > 0)
    {
        *buf = '\0';
    }
    
   return count;
}
// [END_NF_CHANGE]

// [NF_CHANGE]
void putchar_(char character)
{
    (void)character;
}
// [END_NF_CHANGE]

// [NF_CHANGE]
printf_t vsnprintf_(char *buffer, size_t bufsz, char const *format, va_list vlist)
{
    size_t count;
    va_list vlistCopy;

    // Create a copy of the variable argument list
    va_copy(vlistCopy, vlist);

    // Perform the actual formatting operation
    count = doprnt(&buffer, putbuf, bufsz, format, vlistCopy);

    // Clean up the copied variable argument list
    va_end(vlistCopy);

    return count;
}
// [END_NF_CHANGE]

// clang-format on
