//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#if defined(FEATURE_TRACE_TO_STDIO)
#include <hal.h>
#include <nanoHAL_v2.h>
#include <osal.h>
#if (HAL_USE_SERIAL == TRUE)
#include <serialcfg.h>
#elif (HAL_USE_UART == TRUE)
#include <uartcfg.h>
#endif

#include "nanoCLR_Types.h"
#include "nanoCLR_Runtime.h"

uint32_t StdioPort_Write(int portNum, const char *data, size_t size)
{
//#if defined(NF_FEATURE_TRACE_TO_STDIO)
#if (HAL_USE_SERIAL == TRUE)
    (void)portNum;
    //(void)data;
    // return size;
    size_t writeResult =
        chnWriteTimeout(&SERIAL_DRIVER_STDIO, (const uint8_t *)data, size, 1000); //(systime_t)TIMEOUT_INFINITE);
    return (uint32_t)writeResult;
//#endif
#elif (STM32_UART_USE_UART4 == TRUE)
    (void)portNum;
    uartStartSend(&UART_DRIVER_STDIO, size, (const uint8_t *)data); // const void *txbuf);
    return (uint32_t)writeResult;
#else
    // FIXME
    // It's only enabled if the C# programm is in running mode and no source level debugger (C# debugger)
    // is attached. In all other conditions it's better to be quiet.
    // If the debugger is attached we don't output the data because the debugger use the same serial link.
    // Maybe later it can also redirect output to the ESP32 app trace
    // which is redirected to JTAG/openocd interface
    (void)portNum;
    if (g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions == CLR_RT_ExecutionEngine::c_fDebugger_StateProgramRunning)
    {

        char *p = (char *)data;
        int counter = 0; // send characters directly to the trace port
        while (*p != '\0' || counter < (int)size)
        {
            // need putc(*p++) or printf( "%c", *p++);
            counter++;
        }
        return counter;
    }
    else
    {
        return (uint32_t)size;
    }
#endif
}
#endif
