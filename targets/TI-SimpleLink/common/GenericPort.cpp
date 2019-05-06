//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nanoCLR_Types.h"
#include "nanoCLR_Runtime.h"

uint32_t GenericPort_Write( int portNum, const char* data, size_t size )
{
    (void)portNum;
    if (g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions == CLR_RT_ExecutionEngine::c_fDebugger_StateProgramRunning)
    {
        char* p = (char*)data;
        int counter = 0;

        // send characters directly to the trace port
        while(*p != '\0' || counter < (int)size)
        {
            // ets_printf( "%c", *p++); 
            counter++;
        }
        return counter;
    }
    else
    {
        return (uint32_t)size;
    }
}
