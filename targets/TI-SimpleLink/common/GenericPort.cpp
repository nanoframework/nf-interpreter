//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoCLR_Types.h>
#include <nanoCLR_Runtime.h>
#include <ti/drivers/UART.h>

extern UART_Handle uart;

uint32_t GenericPort_Write( int portNum, const char* data, size_t size )
{
    (void)portNum;

    // developer note:
    // Outputing to board UART can be done only if there is no debugger attached, because it uses the same UART.

    if( (g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions & CLR_RT_ExecutionEngine::c_fDebugger_Enabled) == CLR_RT_ExecutionEngine::c_fDebugger_Enabled)
    {
        // debugger port is in use, don't output to UART
    }
    else
    {
        // send characters directly to the UART port
        UART_write(uart, data, size);

        return size;
    }

    return (uint32_t)size;
}
