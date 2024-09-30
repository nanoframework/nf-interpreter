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
#include <nanoHAL_StorageOperation.h>
#include <nanoHAL_Graphics.h>

#if (HAL_USE_CAN == TRUE)
#include <nf_device_can_native_target.h>
#endif
#if (HAL_USE_I2C == TRUE)
#include <sys_dev_i2c_native_target.h>
#endif
#if (HAL_USE_SPI == TRUE)
#include <sys_dev_spi_native_target.h>
#endif
#if (HAL_USE_UART == TRUE)
#include <sys_io_ser_native_target.h>
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

    void nanoHAL_Uninitialize_C(bool isPoweringDown)
    {
        nanoHAL_Uninitialize(isPoweringDown);
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

    FS_Initialize();
    FileSystemVolumeList::Initialize();
    FS_AddVolumes();
    FileSystemVolumeList::InitializeVolumes();

    // clear managed heap region
    unsigned char *heapStart = NULL;
    unsigned int heapSize = 0;

    ::HeapLocation(heapStart, heapSize);
    memset(heapStart, 0, heapSize);

    ConfigurationManager_Initialize();

    Events_Initialize();

    CPU_GPIO_Initialize();

#if (HAL_USE_CAN == TRUE)

#if defined(STM32_CAN_USE_CAN1) && (STM32_CAN_USE_CAN1 == TRUE)
    memset(&Can1_PAL, 0, sizeof(Can1_PAL));
#endif
#if (STM32_CAN_USE_CAN2) && (STM32_CAN_USE_CAN2 == TRUE)
    memset(&Can2_PAL, 0, sizeof(Can2_PAL));
#endif
#if (STM32_CAN_USE_CAN3) && (STM32_CAN_USE_CAN3 == TRUE)
    memset(&Can3_PAL, 0, sizeof(Can3_PAL));
#endif

#endif

#if (HAL_USE_I2C == TRUE)

#if defined(STM32_I2C_USE_I2C1) && (STM32_I2C_USE_I2C1 == TRUE)
    memset(&I2C1_PAL, 0, sizeof(I2C1_PAL));
#endif
#if defined(STM32_I2C_USE_I2C2) && (STM32_I2C_USE_I2C2 == TRUE)
    memset(&I2C2_PAL, 0, sizeof(I2C2_PAL));
#endif
#if defined(STM32_I2C_USE_I2C3) && (STM32_I2C_USE_I2C3 == TRUE)
    memset(&I2C3_PAL, 0, sizeof(I2C3_PAL));
#endif
#if defined(STM32_I2C_USE_I2C4) && (STM32_I2C_USE_I2C4 == TRUE)
    memset(&I2C4_PAL, 0, sizeof(I2C4_PAL));
#endif

#endif

#if (HAL_USE_SPI == TRUE)
    nanoSPI_Initialize();
#endif

#if (HAL_USE_UART == TRUE)

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
    memset(&Uart1_PAL, 0, sizeof(Uart1_PAL));
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
    memset(&Uart2_PAL, 0, sizeof(Uart2_PAL));
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
    memset(&Uart3_PAL, 0, sizeof(Uart3_PAL));
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
    memset(&Uart4_PAL, 0, sizeof(Uart4_PAL));
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
    memset(&Uart5_PAL, 0, sizeof(Uart5_PAL));
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
    memset(&Uart6_PAL, 0, sizeof(Uart6_PAL));
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
    memset(&Uart7_PAL, 0, sizeof(Uart7_PAL));
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
    memset(&Uart8_PAL, 0, sizeof(Uart8_PAL));
#endif

#endif

#if (NANOCLR_GRAPHICS == TRUE)
    DisplayInterfaceConfig config; // not used for DSI display
    g_DisplayInterface.Initialize(config);
    g_DisplayDriver.Initialize();

    // g_TouchInterface.Initialize();
    // g_TouchDevice.Initialize();

    // PalEvent_Initialize();
    // Gesture_Initialize();
    // Ink_Initialize();
#endif

    // Initialise Network Stack
    Network_Initialize();
}

void nanoHAL_Uninitialize(bool isPoweringDown)
{
    (void)isPoweringDown;

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

    FileSystemVolumeList::UninitializeVolumes();

    BlockStorageList_UnInitializeDevices();

    // need to be sure that:
    // - all mutexes for drivers that use them are released
    // - all drivers are stopped

#if (HAL_USE_SPI == TRUE)
    nanoSPI_Uninitialize();
#endif

#if (HAL_USE_CAN == TRUE)

#if defined(STM32_CAN_USE_CAN1) && (STM32_CAN_USE_CAN1 == TRUE)
    canStop(&CAND1);
#endif
#if (STM32_CAN_USE_CAN2) && (STM32_CAN_USE_CAN2 == TRUE)
    canStop(&CAND2);
#endif
#if (STM32_CAN_USE_CAN3) && (STM32_CAN_USE_CAN3 == TRUE)
    canStop(&CAND3);
#endif

#endif

#if (HAL_USE_I2C == TRUE)

#if defined(STM32_I2C_USE_I2C1) && (STM32_I2C_USE_I2C1 == TRUE)
    i2cReleaseBus(&I2CD1);
    i2cStop(&I2CD1);
#endif
#if defined(STM32_I2C_USE_I2C2) && (STM32_I2C_USE_I2C2 == TRUE)
    i2cReleaseBus(&I2CD2);
    i2cStop(&I2CD2);
#endif
#if defined(STM32_I2C_USE_I2C3) && (STM32_I2C_USE_I2C3 == TRUE)
    i2cReleaseBus(&I2CD3);
    i2cStop(&I2CD3);
#endif
#if defined(STM32_I2C_USE_I2C4) && (STM32_I2C_USE_I2C4 == TRUE)
    i2cReleaseBus(&I2CD4);
    i2cStop(&I2CD4);
#endif

#endif

#if (HAL_USE_SPI == TRUE)
    nanoSPI_Uninitialize();
#endif

#if (HAL_USE_UART == TRUE)

#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
    uartReleaseBus(&UARTD1);
    uartStop(&UARTD1);
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
    uartReleaseBus(&UARTD2);
    uartStop(&UARTD2);
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
    uartReleaseBus(&UARTD3);
    uartStop(&UARTD3);
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
    uartReleaseBus(&UARTD4);
    uartStop(&UARTD4);
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
    uartReleaseBus(&UARTD5);
    uartStop(&UARTD5);
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
    uartReleaseBus(&UARTD6);
    uartStop(&UARTD6);
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
    uartReleaseBus(&UARTD7);
    uartStop(&UARTD7);
#endif
#if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
    uartReleaseBus(&UARTD8);
    uartStop(&UARTD8);
#endif

#endif

    CPU_GPIO_Uninitialize();

    Events_Uninitialize();

    HAL_CONTINUATION::Uninitialize();
    HAL_COMPLETION ::Uninitialize();
}
