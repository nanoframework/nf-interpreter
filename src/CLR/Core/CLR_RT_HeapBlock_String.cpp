//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

CLR_RT_HeapBlock_String *CLR_RT_HeapBlock_String::CreateInstance(CLR_RT_HeapBlock &reference, CLR_UINT32 length)
{
    NATIVE_PROFILE_CLR_CORE();

    // compute required size for the string object (header + string length + null terminator)
    CLR_UINT32 totLength = sizeof(CLR_RT_HeapBlock_String) + length + 1;
    CLR_RT_HeapBlock_String *str;

    reference.SetObjectReference(NULL);

    str = (CLR_RT_HeapBlock_String *)g_CLR_RT_ExecutionEngine.ExtractHeapBytesForObjects(DATATYPE_STRING, 0, totLength);
    if (str)
    {
        // grab a pointer to the string storage area (after the CLR_RT_HeapBlock_String header)
        char *szText = (char *)&str[1];

        // zero out the string storage area
        memset(szText, 0, CONVERTFROMSIZETOHEAPBLOCKS(totLength - sizeof(CLR_RT_HeapBlock_String)));

#if defined(NANOCLR_NO_ASSEMBLY_STRINGS)
        str->SetStringText(szText);
#else
        str->SetStringText(szText, NULL);
#endif

        reference.SetObjectReference(str);
    }

    return str;
}

HRESULT CLR_RT_HeapBlock_String::CreateInstance(CLR_RT_HeapBlock &reference, const char *szText)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    if (!szText)
    {
        szText = "";
    }

    NANOCLR_SET_AND_LEAVE(CLR_RT_HeapBlock_String::CreateInstance(reference, szText, (CLR_UINT32)hal_strlen_s(szText)));

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_HeapBlock_String::CreateInstance(CLR_RT_HeapBlock &reference, const char *szText, CLR_UINT32 length)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_String *str;
    char *szTextDst;

    str = CreateInstance(reference, length);
    CHECK_ALLOCATION(str);

    // grab a pointer to the string storage area (after the CLR_RT_HeapBlock_String header)
    szTextDst = (char *)str->StringText();

    // copy the string to the storage area
    memcpy(szTextDst, szText, length);
    szTextDst[length] = 0;

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_HeapBlock_String::CreateInstance(CLR_RT_HeapBlock &reference, const char *szText, CLR_RT_Assembly *assm)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

#if defined(NANOCLR_NO_ASSEMBLY_STRINGS)

    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(reference, szText));

#else

    CLR_RT_HeapBlock_String *str;

    reference.SetObjectReference(NULL);

    // get heap block for the string object
    // only the header is required as we're just pointing to the string stored in the assembly
    str = (CLR_RT_HeapBlock_String *)
              g_CLR_RT_ExecutionEngine.ExtractHeapBytesForObjects(DATATYPE_STRING, 0, sizeof(CLR_RT_HeapBlock_String));
    CHECK_ALLOCATION(str);

    reference.SetObjectReference(str);

    // store the pointers to the string and assembly
    str->SetStringText(szText, assm);

#endif

    NANOCLR_NOCLEANUP();
}

HRESULT CLR_RT_HeapBlock_String::CreateInstance(CLR_RT_HeapBlock &reference, CLR_UINT32 token, CLR_RT_Assembly *assm)
{
    NATIVE_PROFILE_CLR_CORE();
    return CreateInstance(reference, assm->GetString(CLR_DataFromTk(token)), assm);
}

HRESULT CLR_RT_HeapBlock_String::CreateInstance(CLR_RT_HeapBlock &reference, CLR_UINT16 *szText, CLR_UINT32 length)
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();

    CLR_RT_UnicodeHelper uh;
    uh.SetInputUTF16(szText);
    CLR_UINT32 lengthInBytes = uh.CountNumberOfBytes(length);
    CLR_RT_HeapBlock_String *str = CreateInstance(reference, lengthInBytes);
    CHECK_ALLOCATION(str);

    uh.m_outputUTF8 = (CLR_UINT8 *)str->StringText();
    uh.m_outputUTF8_size = lengthInBytes + 1;

    uh.ConvertToUTF8(length, false);

    NANOCLR_NOCLEANUP();
}

CLR_RT_HeapBlock_String *CLR_RT_HeapBlock_String::GetStringEmpty()
{
    return g_CLR_RT_TypeSystem.m_assemblyMscorlib
        ->GetStaticField(Library_corlib_native_System_String::FIELD_STATIC__Empty)
        ->DereferenceString();
}
