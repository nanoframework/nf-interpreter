//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOPAL_DOUBLE_DECL_H_
#define _NANOPAL_DOUBLE_DECL_H_

#include <nanoHAL.h>

#include <math.h>

#ifdef WIN32
#include <float.h>

#define __isnand        _isnan
#define __isinfd(x)    (!_finite(x))

inline int __signbitd(double x)
{
  unsigned char *ptr = (unsigned char*) &x;
  return (*(ptr + (sizeof(double) - 1)) & 0x80) != 0;
}

#define rint(x)         floor((x) + 0.5) 
#define remainder(x,y) ((x) - ((y) * rint((x) / (y))))

#define isgreater(param0,param1) (param0 > param1)
#define isless(param0,param1)    (param0 < param1)

#elif defined(__GNUC__)

#if !defined( isgreater )
#define isgreater   __builtin_isgreater
#endif

#if !defined( isless )
#define isless      __builtin_isless
#endif

#if !defined( __isnand )
#define __isnand    __builtin_isnan
#endif

#if !defined( __isinfd )
#define __isinfd    __builtin_isinf
#endif

#if !defined( __signbitd )
#define __signbitd  __builtin_signbit
#endif

#endif

namespace System
{
    struct Double
    {
        static int32_t      CompareTo( double d, double val );
        static bool         IsInfinity( double d );
        static bool         IsNaN( double d );
        static bool         IsNegativeInfinity( double d );
        static bool         IsPositiveInfinity( double d );
    };
}

#endif  //_NANOPAL_DOUBLE_DECL_H_
