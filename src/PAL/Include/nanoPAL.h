//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOPAL_H_
#define _NANOPAL_H_ 1

//#include <nanoHAL_Types.h>

#include <nanoHAL.h>
#include <nanoPAL_Time.h>
#include <nanoHAL_v2.h>
#include <nanoPAL_BlockStorage.h>

//#include <AsyncProcCalls_decl.h>

typedef void (*HAL_CALLBACK_FPN)( void* arg );

struct HAL_CALLBACK
{
public:
    HAL_CALLBACK_FPN EntryPoint;
    void*            Argument;

public:
    void Initialize( HAL_CALLBACK_FPN EntryPoint, void* Argument )
    {
        this->EntryPoint = EntryPoint;
        this->Argument   = Argument;
    }

    void SetArgument( void* Argument )
    {
        this->Argument = Argument;
    }

    HAL_CALLBACK_FPN GetEntryPoint() const { return this->EntryPoint; }
    void*            GetArgument  () const { return this->Argument  ; }

    void Execute() const
    {
        HAL_CALLBACK_FPN EntryPoint = this->EntryPoint;
        void*            Argument   = this->Argument; 

        if(EntryPoint)
        {
            EntryPoint( Argument );
        }
    }
};


struct HAL_CONTINUATION : public HAL_DblLinkedNode<HAL_CONTINUATION>
{

private:
    HAL_CALLBACK Callback;

public:
    void InitializeCallback( HAL_CALLBACK_FPN EntryPoint, void* Argument = NULL );

    void SetArgument( void* Argument )
    {
        Callback.SetArgument( Argument );
    }

    HAL_CALLBACK_FPN GetEntryPoint() const { return Callback.GetEntryPoint(); }
    void*            GetArgument  () const { return Callback.GetArgument  (); }

    void Execute() const { Callback.Execute(); }

    bool IsLinked();
    void Enqueue();
    void Abort  ();

    //--//

    static void Uninitialize();

    static void InitializeList();

    static bool Dequeue_And_Execute();
};

//--//

struct HAL_COMPLETION : public HAL_CONTINUATION
{
    unsigned __int64 EventTimeTicks;
    bool   ExecuteInISR;

#if defined(_DEBUG)
    unsigned __int64 Start_RTC_Ticks;
#endif

    void InitializeForISR( HAL_CALLBACK_FPN EntryPoint, void* Argument = NULL )
    {
        ExecuteInISR = true;

        InitializeCallback( EntryPoint, Argument );
    }

    void InitializeForUserMode( HAL_CALLBACK_FPN EntryPoint, void* Argument = NULL )
    {
        ExecuteInISR = false;

        InitializeCallback( EntryPoint, Argument );
    }

    void EnqueueTicks               ( unsigned __int64 EventTimeTicks        );
    void EnqueueDelta64             ( unsigned __int64 uSecFromNow           );
    void EnqueueDelta               ( unsigned int uSecFromNow           );
    
    void Abort();

    void Execute();

    //--//

    static void Uninitialize();

    static void InitializeList();

    static void DequeueAndExec();

    static void WaitForInterrupts( unsigned __int64 Expire, unsigned int sleepLevel, unsigned __int64 wakeEvents );
};

//--//

//#include <StateDebounce_decl.h>
//#include <Backlight_decl.h>
//#include <BatteryCharger_decl.h>
//#include <BatteryMeasurement_decl.h>

#include <nanoPAL_COM.h>
// 











//
//#include <Display_decl.h>

//#include <Power_decl.h> (must be before events_decl.h)

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


//#include <events_decl.h>

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

#define EVENTS_TIMEOUT_INFINITE 0xFFFFFFFF

unsigned int Events_WaitForEvents        ( unsigned int sleepLevel, unsigned int WakeupSystemEvents, unsigned int Timeout_Milliseconds );
unsigned int Events_WaitForEventsInternal( unsigned int sleepLevel, unsigned int WakeupSystemEvents, unsigned int Timeout_Milliseconds );

__inline unsigned int Events_WaitForEvents( unsigned int WakeupSystemEvents, unsigned int Timeout_Milliseconds )
{
    return Events_WaitForEvents( SLEEP_LEVEL__SLEEP, WakeupSystemEvents, Timeout_Milliseconds );
}

