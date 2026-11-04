//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_idf.h>
#include <nanoPAL.h>
#include <nanoHAL_Time.h>
#include <nanoHAL_Types.h>
#include <target_platform.h>
#include <nanoPAL_Events.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_ConfigurationManager.h>
#include <nanoHAL_StorageOperation.h>
#include <WireProtocol_Transport.h>
#include <nanoHAL_Graphics.h>
#if (HAL_USE_UART == TRUE)
#include <sys_io_ser_native_target.h>
#endif
void Storage_Initialize();
void Storage_Uninitialize();

extern "C" void FixUpHalSystemConfig();
extern "C" void FixUpBlockRegionInfo();

// Test code for BLE, will be removed once we have managed code interface
extern void blehr_start();
extern void ibeacon_start();

//
//  Reboot handlers clean up on reboot
//
static ON_SOFT_REBOOT_HANDLER s_rebootHandlers[16] =
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

void HAL_AddSoftRebootHandler(ON_SOFT_REBOOT_HANDLER handler)
{
    for (unsigned int i = 0; i < ARRAYSIZE(s_rebootHandlers); i++)
    {
        if (s_rebootHandlers[i] == nullptr)
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

    void nanoHAL_Uninitialize_C(bool isPoweringDown)
    {
        nanoHAL_Uninitialize(isPoweringDown);
    }
}

void nanoHAL_Initialize()
{
    HAL_CONTINUATION::InitializeList();
    HAL_COMPLETION ::InitializeList();

    // Fixup System & Block storage parameters based on Flash chip and partition layout
    FixUpHalSystemConfig();
    FixUpBlockRegionInfo();

    BlockStorageList_Initialize();

    // initialize block storage devices
    BlockStorage_AddDevices();

    // required to setup flash partitions memory mapping
    BlockStorageList_InitializeDevices();

    FS_Initialize();
    FileSystemVolumeList::Initialize();
    FS_AddVolumes();
    FileSystemVolumeList::InitializeVolumes();

    // allocate & clear managed heap region
    unsigned char *heapStart = nullptr;
    unsigned int heapSize = 0;

    ::HeapLocation(heapStart, heapSize);
    memset(heapStart, 0, heapSize);

    ConfigurationManager_Initialize();

    Events_Initialize();

    Storage_Initialize();

    CPU_GPIO_Initialize();

#if (HAL_USE_SPI == TRUE)
    nanoSPI_Initialize();
#endif

#if (HAL_USE_UART == TRUE)

    memset(&Uart0_PAL, 0, sizeof(Uart0_PAL));
    memset(&Uart1_PAL, 0, sizeof(Uart1_PAL));
#if defined(UART_NUM_2)
    memset(&Uart2_PAL, 0, sizeof(Uart2_PAL));
#endif

#endif

    // no PAL events required until now
    // PalEvent_Initialize();

    // Init Networking
    Network_Initialize();

#if CONFIG_NF_WP_CHANNEL_NETWORK
    // Start Wire Protocol over network (listener and discovery)
    WP_Network_Initialize();
#endif

#if (NANOCLR_ESP32_BLE == TRUE)
    // Bluetooth test code
    // blehr_start();  // Heart rate monitor device test
    // ibeacon_start();  // Ibeacon test code
#endif
}

void nanoHAL_Uninitialize(bool isPoweringDown)
{
    (void)isPoweringDown;

    // process Reboot Handlers
    for (size_t i = 0; i < ARRAYSIZE(s_rebootHandlers); i++)
    {
        if (s_rebootHandlers[i] != nullptr)
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

#if !CONFIG_NF_WP_CHANNEL_NETWORK
    // with the Wire Protocol network channel the network stack has to be kept running, otherwise the debugger
    // connection is lost on every CLR reboot
    Network_Uninitialize();
#endif

    FileSystemVolumeList::UninitializeVolumes();

    // required to remove flash partitions memory mapping
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
