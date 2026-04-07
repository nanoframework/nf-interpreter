//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006 - 2021 Skirrid Systems. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NANOPRINTF_H
#define NANOPRINTF_H

#include <stdarg.h>

// clang-format off

/*************************************************************************
Number of chars output

Traditionally printf returns the number of chars output. If you are not
interested in that value you can leave PRINTF_T undefined.
On a small micro you can define the return type as unsigned char if you
are sure the total output width will never exceed 255, or unsigned short.
*************************************************************************/

#define PRINTF_T size_t

/*************************************************************************
Memory access definitions

Some micros such as the AVR can only support storing strings in flash
memory by wrapping the string in a macro. To make this transparent we can
define the printf function itself as a macro which performs the wrap and
calls a renamed version of printf with an _ suffix and no i.
*************************************************************************/

/*
Example for AVR micros using GCC toolchain from WinAVR or Atmel Studio

#define sprintf(buf, format, args...)   _sprntf(buf, PSTR(format), ## args)
#define printf(format, args...)         _prntf(PSTR(format), ## args)

extern printf_t _sprntf(char *, const char *, ...);
extern printf_t _prntf(const char *, ...);
*/

/*************************************************************************
End of customisations - Stop Editing!

The remainder of this file contains the function declarations.
*************************************************************************/

// Create a type definition for the return value
#ifndef PRINTF_T
typedef void printf_t;
#else
typedef PRINTF_T printf_t;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// Function declarations, unless macros have been defined above
printf_t printf_(const char *, ...);
printf_t sprintf_(char *, const char *, ...);
printf_t snprintf_(char *, size_t n, const char *, ...);
printf_t vsnprintf_(char *, size_t n, const char *, va_list);

#ifdef __cplusplus
}
#endif

    // from config

/*************************************************************************
Basic printf only

The code is designed to support a variety of printf-related functions.
If simple serial output is all you want then you can save some space by
defining BASIC_PRINTF_ONLY which allows the internal API to be simplified.
Note that sprintf will not be supported in this case.
*************************************************************************/

// #define BASIC_PRINTF_ONLY

/*************************************************************************
Memory access definitions

Some micros such as the AVR can only support storing and accessing strings
in flash memory using special macros and functions. This section can be
used to specify those methods. You may also need to modify printf.h
to get the compiler to place the format strings in flash memory.

The GET_FORMAT(ptr) macro is used to access a character in the printf
format string. By default this does a normal memory pointer access, but
you can configure it to access flash memory if needed.
*************************************************************************/

/*
Example for AVR micros using GCC toolchain from WinAVR or Atmel Studio

#include <avr/io.h>
#include <avr/pgmspace.h>
#define GET_FORMAT(p)   pgm_read_byte(p)
*/

/*************************************************************************
Output configuration

By default printf will use the putchar function. If this is not defined
in your system you can set your own function here by defining
PUTCHAR_FUNC to be the name of that function.
*************************************************************************/

void putchar_(char character);
#define PUTCHAR_FUNC putchar_

/*************************************************************************
Compiler capability configuration

Set some options that the C pre-processor will not tell us about.
*************************************************************************/

// Does the compiler support double precision or silently degrade to single?
//#define NO_DOUBLE_PRECISION

// Does the compiler support isnan and isinf floating point functions?
// #define NO_ISNAN_ISINF

/*************************************************************************
Formatted item width

Since it is extremely unlikely that you will ever want to use a formatted
width for a single item of more than 127 chars (i.e. the expanded and
padded size of a single % expression), the width variables can normally
be restricted to 8 bits. On small micros this saves a lot of code and
variable space. On a 32-bit RISC it may increase code size due to type
conversions. Choose the variable type to suit your CPU.
Note that a signed type is required.
*************************************************************************/

typedef signed char width_t;

/*************************************************************************
Feature configuration

This section defines the individual feature flags.
These are combined as needed to produce the FEATURE_FLAGS macro.
*************************************************************************/

// Include floating point number support
#define USE_FLOAT (1 << 0)

// Include support for long integers
#define USE_LONG (1 << 1)

// Include support for octal formatting
#define USE_OCTAL (1 << 2)

// Include support for %d decimal formatting
#define USE_SIGNED (1 << 3)

// Include support for the %i synonym for %d
#define USE_SIGNED_I (1 << 4)

// Include support for the %u unsigned decimal specifier
#define USE_UNSIGNED (1 << 5)

// Include support for the %x hex specifier (lowercase output)
#define USE_HEX_LOWER (1 << 6)

// Include support for the %X hex specifier (uppercase output)
#define USE_HEX_UPPER (1 << 7)

// Force uppercase output with %x.
// Used in conjunction with USE_HEX_LOWER.
// Ignored if USE_HEX_UPPER is also set.
#define USE_HEX_UPPER_L (1 << 8)

// Include support for %c single character
#define USE_CHAR (1 << 9)

// Include support for %s string
#define USE_STRING (1 << 10)

// Include support for %S string in flash memory
// Only needed for architectures which cannot access program memory using normal pointers.
// If you have not defined the GET_FORMAT() macro above then you don't need this option.
#define USE_FSTRING (1 << 11)

