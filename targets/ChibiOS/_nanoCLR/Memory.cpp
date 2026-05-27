//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

extern "C" {
    void HeapLocation_C(unsigned char** baseAddress, unsigned int* sizeInBytes)
    {
        *baseAddress = (unsigned char*)(unsigned char*) &HeapBegin;
        *sizeInBytes = (unsigned int)((size_t)&HeapEnd - (size_t)&HeapBegin);
    }
}

void HeapLocation(unsigned char*& baseAddress, unsigned int& sizeInBytes)
{
    NATIVE_PROFILE_PAL_HEAP();

    baseAddress = (unsigned char*)                            &HeapBegin;
    sizeInBytes = (unsigned int)((size_t)&HeapEnd - (size_t)&HeapBegin);
}
