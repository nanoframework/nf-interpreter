//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_SYSTEM_TEXT_H_
#define _NF_SYSTEM_TEXT_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <corlib_native.h>

struct Library_nf_system_text_System_Text_StringBuilder
{
    static const int FIELD___maxCapacity = 1;
    static const int FIELD___chunkChars = 2;
    static const int FIELD___chunkLength = 3;
    static const int FIELD___chunkPrevious = 4;
    static const int FIELD___chunkOffset = 5;

    //--//

};

struct Library_nf_system_text_System_Text_UTF8Decoder
{
    NANOCLR_NATIVE_DECLARE(Convert___VOID__SZARRAY_U1__I4__I4__SZARRAY_CHAR__I4__I4__BOOLEAN__BYREF_I4__BYREF_I4__BYREF_BOOLEAN);

    //--//

};

struct Library_nf_system_text_System_Text_UTF8Encoding
{
    NANOCLR_NATIVE_DECLARE(GetBytes___SZARRAY_U1__STRING);
    NANOCLR_NATIVE_DECLARE(GetBytes___I4__STRING__I4__I4__SZARRAY_U1__I4);
    NANOCLR_NATIVE_DECLARE(GetChars___SZARRAY_CHAR__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(GetChars___SZARRAY_CHAR__SZARRAY_U1__I4__I4);

    //--//

    static HRESULT Helper__GetChars(CLR_RT_StackFrame& stack, bool fIndexed);
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Text;

#endif  //_NF_SYSTEM_TEXT_H_
