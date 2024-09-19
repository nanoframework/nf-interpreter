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

extern printf_t snprintf_(char *buf, size_t n,const char *fmt, ... );
extern printf_t vsnprintf_(char *buffer, size_t bufsz, char const *format, va_list vlist);

#define printf      printf_
#define sprintf     sprintf_
#define snprintf    snprintf_
#define vsnprintf   vsnprintf_

#ifdef __cplusplus
}
#endif

#endif // NANOPRINTF_H

// clang-format on
