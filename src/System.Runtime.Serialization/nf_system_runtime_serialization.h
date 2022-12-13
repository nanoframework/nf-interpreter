//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_SYSTEM_RUNTIME_SERIALIZATION_H
#define NF_SYSTEM_RUNTIME_SERIALIZATION_H

#include <Core.h>
#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>

// declared as struct SerializationHintsAttribute in src\CLR\Include\nanoCLR_Runtime__Serialization.h
// typedef enum __nfpack SerializationOptions
// {
//     SerializationOptions_PointerNeverNull = 16,
//     SerializationOptions_ElementsNeverNull = 32,
//     SerializationOptions_FixedType = 256,
// } SerializationOptions;

struct Library_nf_system_runtime_serialization_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter
{
    NANOCLR_NATIVE_DECLARE(Serialize___STATIC__SZARRAY_U1__OBJECT);
    NANOCLR_NATIVE_DECLARE(Deserialize___STATIC__OBJECT__SZARRAY_U1);

    //--//
};

struct Library_nf_system_runtime_serialization_System_Runtime_Serialization_SerializationException
{
    static const int FIELD__Type = 5;

    //--//
};

struct Library_nf_system_runtime_serialization_System_Runtime_Serialization_SerializationHintsAttribute
{
    static const int FIELD__Options = 1;
    static const int FIELD__ArraySize = 2;
    static const int FIELD__BitPacked = 3;
    static const int FIELD__RangeBias = 4;
    static const int FIELD__Scale = 5;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Runtime_Serialization;

#endif // NF_SYSTEM_RUNTIME_SERIALIZATION_H
