//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _GraphicsMemoryHeap_H
#define _GraphicsMemoryHeap_H 1

#include <string.h>
#include "nanoCLR_Types.h"

struct GraphicsMemoryHeap
{
    void  Initialize();
    void* Allocate(CLR_UINT32 len);
    bool  Release(void* pHeapBlock);
};
struct GraphicsMemory
{
    void GraphicsHeapLocation( CLR_UINT8*&BaseAddress, CLR_UINT8*&SizeInBytes);
};
#endif  // _GraphicsMemoryHeap_H
