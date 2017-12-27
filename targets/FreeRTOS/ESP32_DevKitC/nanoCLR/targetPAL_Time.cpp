//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL.h>
#include <target_platform.h>
#include <Esp32_os.h>

static uint64_t g_nextEvent;   // tick time of next event to be scheduled

HRESULT Time_Initialize()
{
    g_nextEvent = 0xFFFFFFFFFFFF; // never

    // nothing to do here has time management is handled by ChibiOS
    return S_OK;
}

HRESULT Time_Uninitialize()
{
    // nothing to do here has time management is handled by ChibiOS
    return S_OK;
}

void Time_SetCompare ( uint64_t compareValue )
{
// TODO
// setup timer with callback that calls HAL_COMPLETION::DequeueAndExec( );
// see Events_SetBoolTimer

    g_nextEvent = compareValue;
}

extern "C" {

void Time_Interrupt_Hook()
{
    if (HAL_Time_CurrentSysTicks() >= g_nextEvent && g_nextEvent > 0) 
    { 
        // handle event
        HAL_COMPLETION::DequeueAndExec(); // this also schedules the next one, if there is one
    }    
}

}
