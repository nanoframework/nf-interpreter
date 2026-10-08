//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_system_runtime_serialization.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nf_system_runtime_serialization_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter::Deserialize___STATIC__OBJECT__SZARRAY_U1,
    Library_nf_system_runtime_serialization_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter::Serialize___STATIC__SZARRAY_U1__OBJECT,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Runtime_Serialization =
{
    "System.Runtime.Serialization",
    0xBFD148C5,
    method_lookup,
    ARRAYSIZE(method_lookup)
};

// clang-format on
