//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2014-2019 Marco Paland (info@paland.com). All rights reserved.
// Portions Copyright (c) 2021-2022 Eyal Rozenberg <eyalroz1@gmx.com>. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef NANOPRINTF_H
#define NANOPRINTF_H

#include <stdarg.h>
#include <stddef.h>

#define PRINTF_SUPPORT_DECIMAL_SPECIFIERS            1
#define PRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS        1
#define PRINTF_SUPPORT_WRITEBACK_SPECIFIER           1
#define PRINTF_SUPPORT_MSVC_STYLE_INTEGER_SPECIFIERS 0
#define PRINTF_SUPPORT_LONG_LONG                     1
#define PRINTF_MAX_INTEGRAL_DIGITS_FOR_DECIMAL       15
#define PRINTF_CHECK_FOR_NUL_IN_FORMAT_SPECIFIER     0
#define PRINTF_ALIAS_STANDARD_FUNCTION_NAMES         1
#define PRINTF_ALIAS_STANDARD_FUNCTION_NAMES_SOFT    1
#define PRINTF_ALIAS_STANDARD_FUNCTION_NAMES_HARD    0

#ifdef __cplusplus
#include <cstdarg>
#include <cstddef>
extern "C"
{
#else
#include <stdarg.h>
#include <stddef.h>
#endif

#ifdef __GNUC__
#if ((__GNUC__ == 4 && __GNUC_MINOR__ >= 4) || __GNUC__ > 4)
#define ATTR_PRINTF(one_based_format_index, first_arg)                                                                 \
    __attribute__((format(gnu_printf, (one_based_format_index), (first_arg))))
#else
#define ATTR_PRINTF(one_based_format_index, first_arg)                                                                 \
    __attribute__((format(printf, (one_based_format_index), (first_arg))))
#endif
#define ATTR_VPRINTF(one_based_format_index) ATTR_PRINTF((one_based_format_index), 0)
#else
#define ATTR_PRINTF(one_based_format_index, first_arg)
#define ATTR_VPRINTF(one_based_format_index)
#endif

#ifndef PRINTF_ALIAS_STANDARD_FUNCTION_NAMES
#define PRINTF_ALIAS_STANDARD_FUNCTION_NAMES 0
#endif

#if PRINTF_ALIAS_STANDARD_FUNCTION_NAMES_HARD
#define printf_    printf
#define sprintf_   sprintf
#define vsprintf_  vsprintf
#define snprintf_  snprintf
#define vsnprintf_ vsnprintf
#define vprintf_   vprintf
#endif

// If you want to include this implementation file directly rather than
// link against, this will let you control the functions' visibility,
// e.g. make them static so as not to clash with other objects also
// using them.
#ifndef PRINTF_VISIBILITY
#define PRINTF_VISIBILITY
#endif

    /**
     * Prints/send a single character to some opaque output entity
     *
     * @note This function is not implemented by the library, only declared; you must provide an
     * implementation if you wish to use the @ref printf / @ref vprintf function (and possibly
     * for linking against the library, if your toolchain does not support discarding unused functions)
     *
     * @note The output could be as simple as a wrapper for the `write()` system call on a Unix-like
     * system, or even libc's @ref putchar , for replicating actual functionality of libc's @ref printf
     * function; but on an embedded system it may involve interaction with a special output device,
     * like a UART, etc.
     *
     * @note in libc's @ref putchar, the parameter type is an int; this was intended to support the
     * representation of either a proper character or EOF in a variable - but this is really not
     * meaningful to pass into @ref putchar and is discouraged today. See further discussion in:
     * @link https://stackoverflow.com/q/17452847/1593077
     *
     * @param c the single character to print
     */
    PRINTF_VISIBILITY
    void putchar_(char c);

    /**
     * An implementation of the C standard's printf/vprintf
     *
     * @note you must implement a @ref putchar_ function for using this function - it invokes @ref putchar_
     * rather than directly performing any I/O (which insulates it from any dependence on the operating system
     * and external libraries).
     *
     * @param format A string specifying the format of the output, with %-marked specifiers of how to interpret
     * additional arguments.
     * @param arg Additional arguments to the function, one for each %-specifier in @p format string
     * @return The number of characters written into @p s, not counting the terminating null character
     */
    ///@{
    PRINTF_VISIBILITY
    int printf_(const char *format, ...) ATTR_PRINTF(1, 2);
    PRINTF_VISIBILITY
    int vprintf_(const char *format, va_list arg) ATTR_VPRINTF(1);
    ///@}

    /**
     * An implementation of the C standard's sprintf/vsprintf
     *
     * @note For security considerations (the potential for exceeding the buffer bounds), please consider using
     * the size-constrained variant, @ref snprintf / @ref vsnprintf , instead.
     *
     * @param s An array in which to store the formatted string. It must be large enough to fit the formatted
     * output!
     * @param format A string specifying the format of the output, with %-marked specifiers of how to interpret
     * additional arguments.
     * @param arg Additional arguments to the function, one for each specifier in @p format
     * @return The number of characters written into @p s, not counting the terminating null character
     */
    ///@{
    PRINTF_VISIBILITY
    int sprintf_(char *s, const char *format, ...) ATTR_PRINTF(2, 3);
    PRINTF_VISIBILITY
    int vsprintf_(char *s, const char *format, va_list arg) ATTR_VPRINTF(2);
    ///@}

    /**
     * An implementation of the C standard's snprintf/vsnprintf
     *
     * @param s An array in which to store the formatted string. It must be large enough to fit either the
     * entire formatted output, or at least @p n characters. Alternatively, it can be NULL, in which case
     * nothing will be printed, and only the number of characters which _could_ have been printed is
     * tallied and returned.
     * @param n The maximum number of characters to write to the array, including a terminating null character
     * @param format A string specifying the format of the output, with %-marked specifiers of how to interpret
     * additional arguments.
     * @param arg Additional arguments to the function, one for each specifier in @p format
     * @return The number of characters that COULD have been written into @p s, not counting the terminating
     *         null character. A value equal or larger than @p n indicates truncation. Only when the returned value
     *         is non-negative and less than @p n, the null-terminated string has been fully and successfully printed.
     */
    ///@{
    PRINTF_VISIBILITY
    int snprintf_(char *s, size_t count, const char *format, ...) ATTR_PRINTF(3, 4);
    PRINTF_VISIBILITY
    int vsnprintf_(char *s, size_t count, const char *format, va_list arg) ATTR_VPRINTF(3);
    ///@}

    /**
     * printf/vprintf with user-specified output function
     *
     * An alternative to @ref printf_, in which the output function is specified dynamically
     * (rather than @ref putchar_ being used)
     *
     * @param out An output function which takes one character and a type-erased additional parameters
     * @param extra_arg The type-erased argument to pass to the output function @p out with each call
     * @param format A string specifying the format of the output, with %-marked specifiers of how to interpret
     * additional arguments.
     * @param arg Additional arguments to the function, one for each specifier in @p format
     * @return The number of characters for which the output f unction was invoked, not counting the terminating null
     * character
     *
     */
    PRINTF_VISIBILITY
    int fctprintf(void (*out)(char c, void *extra_arg), void *extra_arg, const char *format, ...) ATTR_PRINTF(3, 4);
    PRINTF_VISIBILITY
    int vfctprintf(void (*out)(char c, void *extra_arg), void *extra_arg, const char *format, va_list arg)
        ATTR_VPRINTF(3);

#ifdef __cplusplus
} // extern "C"
#endif

#if PRINTF_ALIAS_STANDARD_FUNCTION_NAMES_HARD
#undef printf_
#undef sprintf_
#undef vsprintf_
#undef snprintf_
#undef vsnprintf_
#undef vprintf_
#else
#if PRINTF_ALIAS_STANDARD_FUNCTION_NAMES_SOFT
#define printf    printf_
#define sprintf   sprintf_
#define vsprintf  vsprintf_
#define snprintf  snprintf_
#define vsnprintf vsnprintf_
#define vprintf   vprintf_
#endif
#endif

#endif // NANOPRINTF_H
