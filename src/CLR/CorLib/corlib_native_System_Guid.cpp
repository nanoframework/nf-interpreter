//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"

//
//  Generate a new GUID
//
//  Based on the version 4 GUID (random)
//  http://guid.one/guid/make


HRESULT Library_corlib_native_System_Guid::GenerateNewGuid___STATIC__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_RT_Random       rand;
    CLR_UINT8*          buf;
    CLR_RT_HeapBlock&   top = stack.PushValueAndClear();
 
    // Create a array of 16 bytes on top of stack to return
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( top, 16, g_CLR_RT_WellKnownTypes.m_UInt8 ));
    buf = top.DereferenceArray()->GetFirstElement();

    rand.Initialize();
    rand.NextBytes(buf, 16);           // fill with random numbers

    buf[7] =  (buf[7] & 0x0f) | 0x40;  // Set verion
    buf[9] =  (buf[7] & 0x3f) | 0x80;  // Set variant
    
    NANOCLR_NOCLEANUP();
}
