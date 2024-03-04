//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <LaunchCLR.h>
#include <targetHAL.h>

#include <sl_system_init.h>
#include <sl_system_kernel.h>
#include <em_gpio.h>
#include <bsp.h>
#include <em_cmu.h>

#include <tx_api.h>

#include <nanoPAL_BlockStorage.h>
// #include <nanoHAL_ConfigurationManager.h>

extern void sli_usbd_init(void);
extern void sli_usbd_configuration_config0_init(void);
extern void sli_usbd_cdc_acm_acm0_init(void);
extern void usb_device_cdc_acm_app_init(void);

// flags for hardware events
TX_EVENT_FLAGS_GROUP nanoHardwareEvents;

// byte pool configuration and definitions
#define DEFAULT_BYTE_POOL_SIZE 4096
TX_BYTE_POOL byte_pool_0;
ALIGN_TYPE memory_area[DEFAULT_BYTE_POOL_SIZE / sizeof(ALIGN_TYPE)];

// threads definitions and configurations

// receiver thread
#define RECEIVER_THREAD_STACK_SIZE 2048
#define RECEIVER_THREAD_PRIORITY   5

TX_THREAD receiverThread;
ALIGN_TYPE receiverThreadStack[RECEIVER_THREAD_STACK_SIZE / sizeof(ALIGN_TYPE)];
extern void ReceiverThread_entry(uint32_t parameter);

// blink thread
#define BLINK_THREAD_STACK_SIZE 1024
#define BLINK_THREAD_PRIORITY   5

TX_THREAD blinkThread;
ALIGN_TYPE blinkThreadStack[BLINK_THREAD_STACK_SIZE / sizeof(ALIGN_TYPE)];
TX_THREAD blinkThread1;
ALIGN_TYPE blinkThread1Stack[BLINK_THREAD_STACK_SIZE / sizeof(ALIGN_TYPE)];

void BlinkThread_entry(uint32_t parameter)
{
    (void)parameter;

    while (1)
    {
        GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
        tx_thread_sleep(TX_TICKS_PER_MILLISEC(500));
    }
}

void BlinkThread1_entry(uint32_t parameter)
{
    (void)parameter;

    while (1)
    {
        tx_thread_sleep(TX_TICKS_PER_MILLISEC(500));
        GPIO_PinOutToggle(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
    }
}

void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;
    uint16_t status;

    // Create a byte memory pool from which to allocate the thread stacks.
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEFAULT_BYTE_POOL_SIZE);

    // #if (HAL_NF_USE_STM32_CRC == TRUE)
    //     // startup crc
    //     crcStart(NULL);
    // #endif

    // initialize block storage list and devices
    // in CLR this is called in nanoHAL_Initialize()
    // for nanoBooter we have to init it in order to provide the flash map for Monitor_FlashSectorMap command
    BlockStorageList_Initialize();
    BlockStorage_AddDevices();

    // initialize configuration manager
    // in CLR this is called in nanoHAL_Initialize()
    // for nanoBooter we have to init it here to have access to network configuration blocks
    // ConfigurationManager_Initialize();

    // Create blink thread
    status = tx_thread_create(
        &blinkThread,
        "Blink Thread",
        BlinkThread_entry,
        0,
        (uint8_t *)blinkThreadStack,
        BLINK_THREAD_STACK_SIZE,
        BLINK_THREAD_PRIORITY,
        BLINK_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }

    // Create blink thread
    status = tx_thread_create(
        &blinkThread1,
        "Blink Thread1",
        BlinkThread1_entry,
        0,
        (uint8_t *)blinkThread1Stack,
        BLINK_THREAD_STACK_SIZE,
        BLINK_THREAD_PRIORITY,
        BLINK_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }

    // Create receiver thread
    status = tx_thread_create(
        &receiverThread,
        "Receiver Thread",
        ReceiverThread_entry,
        0,
        receiverThreadStack,
        RECEIVER_THREAD_STACK_SIZE,
        RECEIVER_THREAD_PRIORITY,
        RECEIVER_THREAD_PRIORITY,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }

    // create HW event group
    status = tx_event_flags_create(&nanoHardwareEvents, "");
    if (status != TX_SUCCESS)
    {
        while (1)
        {
        }
    }

#if HAL_WP_USE_USB_CDC == TRUE
    sli_usbd_init();
    sli_usbd_configuration_config0_init();
    sli_usbd_cdc_acm_acm0_init();
    usb_device_cdc_acm_app_init();
#endif

    // report successful nanoBooter execution
    ReportSuccessfullNanoBooter();
}

//  Application entry point.
int main(void)
{
    // Initialize the board
    sl_system_init();

    // Configure LED0 and LED1 as output
    GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);

    // configure
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);

    // init boot clipboard
    InitBootClipboard();

    // the following IF is not mandatory, it's just providing a way for a user to 'force'
    // the board to remain in nanoBooter and not launching nanoCLR

    // check if there is a request to remain on nanoBooter
    if (!IsToRemainInBooter())
    {
        // if the BTN0 is pressed, skip the check for a valid CLR image and remain in booter
        if (GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN) != 0)
        {
            // check for valid CLR image
            // we are checking for a valid image at the deployment address, which is pointing to the CLR address
            if (CheckValidCLRImage((uint32_t)&__deployment_start__))
            {
                // there seems to be a valid CLR image

                // need to change HF clock to internal RCO so the CLR can boot smoothly
                CMU_CLOCK_SELECT_SET(HF, HFRCO);

                // launch nanoCLR
                LaunchCLR((uint32_t)&__deployment_start__);
            }
        }
    }

    // Enter the ThreadX kernel. Task(s) created in tx_application_define() will start running
    sl_system_kernel_start();
}
