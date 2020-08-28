//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <ti/drivers/UART.h>
#include <board.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/sysbios/knl/Clock.h>

extern UART_Handle uart;

SemaphoreP_Handle uartMutex;

// Need to have calls to these two functions in C code.
// Because they are called only on asm code, GCC linker with LTO option thinks they are not used and just removes them.
// Having them called from a dummy function that is never called it a workaround for this.
// The clean alternative would be to add the GCC attribute used in those functions, but that's not our code to touch.

void dummyFunction(void) __attribute__((used));

// UART operations timeout
#define UART_TIMEOUT_MILLISECONDS 500000

// Never called.
void dummyFunction(void)
{
    vTaskSwitchContext();
    localProgramStart();
}

// configure UART
void ConfigUART()
{
    UART_Params uartParams;

    // Create a UART with data processing off
    UART_Params_init(&uartParams);

    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 921600;
    uartParams.readTimeout = UART_TIMEOUT_MILLISECONDS / Clock_tickPeriod;
    uartParams.writeTimeout = UART_TIMEOUT_MILLISECONDS / Clock_tickPeriod;

    uart = UART_open(Board_UART0, &uartParams);

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
