//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_system_text.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nf_system_text_System_Text_UTF8Decoder::Convert___VOID__SZARRAY_U1__I4__I4__SZARRAY_CHAR__I4__I4__BOOLEAN__BYREF_I4__BYREF_I4__BYREF_BOOLEAN,
    Library_nf_system_text_System_Text_UTF8Encoding::GetBytes___I4__STRING__I4__I4__SZARRAY_U1__I4,
    Library_nf_system_text_System_Text_UTF8Encoding::GetBytes___SZARRAY_U1__STRING,
    Library_nf_system_text_System_Text_UTF8Encoding::GetChars___SZARRAY_CHAR__SZARRAY_U1,
    Library_nf_system_text_System_Text_UTF8Encoding::GetChars___SZARRAY_CHAR__SZARRAY_U1__I4__I4,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Text =
{
    "nanoFramework.System.Text",
    0x6CD03604,
    method_lookup,
    ARRAYSIZE(method_lookup)
};

// clang-format on
