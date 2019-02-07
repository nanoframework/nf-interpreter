#include <stdint.h>

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include "FreeRTOS.h"
#include "task.h"

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>

/* Example/Board Header files */
#include "Board.h"

extern void * mainThread(void *arg0);


//////////////////////////////
#define SL_STOP_TIMEOUT         (200)
#define APPLICATION_NAME        "out of box"
#define APPLICATION_VERSION     "1.00.00.09"

//////////////////////////////

/* Stack size in bytes */
#define THREADSTACKSIZE   4096

/*
 *  ======== main ========
 */
int main(void)
{
    pthread_t thread;
    pthread_attr_t pAttrs;
    struct sched_param priParam;
    int retc;
    int detachState;

    /* Call board init functions */
    Board_initGeneral();

    /* Set priority and stack size attributes */
    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 1;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&pAttrs, detachState);
    if(retc != 0)
    {
        /* pthread_attr_setdetachstate() failed */
        while(1)
        {
            ;
        }
    }

    pthread_attr_setschedparam(&pAttrs, &priParam);

    retc |= pthread_attr_setstacksize(&pAttrs, THREADSTACKSIZE);
    if(retc != 0)
    {
        /* pthread_attr_setstacksize() failed */
        while(1)
        {
            ;
        }
    }

    retc = pthread_create(&thread, &pAttrs, mainThread, NULL);
    if(retc != 0)
    {
        /* pthread_create() failed */
        while(1)
        {
            ;
        }
    }

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    return (0);
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask,
                                   char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}

void vApplicationTickHook(void)
{
    /*
     * This function will be called by each tick interrupt if
     * configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
     * added here, but the tick hook is called from an interrupt context, so
     * code must not attempt to block, and only the interrupt safe FreeRTOS API
     * functions can be used (those that end in FromISR()).
     */
}

void vPreSleepProcessing(uint32_t ulExpectedIdleTime)
{
}

//*****************************************************************************
//
//! \brief Application defined idle task hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
vApplicationIdleHook(void)
{
    /* Handle Idle Hook for Profiling, Power Management etc */
}

//*****************************************************************************
//
//! \brief  Overwrite the GCC _sbrk function which check the heap limit related
//!         to the stack pointer.
//!         In case of freertos this checking will fail.
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void * _sbrk(uint32_t delta)
{
    extern char _end;     /* Defined by the linker */
    extern char __HeapLimit;
    static char *heap_end;
    static char *heap_limit;
    char *prev_heap_end;

    if(heap_end == 0)
    {
        heap_end = &_end;
        heap_limit = &__HeapLimit;
    }

    prev_heap_end = heap_end;
    if(prev_heap_end + delta > heap_limit)
    {
        return((void *) -1L);
    }
    heap_end += delta;
    return((void *) prev_heap_end);
}


// /*****************************************************************************
//                  Local Functions
// *****************************************************************************/
// //*****************************************************************************
// //
// //! Application Boarders display on UART
// //!
// //! \param  ch - Character to be displayed , n - number of time to display
// //!
// //! \return none
// //!
// //*****************************************************************************
// void printBorder(char ch,
//                  int n)
// {
//     int i = 0;

//     for(i = 0; i < n; i++)
//     {
//         putch(ch);
//     }
// }

// //*****************************************************************************
// //
// //! Application startup display on UART
// //!
// //! \param  none
// //!
// //! \return none
// //!
// //*****************************************************************************
// int32_t DisplayBanner(char * AppName,
//                       char * AppVer)
// {
//     int32_t ret = 0;
//     uint8_t macAddress[SL_MAC_ADDR_LEN];
//     uint16_t macAddressLen = SL_MAC_ADDR_LEN;
//     uint16_t ConfigSize = 0;
//     uint8_t ConfigOpt = SL_DEVICE_GENERAL_VERSION;
//     SlDeviceVersion_t ver = {0};
//     char lineBreak[] = "\n\r";
//     ConfigSize = sizeof(SlDeviceVersion_t);

//     /* Print device version info. */
//     ret =
//         sl_DeviceGet(SL_DEVICE_GENERAL, &ConfigOpt, &ConfigSize,
//                      (uint8_t*)(&ver));
//     ASSERT_ON_ERROR(ret);

//     /* Print device Mac address */
//     ret = sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET, 0, &macAddressLen,
//                        &macAddress[0]);
//     ASSERT_ON_ERROR(ret);

