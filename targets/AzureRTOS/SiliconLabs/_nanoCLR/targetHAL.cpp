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
#include <nanoHAL_StorageOperation.h>
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

#if defined(GECKO_USE_USART0) && (GECKO_USE_USART0 == TRUE)
    Usart0_PAL = {0};
#endif
#if defined(GECKO_USE_USART1) && (GECKO_USE_USART1 == TRUE)
    Usart1_PAL = {0};
#endif
#if defined(GECKO_USE_USART2) && (GECKO_USE_USART2 == TRUE)
    Usart2_PAL = {0};
#endif
#if defined(GECKO_USE_USART3) && (GECKO_USE_USART3 == TRUE)
    Usart3_PAL = {0};
#endif
#if defined(GECKO_USE_USART_UART4) && (GECKO_USE_USART_UART4 == TRUE)
    Usart4_PAL = {0};
#endif
#if defined(GECKO_USE_USART_UART5) && (GECKO_USE_USART_UART5 == TRUE)
    Usart5_PAL = {0};
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

    // init DMA driver (don't bother check return value as if it's already started it won't fail)v
    DMADRV_Init();

    // Initialise Network Stack
    Network_Initialize();
}

void nanoHAL_Uninitialize(bool isPoweringDown)
{
    (void)isPoweringDown;

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

#if defined(GECKO_USE_USART0) && (GECKO_USE_USART0 == TRUE)
    if (Usart0_PAL.Usart != NULL)
    {
        UnInit_UART0();
    }
#endif
#if defined(GECKO_USE_USART1) && (GECKO_USE_USART1 == TRUE)
    if (Usart1_PAL.Usart != NULL)
    {
        UnInit_UART1();
    }
#endif
#if defined(GECKO_USE_USART2) && (GECKO_USE_USART2 == TRUE)
    if (Usart2_PAL.Usart != NULL)
    {
        UnInit_UART2();
    }
#endif
#if defined(GECKO_USE_USART3) && (GECKO_USE_USART3 == TRUE)
    if (Usart3_PAL.Usart != NULL)
    {
        UnInit_UART3();
    }
#endif
#if defined(GECKO_USE_USART_UART4) && (GECKO_USE_USART_UART4 == TRUE)
    if (Usart4_PAL.Usart != NULL)
    {
        UnInit_UART4();
    }
#endif
#if defined(GECKO_USE_USART_UART5) && (GECKO_USE_USART_UART5 == TRUE)
    if (Usart5_PAL.Usart != NULL)
    {
        UnInit_UART5();
    }
#endif

#if (HAL_USE_PWM == TRUE)
    DeInitPwm();
#endif

    CPU_GPIO_Uninitialize();

    Events_Uninitialize();

    HAL_CONTINUATION::Uninitialize();
    HAL_COMPLETION ::Uninitialize();
}
