//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOPAL_EVENTS_H_
#define _NANOPAL_EVENTS_H_ 1

#include <nanoWeak.h>
#include <netmf_errors.h>

//
// !!! KEEP IN SYNC WITH Microsoft.SPOT.Hardware.SleepLevel !!!
//
enum SLEEP_LEVEL
{
    SLEEP_LEVEL__AWAKE         = 0x00,
    SLEEP_LEVEL__SELECTIVE_OFF = 0x10,
    SLEEP_LEVEL__SLEEP         = 0x20,
    SLEEP_LEVEL__DEEP_SLEEP    = 0x30,
    SLEEP_LEVEL__OFF           = 0x40,
};

#define EVENTS_TIMEOUT_INFINITE 0xFFFFFFFF

#if NATIVE_PROFILE_PAL & NATIVE_PROFILE_PAL_EVENTS__flag
    #define NATIVE_PROFILE_PAL_EVENTS() Native_Profiler profiler_obj
#else
    #define NATIVE_PROFILE_PAL_EVENTS()
#endif

// destructive read system event flags
unsigned int Events_Get( unsigned int EventsOfInterest );

// non-destructive read system event flags
unsigned int Events_MaskedRead( unsigned int EventsOfInterest );

// returns 0 for timeout, non-zero are events that have happened and were asked to be waiting on (non-destructive read)
// timeout limit is about 3034 milliseconds currently
// values greater than this are capped to this

// sleep relative time into the future, or until a SystemEvent occurs, which occurs first
// timeout is a non-negative number of 1mSec ticks, or -1 (any negative value) to sleep forever until a SystemEvent occurs

// Events_WaitForEvents(0, n), sleeps for n milliseconds independent of events
// Events_WaitForEvents(0, EVENTS_TIMEOUT_INFINITE) sleeps forever.  Don't do that.
// Events_WaitForEvents(flags, EVENTS_TIMEOUT_INFINITE) waits forever for that event.

unsigned int Events_WaitForEvents        ( unsigned int sleepLevel, unsigned int WakeupSystemEvents, unsigned int Timeout_Milliseconds );
unsigned int Events_WaitForEventsInternal( unsigned int sleepLevel, unsigned int WakeupSystemEvents, unsigned int Timeout_Milliseconds );

__inline unsigned int Events_WaitForEvents( unsigned int WakeupSystemEvents, unsigned int Timeout_Milliseconds )
{
    return Events_WaitForEvents( SLEEP_LEVEL__SLEEP, WakeupSystemEvents, Timeout_Milliseconds );
}

__nfweak bool Events_Initialize();
__nfweak bool Events_Uninitialize();
__nfweak void Events_SetBoolTimer( bool* timerCompleteFlag, unsigned int millisecondsFromNow );

#endif //_NANOPAL_EVENTS_H_
