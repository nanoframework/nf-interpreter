//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <ti/drivers/UART2.h>
#include <Board.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti_drivers_config.h>

extern UART2_Handle uart;

SemaphoreP_Handle uartMutex;

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
