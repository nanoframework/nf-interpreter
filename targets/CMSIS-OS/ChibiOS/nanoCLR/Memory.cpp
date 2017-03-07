//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

void HeapLocation(UINT8*& baseAddress, UINT32& sizeInBytes)
{
    NATIVE_PROFILE_PAL_HEAP();

    baseAddress = (UINT8*)                            &HeapBegin;
    sizeInBytes = (UINT32)((size_t)&HeapEnd - (size_t)&HeapBegin + sizeof(HeapEnd));
}