//     UART_PRINT(lineBreak);
//     UART_PRINT("\t");
//     printBorder('=', 44);
//     UART_PRINT(lineBreak);
//     UART_PRINT("\t   %s Example Ver: %s",AppName, AppVer);
//     UART_PRINT(lineBreak);
//     UART_PRINT("\t");
//     printBorder('=', 44);
//     UART_PRINT(lineBreak);
//     UART_PRINT(lineBreak);
//     UART_PRINT("\t CHIP: 0x%x",ver.ChipId);
//     UART_PRINT(lineBreak);
//     UART_PRINT("\t MAC:  %d.%d.%d.%d",ver.FwVersion[0],ver.FwVersion[1],
//                ver.FwVersion[2],
//                ver.FwVersion[3]);
//     UART_PRINT(lineBreak);
//     UART_PRINT("\t PHY:  %d.%d.%d.%d",ver.PhyVersion[0],ver.PhyVersion[1],
//                ver.PhyVersion[2],
//                ver.PhyVersion[3]);
//     UART_PRINT(lineBreak);
//     UART_PRINT("\t NWP:  %d.%d.%d.%d",ver.NwpVersion[0],ver.NwpVersion[1],
//                ver.NwpVersion[2],
//                ver.NwpVersion[3]);
//     UART_PRINT(lineBreak);
//     UART_PRINT("\t ROM:  %d",ver.RomVersion);
//     UART_PRINT(lineBreak);
//     UART_PRINT("\t HOST: %s", SL_DRIVER_VERSION);
//     UART_PRINT(lineBreak);
//     UART_PRINT("\t MAC address: %02x:%02x:%02x:%02x:%02x:%02x", macAddress[0],
//                macAddress[1], macAddress[2], macAddress[3], macAddress[4],
//                macAddress[5]);
//     UART_PRINT(lineBreak);
//     UART_PRINT(lineBreak);
//     UART_PRINT("\t");
//     printBorder('=', 44);
//     UART_PRINT(lineBreak);
//     UART_PRINT(lineBreak);

//     return(ret);
// }

//*****************************************************************************
//
//! This  function clears and enables a GPIO pin interrupt flag
//!
//! \param    index - GPIO index
//!
//! \return None
//!
//*****************************************************************************
static void GPIO_clearAndEnable(uint8_t index)
{
    GPIO_clearInt(index);
    GPIO_enableInt(index);
}


void * mainThread(void *arg)
{
    int32_t RetVal;
    pthread_attr_t pAttrs;
    pthread_attr_t pAttrs_spawn;
    struct sched_param priParam;
    struct timespec ts = {0};

    GPIO_init();
    SPI_init();
    I2C_init();

    /* init Terminal, and print App name */
    //InitTerm();

    /* initialize the realtime clock */
    clock_settime(CLOCK_REALTIME, &ts);

    /* Switch off all LEDs on boards */
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);

    // /* create the sl_Task */
    // pthread_attr_init(&pAttrs_spawn);
    // priParam.sched_priority = SPAWN_TASK_PRIORITY;
    // RetVal = pthread_attr_setschedparam(&pAttrs_spawn, &priParam);
    // RetVal |= pthread_attr_setstacksize(&pAttrs_spawn, TASK_STACK_SIZE);

    // RetVal = pthread_create(&gSpawnThread, &pAttrs_spawn, sl_Task, NULL);

    // if(RetVal)
    // {
    //     /* Handle Error */
    //     UART_PRINT("Unable to create sl_Task thread \n");
    //     while(1)
    //     {
    //         ;
    //     }
    // }
    // RetVal = sl_Start(0, 0, 0);
    // if(RetVal >= 0)
    // {
    //     DisplayBanner(APPLICATION_NAME, APPLICATION_VERSION);
    //     RetVal = sl_Stop(SL_STOP_TIMEOUT);
    //     if(RetVal < 0)
    //     {
    //         /* Handle Error */
    //         UART_PRINT("\n sl_Stop failed\n");
    //         while(1)
    //         {
    //             ;
    //         }
    //     }
    // }
    // else if((RetVal < 0) && (RetVal != SL_ERROR_RESTORE_IMAGE_COMPLETE))
    // {
    //     /* Handle Error */
    //     UART_PRINT("\n sl_Start failed\n");
    //     UART_PRINT("\n %s Example Ver. %s\n",APPLICATION_NAME,
    //                APPLICATION_VERSION);
    //     while(1)
    //     {
    //         ;
    //     }
    // }

    while(1)
    {
        /* Turn on user LED */
        GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
        usleep(50000);
        /* Turn off user LED */
        GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
        usleep(50000);
    }

    return(0);
}