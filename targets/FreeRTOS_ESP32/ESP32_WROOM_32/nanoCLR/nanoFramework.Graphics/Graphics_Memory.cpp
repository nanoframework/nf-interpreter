// Copyright (c) 2017 The nanoFramework project contributors
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

void GraphicsMemory::GraphicsHeapLocation(CLR_UINT8*& graphicsStartingAddress, CLR_UINT8*& graphicsEndingAddress)
{
//    CLR_INT32 graphicsMemoryBlockSize = 2000000;
    CLR_INT32 graphicsMemoryBlockSize = 2000000;

    CLR_INT32 spiramMaxSize = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);
    if (spiramMaxSize < graphicsMemoryBlockSize)                        // limit the size to what is available
    {
        graphicsMemoryBlockSize = spiramMaxSize;
    }
    graphicsStartingAddress = (CLR_UINT8*)heap_caps_malloc(graphicsMemoryBlockSize, MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM);
    graphicsEndingAddress = (CLR_UINT8*)(graphicsStartingAddress + graphicsMemoryBlockSize);

}

#endif  // _GRAPHICS_MEMORY_SETUP_

