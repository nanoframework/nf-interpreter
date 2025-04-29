//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "CorLib.h"

HRESULT Library_corlib_native_System_Guid::GenerateNewGuid___STATIC__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_Random rand;
    CLR_UINT8 *buf;
    CLR_RT_HeapBlock &top = stack.PushValueAndClear();

    // Create a array of 16 bytes on top of stack to return
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance(top, 16, g_CLR_RT_WellKnownTypes.UInt8));
    buf = top.DereferenceArray()->GetFirstElement();

    rand.Initialize();

    // Generate 16 random bytes for the GUID
    rand.NextBytes(buf, 16);

    // Set the version (version 4, bits 12-15 of time_hi_and_version)
    // In a standard layout, the version nibble is in byte index 6 (0-based).
    buf[6] = (buf[6] & 0x0F) | 0x40;

    // Set the variant (the two most significant bits of clock_seq_hi_and_reserved must be 10)
    // In a standard layout, the variant byte is at index 8.
    buf[8] = (buf[8] & 0x3F) | 0x80;

    NANOCLR_NOCLEANUP();
}
