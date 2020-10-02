//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ti/drivers/UART2.h>
#include <board.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti_drivers_config.h>

extern UART2_Handle uart;

SemaphoreP_Handle uartMutex;

// Need to have calls to these two functions in C code.
// Because they are called only on asm code, GCC linker with LTO option thinks they are not used and just removes them.
// Having them called from a dummy function that is never called it a workaround for this.
// The clean alternative would be to add the GCC attribute used in those functions, but that's not our code to touch.

void dummyFunction(void) __attribute__((used));

// Never called.
void dummyFunction(void)
{
    vTaskSwitchContext();
    localProgramStart();
}

// configure UART
void ConfigUART()
{
    UART2_Params uartParams;

    // Create a UART with data processing off
    UART2_Params_init(&uartParams);

    uartParams.writeMode = UART2_Mode_BLOCKING;
    uartParams.readMode = UART2_Mode_BLOCKING;
    uartParams.baudRate = 921600;
    uartParams.readReturnMode = UART2_ReadReturnMode_FULL;

    uart = UART2_open(UART0, &uartParams);

    if (uart == NULL)
    {
        // UART_open() failed
        while (1)
            ;
    }

    uartMutex = SemaphoreP_createBinary(1);
    if (uartMutex == NULL)
    {
        // failed to create semaphore
        while (1)
            ;
    }
}

void __error__(char *pcFilename, unsigned long ulLine)
{
    //
    // Something horrible happened! You need to look
    // at file "pcFilename" at line "ulLine" to see
    // what error is being reported.
    //
    while (1)
    {
    }
}
