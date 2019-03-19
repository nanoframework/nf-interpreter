//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_Events.h>
#include <nanoCLR_Runtime.h>
#include <nanoPAL.h>
#include <target_platform.h>
#include <hal.h>

uint64_t CPU_MillisecondsToTicks(uint64_t ticks);

// timer for bool events
static virtual_timer_t boolEventsTimer;
uint32_t systemEvents;

set_Event_Callback g_Event_Callback     = NULL;
void*              g_Event_Callback_Arg = NULL;

bool Events_Initialize()
{
    NATIVE_PROFILE_PAL_EVENTS();

    // init events
    chSysLock();
    systemEvents = 0;
    chSysUnlock();

    return true;
}

bool Events_Uninitialize()
{
    NATIVE_PROFILE_PAL_EVENTS();

    chVTResetI(&boolEventsTimer);

    return true;
}

void Events_Set( uint32_t events )
{
    NATIVE_PROFILE_PAL_EVENTS();

    // set events
    chSysLock();
    systemEvents |= events;
    chSysUnlock();

    if( g_Event_Callback != NULL )
    {
        g_Event_Callback( g_Event_Callback_Arg );
    }
}

uint32_t Events_Get( uint32_t eventsOfInterest )
{
    NATIVE_PROFILE_PAL_EVENTS();

    // get the requested flags from system events state and...
    uint32_t returnEvents = (systemEvents & eventsOfInterest);

    // ... clear the requested flags atomically
    chSysLock();
    systemEvents &= ~eventsOfInterest;
    chSysUnlock();
    
    // give the caller notice of just the events they asked for ( and were cleared already )
    return returnEvents;
}

uint32_t Events_MaskedRead( uint32_t eventsOfInterest )
{
    NATIVE_PROFILE_PAL_EVENTS();
    return (systemEvents & eventsOfInterest);
}

static void local_Events_SetBoolTimer_Callback( void* arg )
{
    NATIVE_PROFILE_PAL_EVENTS();
    bool* timerCompleteFlag = (bool*)arg;

    chSysLock();
    
    *timerCompleteFlag = true;

    chSysUnlock();
}

void Events_SetCallback( set_Event_Callback pfn, void* arg )
{
    NATIVE_PROFILE_PAL_EVENTS();

    g_Event_Callback     = pfn;
    g_Event_Callback_Arg = arg;
}

void Events_SetBoolTimer( bool* timerCompleteFlag, uint32_t millisecondsFromNow )
{
    NATIVE_PROFILE_PAL_EVENTS();

    if(timerCompleteFlag != NULL)
    {
        // no need to stop the timer even if it's running because the API does it anyway
        chVTSetI(&boolEventsTimer, TIME_MS2I(millisecondsFromNow), local_Events_SetBoolTimer_Callback, timerCompleteFlag);
    }
}

uint32_t Events_WaitForEvents( uint32_t powerLevel, uint32_t wakeupSystemEvents, uint32_t timeoutMilliseconds )
{
    // schedule an interrupt for this far in the future
    // timeout is in milliseconds, need to convert to ticks
    uint64_t countsRemaining = CPU_MillisecondsToTicks( timeoutMilliseconds );

#if defined(HAL_PROFILE_ENABLED)
    Events_WaitForEvents_Calls++;
#endif

    uint64_t expireTimeInTicks  = HAL_Time_CurrentTime() + countsRemaining;
    bool runContinuations = true;

    while(true)
    {
        EVENTS_HEART_BEAT;

        uint32_t events = Events_MaskedRead( wakeupSystemEvents );
        if(events)
        {
            return events;
        }

        if(expireTimeInTicks <= HAL_Time_CurrentTime())
        {
            break;
        }

        // first check and possibly run any continuations
        // but only if we have slept after stalling
        if(runContinuations && !SystemState_QueryNoLock(SYSTEM_STATE_NO_CONTINUATIONS))
        {
            // if we stall on time, don't check again until after we sleep
            runContinuations = HAL_CONTINUATION::Dequeue_And_Execute();
        }
        else
        {
            // try stalled continuations again after sleeping
            runContinuations = true;

            HAL_COMPLETION::WaitForInterrupts(expireTimeInTicks, powerLevel, wakeupSystemEvents );          
        }

        // no events, pass control to the OS
        osThreadYield();

        // check if reboot or exit flags were set when the other OS threads executed
        if(CLR_EE_DBG_IS(RebootPending) || CLR_EE_DBG_IS(ExitPending))
        {
            break;
        }
    }

    return 0;
}

void FreeManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2)
{
    (void)category;
    (void)subCategory;
    (void)data1;
    (void)data2;

    NATIVE_PROFILE_PAL_EVENTS();
}
