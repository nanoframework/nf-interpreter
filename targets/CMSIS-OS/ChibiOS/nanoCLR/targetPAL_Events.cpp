//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_Events.h>
#include <nanoPAL.h>
#include <target_platform.h>
#include <hal.h>

uint64_t CPU_MiliSecondsToSysTicks(uint64_t miliSeconds);

// events timer
static virtual_timer_t eventsBoolTimer;
volatile uint32_t systemEvents;

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

    chVTResetI(&eventsBoolTimer);

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

    *timerCompleteFlag = true;
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

    // we assume only 1 can be active, abort previous just in case
    chVTResetI(&eventsBoolTimer);

    if(timerCompleteFlag != NULL)
    {
        chVTSetI(&eventsBoolTimer, MS2ST(millisecondsFromNow), local_Events_SetBoolTimer_Callback, timerCompleteFlag);
    }
}

uint32_t Events_WaitForEvents( uint32_t powerLevel, uint32_t wakeupSystemEvents, uint32_t timeout_Milliseconds )
{

#if defined(HAL_PROFILE_ENABLED)
    Events_WaitForEvents_Calls++;
#endif

    uint64_t CountsRemaining = CPU_MiliSecondsToSysTicks( (uint64_t)timeout_Milliseconds );
    uint64_t Expire           = HAL_Time_CurrentSysTicks() + CountsRemaining;
 
    while( true )
    {
        uint32_t Events = Events_MaskedRead( wakeupSystemEvents );
        if(Events) 
            return Events;

        if(Expire <= HAL_Time_CurrentSysTicks())
            break;

       // no events, release time to OS
        osDelay(1);
    }

    return 0;
}

void FreeManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2)
{
    NATIVE_PROFILE_PAL_EVENTS();

    // TODO: not sure if this is really needed here... just kept it for further investigation
    // switch(category)
    // {
    //     //case EVENT_GESTURE:
    //     case EVENT_TOUCH:
    //         break;
    //     default:
    //         break;
    // }
}
