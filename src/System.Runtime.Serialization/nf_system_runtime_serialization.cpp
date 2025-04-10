//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_system_runtime_serialization.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nf_system_runtime_serialization_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter::Serialize___STATIC__SZARRAY_U1__OBJECT,
    Library_nf_system_runtime_serialization_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter::Deserialize___STATIC__OBJECT__SZARRAY_U1,
    nullptr,
    nullptr,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Runtime_Serialization =
{
    "System.Runtime.Serialization",
    0x0A066871,
    method_lookup,
    { 100, 0, 0, 0 }
};

// clang-format on
