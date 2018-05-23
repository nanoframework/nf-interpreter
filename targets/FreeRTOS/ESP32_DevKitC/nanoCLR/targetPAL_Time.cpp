//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL.h>
#include <target_platform.h>
#include <Esp32_os.h>

static TimerHandle_t nextEventTimer;

static void NextEventTimer_Callback( TimerHandle_t xTimer )
{
    // this call also schedules the next one, if there is one
    HAL_COMPLETION::DequeueAndExec();
}

HRESULT Time_Initialize()
{
    nextEventTimer = xTimerCreate( "NextEventTimer", 10, pdFALSE, (void *)0, NextEventTimer_Callback);

    return S_OK;
}

HRESULT Time_Uninitialize()
{
    // nothing to do here has time management is handled by FreeRTOS
    return S_OK;
}

void Time_SetCompare ( uint64_t compareValueTicks )
{
    // can have only one event timer setup, abort previous just in case
    xTimerStop( nextEventTimer, 0 );

    if(compareValueTicks == 0)
    {
        // compare value is 0 so dequeue and schedule immediately 
        NextEventTimer_Callback(NULL);
    }
    else
    {
        // need to convert from ticks to milliseconds
        xTimerChangePeriod( nextEventTimer, (compareValueTicks * TIME_CONVERSION__TO_MILLISECONDS) / portTICK_PERIOD_MS,  0 );
    }
}
