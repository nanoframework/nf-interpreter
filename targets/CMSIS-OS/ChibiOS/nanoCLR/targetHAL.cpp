//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <nanoPAL.h>
#include <nanoHAL_Time.h>
#include <nanoHAL_Types.h>
#include <target_platform.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_ConfigurationManager.h>

// because nanoHAL_Initialize/Uninitialize needs to be called in both C and C++ we need a proxy to allow it to be called in 'C'
extern "C" {
    
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
    HAL_COMPLETION  ::InitializeList();

    BlockStorageList_Initialize();

    // initialize block storage devices
    BlockStorage_AddDevices();

    BlockStorageList_InitializeDevices();

    // clear managed heap region
    unsigned char* heapStart = NULL;
    unsigned int heapSize  = 0;

    ::HeapLocation( heapStart, heapSize );
    memset(heapStart, 0, heapSize);

    ConfigurationManager_Initialize();

    Events_Initialize();

    // no PAL events required until now
    //PalEvent_Initialize();
	
	// Initialise Network Stack
    Network_Initialize();
}

void nanoHAL_Uninitialize()
{
    // TODO check for s_rebootHandlers
    // for(int i = 0; i< ARRAYSIZE(s_rebootHandlers); i++)
    // {
    //     if(s_rebootHandlers[i] != NULL)
    //     {
    //         s_rebootHandlers[i]();
    //     }
    //     else
    //     {
    //         break;
    //     }
    // }   

    SOCKETS_CloseConnections();

  #if !defined(HAL_REDUCESIZE)
    // TODO need to call this but it's preventing the debug session from starting
    //Network_Uninitialize();
  #endif

    BlockStorageList_UnInitializeDevices();

    // need to be sure that all mutexes for drivers that use them are released
#if (HAL_USE_SPI == TRUE)

    #if STM32_SPI_USE_SPI1
    spiReleaseBus(&SPID1);
    #endif
    #if STM32_SPI_USE_SPI2
    spiReleaseBus(&SPID2);
    #endif
    #if STM32_SPI_USE_SPI3
    spiReleaseBus(&SPID3);
    #endif
    #if STM32_SPI_USE_SPI4
    spiReleaseBus(&SPID4);
    #endif
    #if STM32_SPI_USE_SPI5
    spiReleaseBus(&SPID5);
    #endif
    #if STM32_SPI_USE_SPI6
    spiReleaseBus(&SPID6);
    #endif

#endif

#if (HAL_USE_I2C == TRUE)

    #if STM32_I2C_USE_I2C1
    i2cReleaseBus(&I2CD1);
    #endif
    #if STM32_I2C_USE_I2C2
    i2cReleaseBus(&I2CD2);
    #endif
    #if STM32_I2C_USE_I2C3
    i2cReleaseBus(&I2CD3);
    #endif
    #if STM32_I2C_USE_I2C4
    i2cReleaseBus(&I2CD4);
    #endif

#endif

#if (HAL_USE_UART == TRUE)

    #if NF_SERIAL_COMM_STM32_UART_USE_USART1
    uartReleaseBus(&UARTD1);
    #endif
    #if NF_SERIAL_COMM_STM32_UART_USE_USART2
    uartReleaseBus(&UARTD2);
    #endif
    #if NF_SERIAL_COMM_STM32_UART_USE_USART3
    uartReleaseBus(&UARTD3);
    #endif
    #if NF_SERIAL_COMM_STM32_UART_USE_UART4
    uartReleaseBus(&UARTD4);
    #endif
    #if NF_SERIAL_COMM_STM32_UART_USE_UART5
    uartReleaseBus(&UARTD5);
    #endif
    #if NF_SERIAL_COMM_STM32_UART_USE_USART6
    uartReleaseBus(&UARTD6);
    #endif
    #if NF_SERIAL_COMM_STM32_UART_USE_UART7
    uartReleaseBus(&UARTD7);
    #endif
    #if NF_SERIAL_COMM_STM32_UART_USE_UART8
    uartReleaseBus(&UARTD8);
    #endif

#endif

    Events_Uninitialize();
    
    HAL_CONTINUATION::Uninitialize();
    HAL_COMPLETION  ::Uninitialize();
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
    GLOBAL_LOCK(irq);

    SystemState_SetNoLock(state);

    GLOBAL_UNLOCK(irq);
}

void SystemState_Clear(SYSTEM_STATE_type state)
{
    GLOBAL_LOCK(irq);

    SystemState_ClearNoLock(state );

    GLOBAL_UNLOCK(irq);
}

bool SystemState_Query(SYSTEM_STATE_type state)
{
    GLOBAL_LOCK(irq);

    return SystemState_QueryNoLock(state);

    GLOBAL_UNLOCK(irq);
}
