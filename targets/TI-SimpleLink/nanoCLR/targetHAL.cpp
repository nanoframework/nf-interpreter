//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL.h>
#include <nanoHAL_Time.h>
#include <nanoHAL_Types.h>
#include <target_platform.h>
#include <nanoPAL_Events.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_ConfigurationManager.h>
// #include <FreeRTOS.h>

#if (HAL_USE_I2C_OPTION == TRUE)
#include <ti/drivers/I2C.h>
#include <win_dev_i2c_native_target.h>
#endif
#if (HAL_USE_SPI_OPTION == TRUE)
#include <ti/drivers/SPI.h>
#include <win_dev_spi_native_target.h>
#endif
#if (HAL_USE_SPI == ON)
#include <easylink/EasyLink.h>
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

    BlockStorageList_Initialize();

    // initialize block storage devices
    BlockStorage_AddDevices();

    BlockStorageList_InitializeDevices();

    // clear managed heap region
    unsigned char *heapStart = NULL;
    unsigned int heapSize = 0;

    ::HeapLocation(heapStart, heapSize);
    memset(heapStart, 0, heapSize);

    ConfigurationManager_Initialize();

    Events_Initialize();

    CPU_GPIO_Initialize();

#if (HAL_USE_SPI_OPTION == TRUE)
    nanoSPI_Initialize();
#endif

    // no PAL events required until now
    // PalEvent_Initialize();

#if (HAL_USE_I2C_OPTION == TRUE)
    I2C1_PAL.i2c = NULL;
#endif

#if (HAL_USE_SPI_OPTION == TRUE)
    SPI1_PAL.masterSpi = NULL;
#endif

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

    BlockStorageList_UnInitializeDevices();

    // PalEvent_Uninitialize();

    // TODO need to call this but it's preventing the debug session from starting
    // Network_Uninitialize();

#if (HAL_USE_SPI_OPTION == TRUE)
    nanoSPI_Uninitialize();
#endif

    CPU_GPIO_Uninitialize();

#if (HAL_USE_I2C_OPTION == TRUE)
    I2C_close(I2C1_PAL.i2c);
#endif

#if (HAL_USE_SPI_OPTION == TRUE)
    SPI_close(SPI1_PAL.masterSpi);
#endif

#if (HAL_USE_EASYLINK == ON)
    EasyLink_abort();
#endif

    Events_Uninitialize();

    HAL_CONTINUATION::Uninitialize();
    HAL_COMPLETION ::Uninitialize();
}
