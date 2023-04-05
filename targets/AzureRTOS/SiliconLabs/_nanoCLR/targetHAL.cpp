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
#include <nanoHAL_Graphics.h>

#include <dmadrv.h>
#include <target_nano_gg_adc_config.h>

// #if (HAL_USE_CAN == TRUE)
// #include <nf_device_can_native_target.h>
// #endif
#include <sys_dev_i2c_native_target.h>
#include <sys_dev_spi_native_target.h>
// #if (HAL_USE_UART == TRUE)
// #include <win_dev_serial_native_target.h>
// #endif

#if (HAL_USE_PWM == TRUE)
extern void DeInitPwm();
#endif

#if (GECKO_USE_ADC0 == TRUE) || (GECKO_USE_ADC1 == TRUE)
#include <nano_gg_adc_native_target.h>
#include <sys_dev_adc_native_target.h>
#endif

#if GECKO_FEATURE_USBD_WINUSB == TRUE
#include <sys_dev_usbstream_native_target.h>
#endif

// global mutex protecting the internal state of the interpreter, including event flags
// mutex_t interpreterGlobalMutex;
//
//  Reboot handlers clean up on reboot
//
static ON_SOFT_REBOOT_HANDLER s_rebootHandlers[16] =
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

void HAL_AddSoftRebootHandler(ON_SOFT_REBOOT_HANDLER handler)
{
    for (size_t i = 0; i < ARRAYSIZE(s_rebootHandlers); i++)
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

    // clear managed heap region
    unsigned char *heapStart = NULL;
    unsigned int heapSize = 0;

    ::HeapLocation(heapStart, heapSize);
    memset(heapStart, 0, heapSize);

#if (NANOCLR_GRAPHICS == TRUE)
    g_GraphicsMemoryHeap.Initialize();
#endif

    ConfigurationManager_Initialize();

    Events_Initialize();

    CPU_GPIO_Initialize();

    // #if (HAL_USE_CAN == TRUE)

    // #if defined(STM32_CAN_USE_CAN1) && (STM32_CAN_USE_CAN1 == TRUE)
    //     Can1_PAL.Driver = NULL;
    // #endif
    // #if (STM32_CAN_USE_CAN2) && (STM32_CAN_USE_CAN2 == TRUE)
    //     Can2_PAL.Driver = NULL;
    // #endif
    // #if (STM32_CAN_USE_CAN3) && (STM32_CAN_USE_CAN3 == TRUE)
    //     Can3_PAL.Driver = NULL;
    // #endif

    // #endif

#if defined(I2C0) && (GECKO_USE_I2C0 == TRUE)
    memset(&I2C0_PAL, 0, sizeof(NF_PAL_I2C));
#endif
#if defined(I2C1) && (GECKO_USE_I2C1 == TRUE)
    memset(&I2C1_PAL, 0, sizeof(NF_PAL_I2C));
#endif
#if defined(I2C2) && (GECKO_USE_I2C2 == TRUE)
    memset(&I2C2_PAL, 0, sizeof(NF_PAL_I2C));
#endif

#if (HAL_USE_SPI == TRUE)
    nanoSPI_Initialize();

#if (GECKO_USE_SPI0 == TRUE)
    memset(&SPI0_PAL, 0, sizeof(NF_PAL_SPI));
#endif
#if (GECKO_USE_SPI1 == TRUE)
    memset(&SPI1_PAL, 0, sizeof(NF_PAL_SPI));
#endif
#if (GECKO_USE_SPI2 == TRUE)
    memset(&SPI2_PAL, 0, sizeof(NF_PAL_SPI));
#endif
#if (GECKO_USE_SPI3 == TRUE)
    memset(&SPI3_PAL, 0, sizeof(NF_PAL_SPI));
#endif
#if (GECKO_USE_SPI4 == TRUE)
    memset(&SPI4_PAL, 0, sizeof(NF_PAL_SPI));
#endif
#if (GECKO_USE_SPI5 == TRUE)
    memset(&SPI5_PAL, 0, sizeof(NF_PAL_SPI));
#endif

#endif

#if (GECKO_USE_ADC0 == TRUE) && defined(NANO_GG_ADC_NATIVE_TARGET_H)
    adc0Initialized = false;
#endif
#if (GECKO_USE_ADC1 == TRUE) && defined(NANO_GG_ADC_NATIVE_TARGET_H)
    adc1Initialized = false;
#endif

#if GECKO_FEATURE_USBD_WINUSB == TRUE
    memset(&UsbStream_PAL, 0, sizeof(UsbStream_PAL));
#endif

    // #if (HAL_USE_UART == TRUE)

    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
    //     Uart1_PAL.UartDriver = NULL;
    //     Uart1_PAL__.UartDriver = NULL;
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
    //     Uart2_PAL.UartDriver = NULL;
    //     Uart2_PAL__.UartDriver = NULL;
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
    //     Uart3_PAL.UartDriver = NULL;
    //     Uart3_PAL__.UartDriver = NULL;
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
    //     Uart4_PAL.UartDriver = NULL;
    //     Uart4_PAL__.UartDriver = NULL;
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
    //     Uart5_PAL.UartDriver = NULL;
    //     Uart5_PAL__.UartDriver = NULL;
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
    //     Uart6_PAL.UartDriver = NULL;
    //     Uart6_PAL__.UartDriver = NULL;
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
    //     Uart7_PAL.UartDriver = NULL;
    //     Uart7_PAL__.UartDriver = NULL;
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
    //     Uart8_PAL.UartDriver = NULL;
    //     Uart8_PAL__.UartDriver = NULL;
    // #endif

    // #endif

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

    // init DMA driver (don't bother check return value as if it's already started it won't fail)v
    DMADRV_Init();

    // Initialise Network Stack
    Network_Initialize();
}

