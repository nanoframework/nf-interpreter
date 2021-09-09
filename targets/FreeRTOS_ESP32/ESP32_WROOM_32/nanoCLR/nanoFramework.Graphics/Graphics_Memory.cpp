//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef GRAPHICS_MEMORY_SETUP_
#define GRAPHICS_MEMORY_SETUP_

#include "nanoCLR_Types.h"
#include <nanoPAL.h>
#include <target_platform.h>
#include <esp32_os.h>
#include "Esp32_DeviceMapping.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "GraphicsMemoryHeap.h"
#include "nanoFramework_Graphics.h"
#include "nanoHAL_Graphics.h"

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

HRESULT InitScreen(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock *spiconfig;

    //CLR_Debug::Printf("before initialize");
    //g_GraphicsMemoryHeap.Initialize();
    //CLR_Debug::Printf("memory allocated");
    // Initialise Graphics after devices initialised
    DisplayInterfaceConfig displayConfig;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    CLR_Debug::Printf("before spi config");
    spiconfig = stack.Arg1().Dereference();

    // Define SPI display configuration for Wrover
    displayConfig.Spi.spiBus = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___spiBus].NumericByRef().u4;                // Spi Bus
    displayConfig.Spi.chipSelect = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___chipSelect].NumericByRef().u4;  // CS_1     GPIO22   CS
    displayConfig.Spi.dataCommand = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___dataCommand].NumericByRef().u4; // D/CX_1   GPIO21   D/C
    displayConfig.Spi.reset = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___reset].NumericByRef().u4;       // RST_1    GPIO18   RESET
    displayConfig.Spi.backLight = spiconfig[Library_nanoFramework_Graphics_nanoFramework_UI_SpiConfiguration::FIELD___backLight].NumericByRef().u4;    // GPIO5    Backlight

    CLR_Debug::Printf("before display initialize");
    g_DisplayInterface.Initialize(displayConfig);
    g_DisplayDriver.Initialize();
    CLR_Debug::Printf("driver and display initialized");

    // g_TouchInterface.Initialize();
    // g_TouchDevice.Initialize();

    PalEvent_Initialize();
    // Gesture_Initialize();
    // Ink_Initialize();

    g_DisplayDriver.Clear();

    NANOCLR_NOCLEANUP();
}

bool GraphicsMemory::GraphicsHeapLocation(CLR_UINT8*& graphicsStartingAddress, CLR_UINT8*& graphicsEndingAddress)
{
    CLR_INT32 graphicsMemoryBlockSize = 2000000;
    CLR_INT32 memoryCaps = MALLOC_CAP_8BIT | MALLOC_CAP_32BIT | MALLOC_CAP_SPIRAM;

    CLR_Debug::Printf("Memory initialization\n");
    if ( heapStartingAddress != 0)
    {
        graphicsStartingAddress = heapStartingAddress;
        graphicsEndingAddress = heapEndingAddress;
        CLR_Debug::Printf("heapStartingAddress != 0, %d, %d\n", heapStartingAddress, heapEndingAddress);
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

    CLR_Debug::Printf("Alloc, s=%d, e=%d, s=%d\n", heapStartingAddress, heapEndingAddress, graphicsMemoryBlockSize);
    return true;
}

#endif  // GRAPHICS_MEMORY_SETUP_

