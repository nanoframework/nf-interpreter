//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2001, 2002 Georges Menie. All rights reserved.
// Portions Copyright (c) 2009-2013 Daniel D Miller. All rights reserved.
// See LICENSE file in the project root for full license information.
//

/*
    The interface of nanoprintf begins here, to be compiled only if
    NANOPRINTF_IMPLEMENTATION is not defined. In a multi-file library what
    follows would be the public-facing nanoprintf.h.
*/

#ifndef NANOPRINTF_H
#define NANOPRINTF_H

#include <stdarg.h>
#include <stddef.h>

/* Define this to fully sandbox nanoprintf inside of a translation unit. */
#ifdef NANOPRINTF_VISIBILITY_STATIC
#define NPF_VISIBILITY static
#else
#define NPF_VISIBILITY extern
#endif

/* Compilers can warn when printf formatting strings are incorrect. */
#if defined(__clang__)
#define NPF_PRINTF_ATTR(FORMAT_INDEX, VARGS_INDEX) __attribute__((__format__(__printf__, FORMAT_INDEX, VARGS_INDEX)))
#elif defined(__GNUC__) || defined(__GNUG__)
#define NPF_PRINTF_ATTR(FORMAT_INDEX, VARGS_INDEX) __attribute__((format(printf, FORMAT_INDEX, VARGS_INDEX)))
#else
#define NPF_PRINTF_ATTR(FORMAT_INDEX, VARGS_INDEX)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /* Public API */

    NPF_VISIBILITY int npf_snprintf(char *buffer, size_t bufsz, const char *format, ...) NPF_PRINTF_ATTR(3, 4);

    NPF_VISIBILITY int npf_vsnprintf(char *buffer, size_t bufsz, char const *format, va_list vlist)
        NPF_PRINTF_ATTR(3, 0);

    typedef void (*npf_putc)(int c, void *ctx);
    NPF_VISIBILITY int npf_pprintf(npf_putc pc, void *pc_ctx, char const *format, ...) NPF_PRINTF_ATTR(3, 4);

    NPF_VISIBILITY int npf_vpprintf(npf_putc pc, void *pc_ctx, char const *format, va_list vlist) NPF_PRINTF_ATTR(3, 0);

/* Public Configuration */

/* Pick reasonable defaults if nothing's been configured. */
#if !defined(NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS) && !defined(NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS) &&  \
    !defined(NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS) && !defined(NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS) &&            \
    !defined(NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS)
#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS   1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS       1
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS       1
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS   0
#endif

/* If anything's been configured, everything must be configured. */
#ifndef NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS
#error NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS must be #defined to 0 or 1
#endif

#ifndef NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS
#error NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS must be #defined to 0 or 1
#endif

#ifndef NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS
#error NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS must be #defined to 0 or 1
#endif

#ifndef NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS
#error NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS must be #defined to 0 or 1
#endif

#ifndef NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS
#error NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS must be #defined to 0 or 1
#endif

/* Ensure flags are compatible. */
#if (NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS == 1) && (NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS == 0)
#error Precision format specifiers must be enabled if float support is enabled.
#endif

/* intmax_t / uintmax_t require stdint from c99 / c++11 */
#ifndef _MSC_VER
#ifdef __cplusplus
#if __cplusplus < 201103L
#error nanoprintf requires C++11 or later.
#endif
#else
#if __STDC_VERSION__ < 199409L
#error nanoprintf requires C99 or later.
#endif
#endif
#endif

    /* Implementation Details (prototype / config helper functions) */

#include <inttypes.h>
#include <stdint.h>

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
    typedef enum
    {
        NPF_FMT_SPEC_FIELD_WIDTH_NONE,
        NPF_FMT_SPEC_FIELD_WIDTH_STAR,
        NPF_FMT_SPEC_FIELD_WIDTH_LITERAL
    } npf__format_spec_field_width_t;
#endif

#if NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS == 1
    typedef enum
    {
        NPF_FMT_SPEC_PRECISION_NONE,
        NPF_FMT_SPEC_PRECISION_STAR,
        NPF_FMT_SPEC_PRECISION_LITERAL
    } npf__format_spec_precision_t;
