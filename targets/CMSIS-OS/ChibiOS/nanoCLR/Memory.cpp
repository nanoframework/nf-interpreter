//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

void HeapLocation(unsigned char*& baseAddress, unsigned int& sizeInBytes)
{
    NATIVE_PROFILE_PAL_HEAP();

    baseAddress = (unsigned char*)                            &HeapBegin;
    sizeInBytes = (unsigned int)((size_t)&HeapEnd - (size_t)&HeapBegin + sizeof(HeapEnd));
}
