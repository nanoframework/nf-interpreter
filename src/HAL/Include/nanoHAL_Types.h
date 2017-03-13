//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _HAL_TYPES_H_
#define _HAL_TYPES_H_ 1

#include <netmf_errors.h>

#include <stddef.h>

#if defined (__GNUC__)
#define __int64 long long
#undef NULL
#endif

#if defined(__arm) || defined(__GNUC__)

#undef UNICODE

typedef unsigned int BOOL;

#define TRUE  1
#define FALSE 0

typedef unsigned char      BYTE;
typedef unsigned char*     PBYTE;

typedef unsigned char      UINT8;
typedef signed   char      INT8;

typedef unsigned short int UINT16;
typedef signed   short int INT16;

typedef unsigned int       UINT32;
typedef signed   int       INT32;

typedef unsigned int       ULONG;
typedef signed   int       LONG;

typedef unsigned __int64   UINT64;
typedef signed   __int64   INT64;

#define NULL 0

typedef char               CHAR;
typedef char*              LPSTR;
typedef const char*        LPCSTR;
typedef unsigned short     WORD;
typedef unsigned long      DWORD;


typedef wchar_t            WCHAR;
typedef WCHAR*             LPWSTR;
typedef const WCHAR*       LPCWSTR;

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
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;

#endif // !(defined(_WIN32) || defined(WIN32))

#endif  // _HAL_TYPES_H_

