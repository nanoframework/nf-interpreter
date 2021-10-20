//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef GRAPHICS_MEMORY_SETUP_
#define GRAPHICS_MEMORY_SETUP_

#include "nanoCLR_Types.h"
#include "GraphicsMemoryHeap.h"

struct GraphicsMemory g_GraphicsMemory;

extern CLR_UINT32 GraphicsHeapBegin;
extern CLR_UINT32 GraphicsHeapEnd;
bool GraphicsMemory::GraphicsHeapLocation(
    CLR_UINT32 requested,
    CLR_UINT8 *&graphicsStartingAddress,
    CLR_UINT8 *&graphicsEndingAddress)
{
    (void)requested;
    graphicsStartingAddress = (CLR_UINT8 *)&GraphicsHeapBegin;
    graphicsEndingAddress = (CLR_UINT8 *)&GraphicsHeapEnd;
    return true;
}
#endif // GRAPHICS_MEMORY_SETUP