// Include support for %b binary specifier
#define USE_BINARY (1 << 12)

// Include precision support when floating point is not present.
// Precision is automatically enabled when floating point support is used.
#define USE_PRECISION (1UL << 16)

// Allow use of leading zero padding e.g. "%03d"
#define USE_ZERO_PAD (1UL << 17)

// Allow use of space padding e.g. "%3d" or "%12s"
#define USE_SPACE_PAD (1UL << 18)

// Include indirect width/precision support e.g. "%*d"
#define USE_INDIRECT (1UL << 19)

// Allow forcing a leading plus sign e.g. "%+3d"
#define USE_PLUS_SIGN (1UL << 20)

// Allow forcing a leading space (instead of + or -) in front of zero e.g. "% 3d"
#define USE_SPACE_SIGN (1UL << 21)

// Include support for the left-justify '-' flag.
#define USE_LEFT_JUST (1UL << 22)

// Include support for the special '#' flag.
#define USE_SPECIAL (1UL << 23)

// Use smaller but less efficient floating point normalisation.
// This is not recommended unless code space is critically low.
#define USE_SMALL_FLOAT (1UL << 24)

// Include support for 64-bit integers e.g. "%lld"
#define USE_LONG_LONG (1UL << 25)

/*************************************************************************
Pre-defined feature sets

This section provides some commonly used combinations of features.
*************************************************************************/

// Lowercase hex integers only. This really is the bare minimum.
#define HEX_INT (USE_HEX_LOWER)

// Decimal and lowercase hex only.
#define MINIMAL_INT (USE_SIGNED | USE_HEX_LOWER)

// Signed and unsigned decimal, lower case hex, zero & space padding, plus char and string
#define BASIC_INT (USE_CHAR | USE_STRING | USE_SIGNED | USE_UNSIGNED | USE_HEX_LOWER | USE_ZERO_PAD | USE_SPACE_PAD)

// All short integer features except octal, binary, %i, indirection and specials.
#define SHORT_INT                                                                                                      \
    (USE_CHAR | USE_STRING | USE_SIGNED | USE_UNSIGNED | USE_HEX_LOWER | USE_HEX_UPPER | USE_PRECISION |               \
     USE_ZERO_PAD | USE_SPACE_PAD | USE_PLUS_SIGN | USE_SPACE_SIGN | USE_LEFT_JUST)

// As above, but also supports long integers.
#define LONG_INT (USE_LONG | SHORT_INT)

// As above, but also supports long-long integers.
#define LONG_LONG_INT (USE_LONG_LONG | LONG_INT)

// All possible integer features.
#define FULL_INT (USE_BINARY | USE_OCTAL | USE_SIGNED_I | USE_INDIRECT | USE_SPECIAL | LONG_LONG_INT)

// All available features including floating point.
#define FULL_FLOAT (USE_FLOAT | FULL_INT)

/*************************************************************************
Features included in your build of printf. Use only the features you need
to keep code size and execution time to a minimum.

You can use the custom set, with anything you don't want commented out,
or you can use one of the pre-defined sets.

Examples:

#define FEATURE_FLAGS   CUSTOM_SET
#define FEATURE_FLAGS   SHORT_INT

Features and pre-defined sets are set out in the following sections.
*************************************************************************/

// Custom feature set. Comment out features you don't want.
#define CUSTOM_SET                                                                                                     \
    (0 | USE_FLOAT | USE_LONG | USE_BINARY | USE_OCTAL | USE_SIGNED | USE_SIGNED_I | USE_UNSIGNED | USE_HEX_LOWER |    \
     USE_HEX_UPPER | USE_HEX_UPPER_L | USE_CHAR | USE_STRING | USE_FSTRING | USE_PRECISION | USE_ZERO_PAD |            \
     USE_SPACE_PAD | USE_INDIRECT | USE_PLUS_SIGN | USE_SPACE_SIGN | USE_LEFT_JUST | USE_SPECIAL | USE_SMALL_FLOAT |   \
     USE_LONG_LONG)

#define NANOFRAMEWORK_SET ( 0 \
        | USE_FLOAT       \
        | USE_LONG        \
        | USE_BINARY      \
        | USE_OCTAL       \
        | USE_SIGNED      \
        | USE_SIGNED_I    \
        | USE_UNSIGNED    \
        | USE_HEX_LOWER   \
        | USE_HEX_UPPER   \
        | USE_HEX_UPPER_L \
        | USE_CHAR        \
        | USE_STRING      \
        | USE_FSTRING     \
        | USE_PRECISION   \
        | USE_ZERO_PAD    \
        | USE_SPACE_PAD   \
        | USE_INDIRECT    \
        | USE_PLUS_SIGN   \
        | USE_SPACE_SIGN  \
        | USE_LEFT_JUST   \
        | USE_SPECIAL     \
        | USE_LONG_LONG   \
)

#define FEATURE_FLAGS NANOFRAMEWORK_SET

/*************************************************************************
End of customisations - Stop Editing!
*************************************************************************/

#define printf      printf_
#define sprintf     sprintf_
#define snprintf    snprintf_
#define vsnprintf   vsnprintf_

#endif // NANOPRINTF_H

// clang-format on
