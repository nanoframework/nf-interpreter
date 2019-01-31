//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_os.h>

#include "nanoCLR_Types.h"
#include "nanoCLR_Runtime.h"

uint32_t GenericPort_Write( int portNum, const char* data, size_t size )
{
    // FIXME
    // It's only enabled if the C# programm is in running mode and no source level debugger (C# debugger)
    // is attached. In all other conditions it's better to be quiet.
    // If the debugger is attached we don't output the data because the debugger use the same serial link.
    // Maybe later it can also redirect output to the ESP32 app trace 
    // which is redirected to JTAG/openocd interface
    (void)portNum;
    if (g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions == CLR_RT_ExecutionEngine::c_fDebugger_StateProgramRunning)
    {
        char* p = (char*)data;
        int counter = 0;

        // send characters directly to the trace port
        while(*p != '\0' || counter < (int)size)
        {
            ets_printf( "%c", *p++); 
            counter++;
        }
        return counter;
    }
    else
    {
        return (uint32_t)size;
    }
}