void Events_SetBoolTimer(bool* TimerCompleteFlag, unsigned int MillisecondsFromNow);


//#include <palevent_decl.h>

//#include <heap_decl.h>

// This function returns location of the CLR heap.
void HeapLocation(unsigned char*& BaseAddress, unsigned int& SizeInBytes);


//#include <gesture_decl.h>
//#include <ink_decl.h>
//
//#include <graphics_decl.h>
//
//#include <I2C_decl.h>
//#include <io_decl.h>
//#include <instrumentation_decl.h>
//
//#include <FS_decl.h>
//
//#include <GPIO_button_decl.h>
//#include <piezo_decl.h>
//

//#include <Power_decl.h>

//
// !!! KEEP IN SYNC WITH Microsoft.SPOT.Hardware.PowerLevel !!!
//
enum POWER_LEVEL
{
    POWER_LEVEL__HIGH_POWER = 0x10,
    POWER_LEVEL__MID_POWER  = 0x20,
    POWER_LEVEL__LOW_POWER  = 0x30,
};


//#include <Security_decl.h>
//#include <Sockets_decl.h>

//#include <Time_decl.h>














































































//#include <TimeService_decl.h>
//#include <TouchPanel_decl.h>

//#include <nanocrt_decl.h>

#if defined(PLATFORM_ARM)
#pragma check_printf_formats   /* hint to the compiler to check f/s/printf format */
#endif
int hal_vprintf( const char* format, va_list arg );

#if defined(PLATFORM_ARM)
#pragma check_printf_formats   /* hint to the compiler to check f/s/printf format */
#endif
int hal_vfprintf( COM_HANDLE stream, const char* format, va_list arg );

#if defined(PLATFORM_ARM)
#pragma check_printf_formats   /* hint to the compiler to check f/s/printf format */
#endif
int hal_snprintf( char* buffer, size_t len, const char* format, ... );

int hal_vsnprintf( char* buffer, size_t len, const char* format, va_list arg );

#if defined(PLATFORM_ARM)
#define printf     DoNotUse_*printf []
#define sprintf    DoNotUse_*printf []
#define fprintf    DoNotUse_*printf []

#define _printf    DoNotUse_*printf []
#define _sprintf   DoNotUse_*printf []
#define _fprintf   DoNotUse_*printf []

#define snprintf   DoNotUse_*printf []
#define vsnprintf  DoNotUse_*printf []

#define _snprintf  DoNotUse_*printf []
#define _vsnprintf DoNotUse_*printf []

#define strcpy    DoNotUse_*strcpy  []
#define strncpy   DoNotUse_*strcpy  []
#define strlen    DoNotUse_*strlen  []
#define strncmp   DoNotUse_*strncmp  []


int hal_strcpy_s ( char* strDst, size_t sizeInBytes, const char* strSrc );
int hal_strncpy_s( char* strDst, size_t sizeInBytes, const char* strSrc, size_t count );
size_t hal_strlen_s (const char * str);
int hal_strncmp_s( const char* str1, const char* str2, size_t num );

#elif defined(_WIN32)

#define hal_strcpy_s(strDst, sizeInBytes, strSrc) strcpy_s(strDst, sizeInBytes, strSrc)
#define hal_strncpy_s(strDst, sizeInBytes, strSrc, count) strncpy_s(strDst, sizeInBytes, strSrc, count)
#define hal_strlen_s(str) strlen(str)
#define hal_strncmp_s(str1, str2, num) strncmp(str1, str2, num)

#else
!ERROR
#endif


//#include <USART_decl.h>
//#include <USB_decl.h>
//#include <PWM_decl.h>
//#include <Analog_decl.h>
//#include <AnalogOut_decl.h>
//
#include "TargetPAL_BlockStorage.h"

//#include <SD_decl.h>

//#include "..\pal\Diagnostics\Native_Profiler.h"
// native profiler
#define NATIVE_PROFILE_PAL_HEAP()
#define NATIVE_PROFILE_PAL_CRT()

#endif // _NANOPAL_H_
