//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ch.h>
#include <hal.h>
#include <nanoPAL.h>
#include <nanoHAL_Time.h>
#include <nanoHAL_Types.h>
#include <target_platform.h>
#include <nanoPAL_Events.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_ConfigurationManager.h>

#if (HAL_USE_CAN == TRUE)
#include <nf_devices_can_native_target.h>
#endif
#if (HAL_USE_I2C == TRUE)
#include <win_dev_i2c_native_target.h>
#endif
#if (HAL_USE_SPI == TRUE)
#include <win_dev_spi_native_target.h>
#endif
#if (HAL_USE_UART == TRUE)
#include "win_dev_serial_native_target.h"
#endif

// global mutex protecting the internal state of the interpreter, including event flags
// mutex_t interpreterGlobalMutex;

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
    // initialize global mutex
    // chMtxObjectInit(&interpreterGlobalMutex);

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

    // no PAL events required until now
    // PalEvent_Initialize();

#if (HAL_USE_CAN == TRUE)

#if STM32_CAN_USE_CAN1
    Can1_PAL.Driver = NULL;
#endif
#if STM32_CAN_USE_CAN2
    Can2_PAL.Driver = NULL;
#endif
#if STM32_CAN_USE_CAN3
    Can3_PAL.Driver = NULL;
#endif

#endif

#if (HAL_USE_I2C == TRUE)

#if STM32_I2C_USE_I2C1
    I2C1_PAL.Driver = NULL;
#endif
#if STM32_I2C_USE_I2C2
    I2C2_PAL.Driver = NULL;
#endif
#if STM32_I2C_USE_I2C3
    I2C3_PAL.Driver = NULL;
#endif
#if STM32_I2C_USE_I2C4
    I2C4_PAL.Driver = NULL;
#endif

#endif

#if (HAL_USE_SPI == TRUE)
    nanoSPI_Initialize();
#endif

#if (HAL_USE_UART == TRUE)

#if NF_SERIAL_COMM_STM32_UART_USE_USART1
    Uart1_PAL.UartDriver = NULL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_USART2
    Uart2_PAL.UartDriver = NULL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_USART3
    Uart3_PAL.UartDriver = NULL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_UART4
    Uart4_PAL.UartDriver = NULL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_UART5
    Uart5_PAL.UartDriver = NULL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_USART6
    Uart6_PAL.UartDriver = NULL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_UART7
    Uart7_PAL.UartDriver = NULL;
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_UART8
    Uart8_PAL.UartDriver = NULL;
#endif

#endif

    // Initialise Network Stack
    Network_Initialize();
}

void nanoHAL_Uninitialize()
{
    // release the global mutex, just in case it's locked somewhere
    // chMtxUnlock(&interpreterGlobalMutex);

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
    // Network_Uninitialize();
#endif

    BlockStorageList_UnInitializeDevices();

    // need to be sure that:
    // - all mutexes for drivers that use them are released
    // - all drivers are stopped

#if (HAL_USE_SPI == TRUE)
    nanoSPI_Uninitialize();
#endif

#if (HAL_USE_CAN == TRUE)

#if STM32_CAN_USE_CAN1
    canStop(&CAND1);
#endif
#if STM32_CAN_USE_CAN2
    canStop(&CAND2);
#endif
#if STM32_CAN_USE_CAN3
    canStop(&CAND3);
#endif

#endif

#if (HAL_USE_I2C == TRUE)

#if STM32_I2C_USE_I2C1
    i2cReleaseBus(&I2CD1);
    i2cStop(&I2CD1);
#endif
#if STM32_I2C_USE_I2C2
    i2cReleaseBus(&I2CD2);
    i2cStop(&I2CD2);
#endif
#if STM32_I2C_USE_I2C3
    i2cReleaseBus(&I2CD3);
    i2cStop(&I2CD3);
#endif
#if STM32_I2C_USE_I2C4
    i2cReleaseBus(&I2CD4);
    i2cStop(&I2CD4);
#endif

#endif

#if (HAL_USE_SPI == TRUE)
    nanoSPI_Uninitialize();
#endif

#if (HAL_USE_UART == TRUE)

#if NF_SERIAL_COMM_STM32_UART_USE_USART1
    uartReleaseBus(&UARTD1);
    uartStop(&UARTD1);
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_USART2
    uartReleaseBus(&UARTD2);
    uartStop(&UARTD2);
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_USART3
    uartReleaseBus(&UARTD3);
    uartStop(&UARTD3);
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_UART4
    uartReleaseBus(&UARTD4);
    uartStop(&UARTD4);
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_UART5
    uartReleaseBus(&UARTD5);
    uartStop(&UARTD5);
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_USART6
    uartReleaseBus(&UARTD6);
    uartStop(&UARTD6);
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_UART7
    uartReleaseBus(&UARTD7);
    uartStop(&UARTD7);
#endif
#if NF_SERIAL_COMM_STM32_UART_USE_UART8
    uartReleaseBus(&UARTD8);
    uartStop(&UARTD8);
#endif

#endif

    CPU_GPIO_Uninitialize();

    Events_Uninitialize();

    HAL_CONTINUATION::Uninitialize();
    HAL_COMPLETION ::Uninitialize();
}
