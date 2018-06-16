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
    
    struct Math
    {
        #if (DP_FLOATINGPOINT == TRUE)
            // double-precision floating point

            static double       Abs(double d);
            static double       Acos(double d);
            static double       Asin(double d);
            static double       Atan(double d);
            static double       Atan2(double x, double y);
            static double       Ceiling(double d);
            static double       Cos(double d);
            static double       Cosh(double d);
            static double       IEEERemainder(double x, double y );
            static double       Exp(double d);
            static double       Floor(double d);
            static double       Fmod(double x, double y );
            static double       Log(double d);
            static double       Log10(double d);
            static double       Max(double x, double y);
            static double       Min(double x, double y);
            static double       Pow(double x, double y);
            static double       Round(double d);
            static int32_t      Sign(double d);
            static double       Sin(double d);
            static double       Sinh(double d);
            static double       Sqrt(double d);
            static double       Tan(double d);
            static double       Tanh(double d);
            static double       Truncate(double d, double& integralValue);

        #else
            // single-precision floating point

            static float       Abs(float d);
            static float       Acos(float d);
            static float       Asin(float d);
            static float       Atan(float d);
            static float       Atan2(float x, float y);
            static float       Ceiling(float d);
            static float       Cos(float d);
            static float       Cosh(float d);
            static float       IEEERemainder(float x, float y );
            static float       Exp(float d);
            static float       Floor(float d);
            static float       Fmod(float x, float y );
            static float       Log(float d);
            static float       Log10(float d);
            static float       Max(float x, float y);
            static float       Min(float x, float y);
            static float       Pow(float x, float y);
            static float       Round(float d);
            static int32_t      Sign(float d);
            static float       Sin(float d);
            static float       Sinh(float d);
            static float       Sqrt(float d);
            static float       Tan(float d);
            static float       Tanh(float d);
            static float       Truncate(float d, float& integralValue);
      
        #endif // DP_FLOATINGPOINT
    };
}
#endif  //_NANOPAL_DOUBLE_DECL_H_
