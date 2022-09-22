//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_native.h"
#include <nanoHAL.h>

uint32_t GetCaps(CLR_RT_StackFrame &stack)
{
    uint32_t caps = MALLOC_CAP_32BIT | MALLOC_CAP_8BIT;
    int memoryType = (int)stack.Arg0().NumericByRef().u4;

    if (memoryType == NativeMemory_MemoryType::NativeMemory_MemoryType_Internal)
    {
        caps |= MALLOC_CAP_INTERNAL;
    }
    else if (memoryType == NativeMemory_MemoryType::NativeMemory_MemoryType_SpiRam)
    {
        caps |= MALLOC_CAP_SPIRAM;
    }

    return caps;
}

//
// Get the total size of all the regions.
//
HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_NativeMemory::
    NativeGetMemoryTotalSize___STATIC__U4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        stack.SetResult_U4((uint32_t)heap_caps_get_total_size(GetCaps(stack)));
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

//
// Get the total free size of all the regions.
//
HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_NativeMemory::
    NativeGetMemoryTotalFreeSize___STATIC__U4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        stack.SetResult_U4((uint32_t)heap_caps_get_free_size(GetCaps(stack)));
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

//
// Get the largest free block of memory able to be allocated.
//
HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_NativeMemory::
    NativeGetMemoryLargestFreeBlock___STATIC__U4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        stack.SetResult_U4((uint32_t)heap_caps_get_largest_free_block(GetCaps(stack)));
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
