//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sl_system_init.h>
#include <sl_system_kernel.h>
#include <em_gpio.h>
#include <sl_simple_led_instances.h>

#include <LaunchCLR.h>
#include <targetHAL.h>

#include <tx_api.h>

#include <nanoPAL_BlockStorage.h>
// #include <nanoHAL_ConfigurationManager.h>

// byte pool configuration and definitions
#define DEFAULT_BYTE_POOL_SIZE 4096
TX_BYTE_POOL byte_pool_0;
uint8_t memory_area[DEFAULT_BYTE_POOL_SIZE];

// threads definitions and configurations

// receiver thread
#define RECEIVER_THREAD_STACK_SIZE 2048
#define RECEIVER_THREAD_PRIORITY   5

TX_THREAD receiverThread;
uint32_t receiverThreadStack[RECEIVER_THREAD_STACK_SIZE / sizeof(uint32_t)];
extern void ReceiverThread_entry(uint32_t parameter);

// blink thread
#define BLINK_THREAD_STACK_SIZE 1024
#define BLINK_THREAD_PRIORITY   5

TX_THREAD blinkThread;
uint32_t blinkThreadStack[BLINK_THREAD_STACK_SIZE / sizeof(uint32_t)];
TX_THREAD blinkThread1;
uint32_t blinkThread1Stack[BLINK_THREAD_STACK_SIZE / sizeof(uint32_t)];

void BlinkThread_entry(uint32_t parameter)
{
    (void)parameter;

    while (1)
    {
        sl_led_toggle(&sl_led_led0);
        tx_thread_sleep(TX_TICKS_PER_MILLISEC(500));
    }
}

void BlinkThread1_entry(uint32_t parameter)
{
    (void)parameter;

    while (1)
    {
        tx_thread_sleep(TX_TICKS_PER_MILLISEC(500));
        sl_led_toggle(&sl_led_led1);
    }
}

void tx_application_define(void *first_unused_memory)
{
    (void)first_unused_memory;
    uint16_t status;

    // Create a byte memory pool from which to allocate the thread stacks.
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", memory_area, DEFAULT_BYTE_POOL_SIZE);

    // starts the serial driver
    // sdStart(&SERIAL_DRIVER, NULL);

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
        blinkThreadStack,
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
        blinkThread1Stack,
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

    // report successfull nanoBooter execution
    ReportSuccessfullNanoBooter();
}

//  Application entry point.
int main(void)
{
    // Initialize the board
    sl_system_init();

    // init boot clipboard
    InitBootClipboard();

    // the following IF is not mandatory, it's just providing a way for a user to 'force'
    // the board to remain in nanoBooter and not launching nanoCLR

    // // check if there is a request to remain on nanoBooter
    // if (!IsToRemainInBooter())
    // {
    //     // if the USER button (blue one) is pressed, skip the check for a valid CLR image and remain in booter
    //     if (palReadPad(GPIOC, GPIOC_BUTTON_USER))
    //     {
    //         // check for valid CLR image
    //         // we are checking for a valid image at the deployment address, which is pointing to the CLR address
    //         if (CheckValidCLRImage((uint32_t)&__deployment_start__))
    //         {
    //             // there seems to be a valid CLR image
    //             // launch nanoCLR
    //             LaunchCLR((uint32_t)&__deployment_start__);
    //         }
    //     }
    // }

/*Set unbuffered mode for stdout (newlib)*/
//       setvbuf(stdout, NULL, _IONBF, 0);  
//       /*Set unbuffered mode for stdin (newlib)*/
//   setvbuf(stdin, NULL, _IONBF, 0);   


    // Enter the ThreadX kernel. Task(s) created in tx_application_define() will start running
    sl_system_kernel_start();
}
