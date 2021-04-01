//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Types.h>
#include <nanoCLR_Runtime.h>
#include <ti/drivers/UART2.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/sysbios/knl/Clock.h>

// UART operations timeout
#define UART_TIMEOUT_MILLISECONDS 500000

extern UART2_Handle uart;
extern SemaphoreP_Handle uartMutex;

// developer note:
// Outputing to board UART can be done only if there is no debugger attached, because it uses the same UART.

extern "C" uint32_t DebuggerPort_WriteProxy(const char *format, ...)
{
    va_list arg;
    uint32_t chars = 0;

    if (CLR_EE_DBG_IS_NOT(Enabled))
    {
        if (SemaphoreP_pend(uartMutex, UART2_WAIT_FOREVER) == SemaphoreP_OK)
        {
            va_start(arg, format);

            chars = CLR_Debug::PrintfV(format, arg);

            va_end(arg);

            SemaphoreP_post(uartMutex);
        }
    }

    return chars;
}

uint32_t GenericPort_Write(int portNum, const char *data, size_t size)
{
    (void)portNum;
    size_t bytesWritten;

    if (CLR_EE_DBG_IS_NOT(Enabled))
    {
        // debugger port is NOT in use, OK to output to UART
        // send characters directly to the UART port
        UART2_writeTimeout(uart, data, size, &bytesWritten, UART_TIMEOUT_MILLISECONDS / Clock_tickPeriod);

        return bytesWritten;
    }

    return (uint32_t)size;
}
