//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <Esp32_os.h>
#include <nanoPAL.h>
#include <nanoHAL_Time.h>
#include <nanoHAL_Types.h>
#include <target_platform.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_ConfigurationManager.h>

void Storage_Initialize();
void Storage_Uninitialize();

extern "C" void FixUpHalSystemConfig();
extern "C" void FixUpBlockRegionInfo();

#if NANOCLR_GRAPHICS
// TODO this block should be in header somewhere
#include "Display.h"
#include "DisplayInterface.h"
#include "TouchDevice.h"
#include "TouchInterface.h"
#include "GraphicsMemoryHeap.h"
#include "Graphics.h"

extern DisplayInterface g_DisplayInterface;
extern DisplayDriver g_DisplayDriver;

extern TouchDevice g_TouchDevice;
extern TouchInterface g_TouchInterface;

extern GraphicsMemoryHeap g_GraphicsMemoryHeap;
#endif

//
//  Reboot handlers clean up on reboot
//
static ON_SOFT_REBOOT_HANDLER s_rebootHandlers[16] =
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

void HAL_AddSoftRebootHandler(ON_SOFT_REBOOT_HANDLER handler)
{
    for (unsigned int i = 0; i < ARRAYSIZE(s_rebootHandlers); i++)
    {
        if (s_rebootHandlers[i] == NULL)
        {
            s_rebootHandlers[i] = handler;
            return;
        }
        else if (s_rebootHandlers[i] == handler)
        {
            return;
        }
    }
}

// because nanoHAL_Initialize/Uninitialize needs to be called in both C and C++ we need a proxy to allow it to be called
// in 'C'
extern "C"
{

    void nanoHAL_Initialize_C()
    {
        nanoHAL_Initialize();
    }

    void nanoHAL_Uninitialize_C()
    {
        nanoHAL_Uninitialize();
    }
}

void nanoHAL_Initialize()
{
    HAL_CONTINUATION::InitializeList();
    HAL_COMPLETION ::InitializeList();

    // Fixup System & Block storage parameters based on Flash chip and parttion layout
    FixUpHalSystemConfig();
    FixUpBlockRegionInfo();

    BlockStorageList_Initialize();

    // initialize block storage devices
    BlockStorage_AddDevices();

    BlockStorageList_InitializeDevices();

    // allocate & clear managed heap region
    unsigned char *heapStart = NULL;
    unsigned int heapSize = 0;

    ::HeapLocation(heapStart, heapSize);
    memset(heapStart, 0, heapSize);

    ConfigurationManager_Initialize();

#if NANOCLR_GRAPHICS
    g_GraphicsMemoryHeap.Initialize();
#endif	


    Events_Initialize();

    Storage_Initialize();

    CPU_GPIO_Initialize();

#if (HAL_USE_SPI == TRUE)
    nanoSPI_Initialize();
#endif

#if NANOCLR_GRAPHICS
    // Initialise Graphics after devices initialised
    SpiDisplayConfig displayConfig 
    {
        // Wrover board config
        1,                  // Spi Bus
        GPIO_NUM_22,        // CS_1     GPIO22   CS
        GPIO_NUM_21,        // D/CX_1   GPIO21   D/C
        GPIO_NUM_18,        // RST_1   GPIO18   RESET
        GPIO_NUM_5          // GPIO5   Backlight
    };

    g_DisplayInterface.Initialize(displayConfig);
    g_DisplayDriver.Initialize();

    //g_TouchInterface.Initialize();
    //g_TouchDevice.Initialize();
#endif	

    // no PAL events required until now
    // PalEvent_Initialize();

    // Init Networking
    Network_Initialize();

    // Start Network Debugger
    // SOCKETS_DbgInitialize( 0 );
}

void nanoHAL_Uninitialize()
{
    // check for s_rebootHandlers
    for (unsigned int i = 0; i < ARRAYSIZE(s_rebootHandlers); i++)
    {
        if (s_rebootHandlers[i] != NULL)
        {
            s_rebootHandlers[i]();
        }
        else
        {
            break;
        }
    }

    Storage_Uninitialize();

    SOCKETS_CloseConnections();

#if !defined(HAL_REDUCESIZE)
    // TODO need to call this but it's preventing the debug session from starting
    // Network_Uninitialize();
#endif

    BlockStorageList_UnInitializeDevices();

#if (HAL_USE_SPI == TRUE)
    nanoSPI_Uninitialize();
#endif

    CPU_GPIO_Uninitialize();

    // PalEvent_Uninitialize();

    Events_Uninitialize();

    HAL_CONTINUATION::Uninitialize();
    HAL_COMPLETION ::Uninitialize();
}

volatile int32_t SystemStates[SYSTEM_STATE_TOTAL_STATES];

void SystemState_SetNoLock(SYSTEM_STATE_type state)
{
    SystemStates[state]++;
}

void SystemState_ClearNoLock(SYSTEM_STATE_type state)
{
    SystemStates[state]--;
}

bool SystemState_QueryNoLock(SYSTEM_STATE_type state)
{
    return (SystemStates[state] > 0) ? true : false;
}

void SystemState_Set(SYSTEM_STATE_type state)
{
    GLOBAL_LOCK();

    SystemState_SetNoLock(state);

    GLOBAL_UNLOCK();
}

void SystemState_Clear(SYSTEM_STATE_type state)
{
    GLOBAL_LOCK();

    SystemState_ClearNoLock(state);

    GLOBAL_UNLOCK();
}

bool SystemState_Query(SYSTEM_STATE_type state)
{
    GLOBAL_LOCK();

    bool systemStateCopy = SystemState_QueryNoLock(state);

    GLOBAL_UNLOCK();

    return systemStateCopy;
}

// Just in case storage is not configured
__nfweak void Storage_Initialize(){};
__nfweak void Storage_Uninitialize(){};