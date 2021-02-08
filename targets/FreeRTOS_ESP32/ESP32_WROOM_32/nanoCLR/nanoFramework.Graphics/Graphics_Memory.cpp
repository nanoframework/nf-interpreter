//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _GRAPHICS_MEMORY_SETUP_
#define _GRAPHICS_MEMORY_SETUP_ 1

#include "nanoCLR_Types.h"
#include <nanoPAL.h>
#include <target_platform.h>
#include <Esp32_os.h>
#include "Esp32_DeviceMapping.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "GraphicsMemoryHeap.h"

struct GraphicsMemory g_GraphicsMemory;

// Choosing Integrate RAM into ESP32 memory map from CONFIG_SPIRAM_USE.
// This is the most basic option for external SPI RAM integration
// During the ESP - IDF startup, external RAM is mapped into the data address space, 
// starting at address 0x3F800000 (byte - accessible).
// The length of this region is the same as the SPI RAM size(up to the limit of 4 MB).

// Applications can manually place data in external memory by creating pointers to this region.
// So if an application uses external memory, it is responsible for all management of the external SPI RAM.
// coordinating buffer usage, preventing corruption, etc.

static CLR_UINT8* heapStartingAddress = 0;
static CLR_UINT8* heapEndingAddress = 0;

bool GraphicsMemory::GraphicsHeapLocation(CLR_UINT8*& graphicsStartingAddress, CLR_UINT8*& graphicsEndingAddress)
{
    CLR_INT32 graphicsMemoryBlockSize = 2000000;
    CLR_INT32 memoryCaps = MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM;

    if ( heapStartingAddress != 0)
    {
        graphicsStartingAddress = heapStartingAddress;
        graphicsEndingAddress = heapEndingAddress;
        return true;
    }

    // Get Largest free block in SPIRam
    CLR_INT32 spiramMaxSize = heap_caps_get_largest_free_block(memoryCaps);
    if ( spiramMaxSize == 0)
    {
        // No SpiRam, try and allocate small block in normal ram to keep allocator happy for
        // people tryig to run graphics on boards without Spiram
        // Should be able to use with small screens 
        memoryCaps ^= MALLOC_CAP_SPIRAM;
        
        // TODO: Have a parameter to govern this size as it may be enough for small display
        spiramMaxSize = 20*1024;
    }

    if (spiramMaxSize < graphicsMemoryBlockSize)                        // limit the size to what is available
    {
        graphicsMemoryBlockSize = spiramMaxSize;
    }
    graphicsStartingAddress = (CLR_UINT8*)heap_caps_malloc(graphicsMemoryBlockSize, memoryCaps);
    ASSERT(graphicsStartingAddress != NULL);
    graphicsEndingAddress = (CLR_UINT8*)(graphicsStartingAddress + graphicsMemoryBlockSize);

    // Save where we allocated it for restarts
    heapStartingAddress = graphicsStartingAddress;
    heapEndingAddress = graphicsEndingAddress;

    return true;
}

#endif  // _GRAPHICS_MEMORY_SETUP_

