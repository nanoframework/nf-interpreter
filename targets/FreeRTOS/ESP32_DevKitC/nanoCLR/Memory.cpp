//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

// Define an area for the CLR managed heap
#if !defined(MANAGED_HEAP_SIZE)
#define MANAGED_HEAP_SIZE  (64*1024)
#endif

uint32_t managedHeap[MANAGED_HEAP_SIZE/ sizeof(uint32_t)];

void HeapLocation(unsigned char*& baseAddress, unsigned int& sizeInBytes)
{
    NATIVE_PROFILE_PAL_HEAP();

    baseAddress = (unsigned char*)&managedHeap;
    sizeInBytes = sizeof(managedHeap);
}
