//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <stdint.h>
#include <nanoCLR_Application.h>

// RTOS header files
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

// board Header files
#include <Board.h>

//////////////////////////////

// Stack size in bytes
#define THREADSTACKSIZE   2048

Task_Handle receiverHandle;
Task_Handle clrHandle;

CLR_SETTINGS clrSettings;

extern void ReceiverThread(UArg arg0, UArg arg1);
extern void CLRStartupThread(UArg arg0, UArg arg1);

int main(void)
{
    Task_Params taskParams;

    // Call board init functions
    Board_initGeneral();

    // setup Task thread
    Task_Params_init(&taskParams);
    taskParams.stackSize = THREADSTACKSIZE;
    taskParams.priority = 4;

    // create Receiver
    receiverHandle = Task_create((Task_FuncPtr)ReceiverThread, &taskParams, Error_IGNORE);
    if (receiverHandle == NULL)
    {
        while (1);
    }

    // CLR settings to launch CLR thread   
    (void)memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

    clrSettings.MaxContextSwitches         = 50;
    clrSettings.WaitForDebugger            = false;
    clrSettings.EnterDebuggerLoopAfterExit = true;

    // setup CLR task
    taskParams.arg0 = (UArg)&clrSettings;
    taskParams.stackSize = THREADSTACKSIZE;
    taskParams.priority = 4;
    clrHandle = Task_create(CLRStartupThread, &taskParams, Error_IGNORE);
    if (clrHandle == NULL)
    {
        while (1);
    }

    GPIO_init();
    UART_init();
    ConfigUART();

    // Switch off all LEDs on board
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED2, Board_GPIO_LED_OFF);
    
    BIOS_start();

    return (0);
}
