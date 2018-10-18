//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOPAL_DOUBLE_DECL_H_
#define _NANOPAL_DOUBLE_DECL_H_

#include <nanoHAL.h>

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
