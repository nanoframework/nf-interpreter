//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _HAL_TYPES_H_
#define _HAL_TYPES_H_ 1

#include <nf_errors_exceptions.h>

#include <stddef.h>

#if defined (__GNUC__)
#define __int64 long long
#undef NULL
#endif

#if defined(__arm) || defined(__GNUC__)

#undef UNICODE

#define NULL 0

#endif //defined(__arm) || defined(__GNUC__)

#define ARRAYSIZE_CONST_EXPR(x) (sizeof(x)/sizeof(x[0]))
#if (!defined(_WIN32) && !defined(WIN32))
#define ARRAYSIZE(x) ARRAYSIZE_CONST_EXPR(x)
#endif
#define MAXSTRLEN(x) (ARRAYSIZE(x)-1)
#define ROUNDTOMULTIPLE(x,y)           ((x + sizeof(y) - 1) & ~(sizeof(y)-1)) // Only works with powers of 2.
#define CONVERTFROMSIZETOELEMENTS(x,y) ((x + sizeof(y) - 1) /   sizeof(y))
#define CONVERTFROMSIZETOHEAPBLOCKS(x) CONVERTFROMSIZETOELEMENTS(x,CLR_RT_HeapBlock)

//--//

#if !(defined(_WIN32) || defined(WIN32))

typedef struct SYSTEMTIME
{
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
} SYSTEMTIME;

#endif // !(defined(_WIN32) || defined(WIN32))

#endif  // _HAL_TYPES_H_