void nanoHAL_Uninitialize(bool isPoweringDown)
{
    (void)isPoweringDown;

    // release the global mutex, just in case it's locked somewhere
    // chMtxUnlock(&interpreterGlobalMutex);
    // process Reboot Handlers
    for (size_t i = 0; i < ARRAYSIZE(s_rebootHandlers); i++)
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

    DMADRV_DeInit();

    // TODO
    SOCKETS_CloseConnections();

#if !defined(HAL_REDUCESIZE)
    // TODO need to call this but it's preventing the debug session from starting
    Network_Uninitialize();
#endif

    BlockStorageList_UnInitializeDevices();

    // need to be sure that:
    // - all mutexes for drivers that use them are released
    // - all drivers are stopped

    // #if (HAL_USE_CAN == TRUE)

    // #if defined(STM32_CAN_USE_CAN1) && (STM32_CAN_USE_CAN1 == TRUE)
    //     canStop(&CAND1);
    // #endif
    // #if (STM32_CAN_USE_CAN2) && (STM32_CAN_USE_CAN2 == TRUE)
    //     canStop(&CAND2);
    // #endif
    // #if (STM32_CAN_USE_CAN3) && (STM32_CAN_USE_CAN3 == TRUE)
    //     canStop(&CAND3);
    // #endif

    // #endif

#if defined(I2C0) && (GECKO_USE_I2C0 == TRUE)
    if (I2C0_PAL.Configuration != NULL)
    {
        platform_free(I2C0_PAL.Configuration);
        I2C0_PAL.Configuration = NULL;
    }
    I2C_Reset(I2C0);
#endif
#if defined(I2C1) && (GECKO_USE_I2C1 == TRUE)
    if (I2C1_PAL.Configuration != NULL)
    {
        platform_free(I2C1_PAL.Configuration);
        I2C1_PAL.Configuration = NULL;
    }
    I2C_Reset(I2C1);
#endif
#if defined(I2C2) && (GECKO_USE_I2C2 == TRUE)
    if (I2C2_PAL.Configuration != NULL)
    {
        platform_free(I2C2_PAL.Configuration);
        I2C2_PAL.Configuration = NULL;
    }
    I2C_Reset(I2C2);
#endif

#if (HAL_USE_SPI == TRUE)
    nanoSPI_Uninitialize();
#endif

#if (GECKO_USE_ADC0 == TRUE) && defined(NANO_GG_ADC_NATIVE_TARGET_H)
    ADC_Reset(ADC0);
    adc0Initialized = false;
#endif
#if (GECKO_USE_ADC1 == TRUE) && defined(NANO_GG_ADC_NATIVE_TARGET_H)
    ADC_Reset(ADC1);
    adc1Initialized = false;
#endif

#if GECKO_FEATURE_USBD_WINUSB == TRUE

    // abort any transfer in progress, just in case
    sl_usbd_vendor_abort_write_bulk(sl_usbd_vendor_winusb_number);
    sl_usbd_vendor_abort_read_bulk(sl_usbd_vendor_winusb_number);

#endif

    // #if (HAL_USE_UART == TRUE)

    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_USART1) && (NF_SERIAL_COMM_STM32_UART_USE_USART1 == TRUE)
    //     uartReleaseBus(&UARTD1);
    //     uartStop(&UARTD1);
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_USART2) && (NF_SERIAL_COMM_STM32_UART_USE_USART2 == TRUE)
    //     uartReleaseBus(&UARTD2);
    //     uartStop(&UARTD2);
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_USART3) && (NF_SERIAL_COMM_STM32_UART_USE_USART3 == TRUE)
    //     uartReleaseBus(&UARTD3);
    //     uartStop(&UARTD3);
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_UART4) && (NF_SERIAL_COMM_STM32_UART_USE_UART4 == TRUE)
    //     uartReleaseBus(&UARTD4);
    //     uartStop(&UARTD4);
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_UART5) && (NF_SERIAL_COMM_STM32_UART_USE_UART5 == TRUE)
    //     uartReleaseBus(&UARTD5);
    //     uartStop(&UARTD5);
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_USART6) && (NF_SERIAL_COMM_STM32_UART_USE_USART6 == TRUE)
    //     uartReleaseBus(&UARTD6);
    //     uartStop(&UARTD6);
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_UART7) && (NF_SERIAL_COMM_STM32_UART_USE_UART7 == TRUE)
    //     uartReleaseBus(&UARTD7);
    //     uartStop(&UARTD7);
    // #endif
    // #if defined(NF_SERIAL_COMM_STM32_UART_USE_UART8) && (NF_SERIAL_COMM_STM32_UART_USE_UART8 == TRUE)
    //     uartReleaseBus(&UARTD8);
    //     uartStop(&UARTD8);
    // #endif

    // #endif

#if (HAL_USE_PWM == TRUE)
    DeInitPwm();
#endif

    CPU_GPIO_Uninitialize();

    Events_Uninitialize();

    HAL_CONTINUATION::Uninitialize();
    HAL_COMPLETION ::Uninitialize();
}
