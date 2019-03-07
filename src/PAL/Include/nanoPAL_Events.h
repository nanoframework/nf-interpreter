//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NANOPAL_EVENTS_H_
#define _NANOPAL_EVENTS_H_ 1

#include <nanoHAL.h>
#include <nanoWeak.h>

typedef void (*set_Event_Callback)( void* );

#define EVENTS_TIMEOUT_INFINITE 0xFFFFFFFF

#if NATIVE_PROFILE_PAL & NATIVE_PROFILE_PAL_EVENTS__flag
    #define NATIVE_PROFILE_PAL_EVENTS() Native_Profiler profiler_obj
#else
    #define NATIVE_PROFILE_PAL_EVENTS()
#endif


 bool Events_Initialize();
 bool Events_Uninitialize();
 void Events_Set( uint32_t events );

// destructive read system event flags
 uint32_t Events_Get( uint32_t eventsOfInterest );

// non-destructive read system event flags
 uint32_t Events_MaskedRead( uint32_t eventsOfInterest );

// returns 0 for timeout, non-zero are events that have happened and were asked to be waiting on (non-destructive read)
// timeout limit is about 3034 milliseconds currently
// values greater than this are capped to this

// sleep relative time into the future, or until a SystemEvent occurs, which occurs first
// timeout is a non-negative number of 1mSec ticks, or -1 (any negative value) to sleep forever until a SystemEvent occurs

// Events_WaitForEvents(0, n), sleeps for n milliseconds independent of events
// Events_WaitForEvents(0, EVENTS_TIMEOUT_INFINITE) sleeps forever.  Don't do that.
// Events_WaitForEvents(flags, EVENTS_TIMEOUT_INFINITE) waits forever for that event.

uint32_t Events_WaitForEvents( uint32_t powerLevel, uint32_t wakeupSystemEvents, uint32_t timeoutMilliseconds );

__inline uint32_t Events_WaitForEvents( uint32_t wakeupSystemEvents, uint32_t timeoutMilliseconds )
{
    return Events_WaitForEvents( SLEEP_LEVEL__SLEEP, wakeupSystemEvents, timeoutMilliseconds );
}

 void Events_SetBoolTimer( bool* timerCompleteFlag, uint32_t millisecondsFromNow );
 void Events_SetCallback( set_Event_Callback pfn, void* arg );
 void FreeManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2);


#ifdef __cplusplus
extern "C" {
#endif

    void PostManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2);

#ifdef __cplusplus
}
#endif

//--//

typedef void (*PALEVENTLISTENER) (uint32_t e, uint32_t param);

struct PalEventListener : public HAL_DblLinkedNode<PalEventListener>
{
    PALEVENTLISTENER m_palEventListener;
    uint32_t     m_eventMask;
};


HRESULT PalEvent_Initialize();
HRESULT PalEvent_Uninitialize();
HRESULT PalEvent_Post(uint32_t e, uint32_t param);
HRESULT PalEvent_Enlist(PalEventListener* listener);

//--//

struct PalEventDriver
{
public:

    HAL_DblLinkedList<PalEventListener> listenerList;

    static HRESULT Initialize();
    static HRESULT Uninitialize();
    static HRESULT PostEvent(uint32_t e, uint32_t param);
    static HRESULT EnlistListener(PalEventListener* listener);

private:
    static bool s_initialized;
};

extern PalEventDriver g_palEventDriver;

#endif //_NANOPAL_EVENTS_H_