#endif

    typedef enum
    {
        NPF_FMT_SPEC_LEN_MOD_NONE,
        NPF_FMT_SPEC_LEN_MOD_SHORT,       /* 'h' */
        NPF_FMT_SPEC_LEN_MOD_LONG,        /* 'l' */
        NPF_FMT_SPEC_LEN_MOD_LONG_DOUBLE, /* 'L' */
        NPF_FMT_SPEC_LEN_MOD_CHAR         /* 'hh' */
#if NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS == 1
        ,
        NPF_FMT_SPEC_LEN_MOD_LARGE_LONG_LONG, /* 'll' */
        NPF_FMT_SPEC_LEN_MOD_LARGE_INTMAX,    /* 'j' */
        NPF_FMT_SPEC_LEN_MOD_LARGE_SIZET,     /* 'z' */
        NPF_FMT_SPEC_LEN_MOD_LARGE_PTRDIFFT   /* 't' */
#endif
    } npf__format_spec_length_modifier_t;

    typedef enum
    {
        NPF_FMT_SPEC_CONV_PERCENT,      /* '%' */
        NPF_FMT_SPEC_CONV_CHAR,         /* 'c' */
        NPF_FMT_SPEC_CONV_STRING,       /* 's' */
        NPF_FMT_SPEC_CONV_SIGNED_INT,   /* 'i', 'd' */
        NPF_FMT_SPEC_CONV_OCTAL,        /* 'o' */
        NPF_FMT_SPEC_CONV_HEX_INT,      /* 'x', 'X' */
        NPF_FMT_SPEC_CONV_UNSIGNED_INT, /* 'u' */
        NPF_FMT_SPEC_CONV_POINTER       /* 'p' */
#if NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS == 1
        ,
        NPF_FMT_SPEC_CONV_WRITEBACK /* 'n' */
#endif
#if NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS == 1
        ,
        NPF_FMT_SPEC_CONV_FLOAT_DECIMAL /* 'f', 'F' */
#endif
    } npf__format_spec_conversion_t;

    typedef enum
    {
        NPF_FMT_SPEC_CONV_CASE_LOWER,
        NPF_FMT_SPEC_CONV_CASE_UPPER
    } npf__format_spec_conversion_case_t;

    typedef struct
    {
        /* optional flags */
        char prepend_sign;     /* '+' */
        char prepend_space;    /* ' ' */
        char alternative_form; /* '#' */

#if NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS == 1
        /* field width */
        npf__format_spec_field_width_t field_width_type;
        int field_width;
        char left_justified;   /* '-' */
        char leading_zero_pad; /* '0' */
#endif

#if NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS == 1
        /* precision */
        npf__format_spec_precision_t precision_type;
        int precision;
#endif

        /* length modifier for specifying argument size */
        npf__format_spec_length_modifier_t length_modifier;

        /* conversion specifiers */
        npf__format_spec_conversion_t conv_spec;
        npf__format_spec_conversion_case_t conv_spec_case;
    } npf__format_spec_t;

#if NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS == 0
    typedef long npf__int_t;
    typedef unsigned long npf__uint_t;
#else
typedef intmax_t npf__int_t;
typedef uintmax_t npf__uint_t;
#endif

    NPF_VISIBILITY int npf__parse_format_spec(char const *format, npf__format_spec_t *out_spec);

    typedef struct
    {
        char *dst;
        size_t len;
        size_t cur;
    } npf__bufputc_ctx_t;

    NPF_VISIBILITY void npf__bufputc(int c, void *ctx);
    NPF_VISIBILITY void npf__bufputc_nop(int c, void *ctx);
    NPF_VISIBILITY int npf__itoa_rev(char *buf, npf__int_t i);
    NPF_VISIBILITY int npf__utoa_rev(char *buf, npf__uint_t i, unsigned base, npf__format_spec_conversion_case_t cc);
#if NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS == 1
    NPF_VISIBILITY int npf__dsplit_abs(
        double d,
        uint64_t *out_int_part,
        uint64_t *out_frac_part,
        int *out_frac_base10_neg_e);
    NPF_VISIBILITY int npf__dtoa_rev(
        char *buf,
        double d,
        unsigned base,
        npf__format_spec_conversion_case_t cc,
        int *out_frac_chars);
#endif

#ifdef __cplusplus
}
#endif

// do not define sprintf, it is not safe
//#define sprintf snprintf(buff, ARRAYSIZE(buff), fmt, ##__VA_ARGS__)

// the defines below allow using the regular calls to *snprintf
#define snprintf  npf_snprintf
#define vsnprintf npf_vsnprintf

#endif // NANOPRINTF_H
