//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_native_system_math.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    Library_nf_native_system_math_System_Math::Abs___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Abs___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Acos___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Acos___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Asin___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Asin___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Atan___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Atan___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Atan2___STATIC__R8__R8__R8,
    Library_nf_native_system_math_System_Math::Atan2___STATIC__R4__R4__R4,
    Library_nf_native_system_math_System_Math::Ceiling___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Ceiling___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Cos___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Cos___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Cosh___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Cosh___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::IEEERemainder___STATIC__R8__R8__R8,
    Library_nf_native_system_math_System_Math::IEEERemainder___STATIC__R4__R4__R4,
    Library_nf_native_system_math_System_Math::Exp___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Exp___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Floor___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Floor___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Log___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Log___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Log10___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Log10___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Max___STATIC__R8__R8__R8,
    Library_nf_native_system_math_System_Math::Max___STATIC__R4__R4__R4,
    Library_nf_native_system_math_System_Math::Min___STATIC__R8__R8__R8,
    Library_nf_native_system_math_System_Math::Min___STATIC__R4__R4__R4,
    Library_nf_native_system_math_System_Math::Pow___STATIC__R8__R8__R8,
    Library_nf_native_system_math_System_Math::Pow___STATIC__R4__R4__R4,
    Library_nf_native_system_math_System_Math::Round___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Round___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Sign___STATIC__I4__R8,
    Library_nf_native_system_math_System_Math::Sign___STATIC__I4__R4,
    Library_nf_native_system_math_System_Math::Sin___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Sin___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Sinh___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Sinh___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Sqrt___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Sqrt___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Tan___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Tan___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Tanh___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Tanh___STATIC__R4__R4,
    Library_nf_native_system_math_System_Math::Truncate___STATIC__R8__R8,
    Library_nf_native_system_math_System_Math::Truncate___STATIC__R4__R4,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Math =
{
    "System.Math", 
    0x4BDCF00F,
    method_lookup,
    { 1, 0, 4, 3 }
};
