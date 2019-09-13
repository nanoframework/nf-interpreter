//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <stdint.h>
#include <nanoCLR_Application.h>
// // POSIX Header files
// #include <pthread.h>
// #include <unistd.h>

// RTOS header files
//#include "FreeRTOS.h"
//#include "task.h"
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

// board Header files
#include <Board.h>

//////////////////////////////

// Stack size in bytes
#define THREADSTACKSIZE   2048

extern void * mainThread(void *arg0);
Task_Handle receiverHandle;
Task_Handle clrHandle;

extern void * ReceiverThread(void *arg0);
extern void * CLRStartupThread(void *arg0);


int main(void)
{
    Task_Params taskParams;


    // Call board init functions
    Board_initGeneral();

    GPIO_init();
    UART_init();
    ConfigUART();

    // Switch off all LEDs on board
    GPIO_write(Board_GPIO_RLED, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_GLED, Board_GPIO_LED_OFF);

    // setup Task thread
    Task_Params_init(&taskParams);
    taskParams.stackSize = THREADSTACKSIZE;
    taskParams.priority = 1;

    // create Receiver
    receiverHandle = Task_create((Task_FuncPtr)ReceiverThread, &taskParams, Error_IGNORE);
    if (receiverHandle == NULL) {
        while (1);
    }

    // CLR settings to launch CLR thread
    CLR_SETTINGS clrSettings;
    (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

    clrSettings.MaxContextSwitches         = 50;
    clrSettings.WaitForDebugger            = false;
    clrSettings.EnterDebuggerLoopAfterExit = true;

    // create CLR
    clrHandle = Task_create((Task_FuncPtr)CLRStartupThread, &taskParams, Error_IGNORE);
    if (clrHandle == NULL) {
        while (1);
    }
    
    BIOS_start();

    return (0);
}
