//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_native.h"
#include <esp32_idf.h>


uint32_t GetCaps(CLR_RT_StackFrame &stack)
{
    uint32_t caps = MALLOC_CAP_32BIT | MALLOC_CAP_8BIT;
    int memoryType = (int)stack.Arg0().NumericByRef().u4;

    if ( memoryType == NativeMemory_MemoryType::NativeMemory_MemoryType_All )
    {
        caps | = (MALLOC_CAP_INTERNAL + MALLOC_CAP_SPIRAM);
    }
    else if (memoryType == NativeMemory_MemoryType::NativeMemory_MemoryType_Internal)
    {
         caps | = MALLOC_CAP_INTERNAL;
    }
    else
    {
         caps | = MALLOC_CAP_SPIRAM;
    }
    
    return caps;
}


HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_NativeMemory::NativeGetMemoryTotalSize___STATIC__U4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();
    {
        stack.SetResult_U4((uint32_t)heap_caps_get_total_size(GetCaps(stack)));
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_NativeMemory::NativeGetMemoryTotalFreeSize___STATIC__U4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();
    {
        stack.SetResult_U4((uint32_t)heap_caps_get_free_size(GetCaps(stack)));
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_NativeMemory::NativeGetMemoryLargestFreeBlock___STATIC__U4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();
    {
        stack.SetResult_U4((uint32_t)heap_caps_get_largest_free_blockcaps(GetCaps(stack)));
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
