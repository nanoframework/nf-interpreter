//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Esp32_os.h>
#include <nanoPAL.h>
#include <nanoHAL_Time.h>
#include <nanoHAL_Types.h>
#include <target_platform.h>
#include <nanoPAL_Events.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_ConfigurationManager.h>
#include <nanoHAL_Graphics.h>

void Storage_Initialize();
void Storage_Uninitialize();

extern "C" void FixUpHalSystemConfig();
extern "C" void FixUpBlockRegionInfo();

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
    DisplayInterfaceConfig displayConfig;

    displayConfig.Spi.spiBus = 1;                   // Spi Bus
    displayConfig.Spi.chipSelect = GPIO_NUM_22;     // CS_1     GPIO22   CS
    displayConfig.Spi.dataCommand = GPIO_NUM_21;    // D/CX_1   GPIO21   D/C
    displayConfig.Spi.reset = GPIO_NUM_18;          // RST_1   GPIO18   RESET
    displayConfig.Spi.backLight = GPIO_NUM_5;       // GPIO5   Backlight

    g_DisplayInterface.Initialize(displayConfig);
    g_DisplayDriver.Initialize();

    //g_TouchInterface.Initialize();
    //g_TouchDevice.Initialize();

    PalEvent_Initialize();
    //Gesture_Initialize();
    //Ink_Initialize();

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

// Just in case storage is not configured
__nfweak void Storage_Initialize(){};
__nfweak void Storage_Uninitialize(){};