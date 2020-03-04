//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _GRAPHICS_MEMORY_SETUP_
#define _GRAPHICS_MEMORY_SETUP_ 1

#include "nanoCLR_Types.h"
#include "GraphicsMemoryHeap.h"

struct GraphicsMemory g_GraphicsMemory;

void GraphicsMemory::GraphicsHeapLocation(CLR_UINT32& graphicsStartingAddress, CLR_UINT32& graphicsMemoryBlockSize)
{
    // Setup desired size of Graphics memory to reserve
    graphicsMemoryBlockSize = 500000;

    //To Do : Check if enough memory is available and limit to size available
    //#####
    graphicsStartingAddress = (CLR_UINT32)platform_malloc(graphicsMemoryBlockSize);
}
#endif  // _GRAPHICS_MEMORY_SETUP_
