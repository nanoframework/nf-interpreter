//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_Types.h>
#include <nanoHAL_Time.h>
#include <target_platform.h>
#include <hal.h>
#include <ch.h>

// Utilities to output date time related strings
bool Utility_SafeSprintfV( char*& szBuffer, size_t& iBuffer, char* format, va_list arg );
bool SafeSprintf( char*& szBuffer, size_t& iBuffer,  char* format, ... );
bool SafeSprintfV( char*& szBuffer, size_t& iBuffer, char* format, va_list arg );
bool Utility_SafeSprintf( char*& szBuffer, size_t& iBuffer, char* format, ... );


// Converts CMSIS sysTicks to .NET ticks (100 nanoseconds)
signed __int64 HAL_Time_SysTicksToTime(unsigned int sysTicks) {
    
    // this is a rewrite of ChibiOS ST2US(n) macro because it will overflow if doing the math using uint32_t
    
    // convert to microseconds from CMSIS SysTicks
    int64_t microsecondsFromSysTicks = (((sysTicks) * 1000000ULL + (int64_t)CH_CFG_ST_FREQUENCY - 1ULL) / (int64_t)CH_CFG_ST_FREQUENCY);

    // need to convert from microseconds to 100 nanoseconds
    return  microsecondsFromSysTicks * 10;
}

// Returns the current date time from the system tick or from the RTC if it's available (this depends on the respective configuration option)
signed __int64  HAL_Time_CurrentDateTime(bool datePartOnly)
{
#if defined(HAL_USE_RTC)

    // use RTC to get date time
    SYSTEMTIME st; 
    RTCDateTime _dateTime;

    rtcGetTime(&RTCD1, &_dateTime);

    st.wDay = (unsigned short) _dateTime.day;
    st.wMonth = (unsigned short) _dateTime.month;
    st.wYear = (unsigned short) _dateTime.year;
    st.wDayOfWeek = (unsigned short) _dateTime.dayofweek;

    // zero 'time' fields if date part only is required
    if(datePartOnly)
    {
        st.wMilliseconds = 0;
        st.wSecond = 0;
        st.wMinute = 0;
        st.wMinute = 0;
    }
    else
    {
        // full date&time required, fill in 'time' fields too
        
        st.wMilliseconds =(unsigned short) (_dateTime.millisecond % 1000);
        _dateTime.millisecond /= 1000;
        st.wSecond = (unsigned short) (_dateTime.millisecond % 60);
        _dateTime.millisecond /= 60;
        st.wMinute = (unsigned short) (_dateTime.millisecond % 60);
        _dateTime.millisecond /= 60;
        st.wMinute = (unsigned short) (_dateTime.millisecond % 24);
    }

    return HAL_Time_ConvertFromSystemTime( &st );

#else

    // use system ticks
    return HAL_Time_SysTicksToTime( HAL_Time_CurrentSysTicks() );

#endif
};

bool SafeSprintf( char*& szBuffer, size_t& iBuffer,  char* format, ... )
{
    va_list arg;
    bool    fRes;

    va_start( arg, format );

    fRes = Utility_SafeSprintfV( szBuffer, iBuffer, format, arg );

    va_end( arg );

    return fRes;
}

bool SafeSprintfV( char*& szBuffer, size_t& iBuffer, char* format, va_list arg )
{
    int  chars = hal_vsnprintf( szBuffer, iBuffer, format, arg );
    bool fRes  = (chars >= 0);

    if(fRes == false) chars = 0;

    szBuffer += chars; szBuffer[0] = 0;
    iBuffer  -= chars;

    return fRes;
}

bool Utility_SafeSprintfV( char*& szBuffer, size_t& iBuffer, char* format, va_list arg )
{
    return SafeSprintfV(szBuffer, iBuffer, format, arg);
}

bool Utility_SafeSprintf( char*& szBuffer, size_t& iBuffer, char* format, ... )
{
    va_list arg;
    bool   fRes;

    va_start( arg, format );

    fRes = SafeSprintfV(szBuffer, iBuffer, format, arg);

    va_end( arg );

    return fRes;
}

bool HAL_Time_TimeSpanToStringEx( const int64_t& ticks, char*& buf, size_t& len )
{
    uint64_t ticksAbs;
    uint64_t rest;

    if(ticks < 0)
    {
        ticksAbs = -ticks;

        Utility_SafeSprintf( buf, len, "-" );
    }
    else
    {
        ticksAbs = ticks;
    }

    rest      = ticksAbs % ( 1000 * TIME_CONVERSION__TICKUNITS);
    ticksAbs  = ticksAbs / ( 1000 * TIME_CONVERSION__TICKUNITS);  // Convert to seconds.

    if(ticksAbs > TIME_CONVERSION__ONEDAY) // More than one day.
    {
        Utility_SafeSprintf( buf, len, "%d.", (int32_t)(ticksAbs / TIME_CONVERSION__ONEDAY) ); ticksAbs %= TIME_CONVERSION__ONEDAY;
    }

    SafeSprintf( buf, len, "%02d:", (int32_t)(ticksAbs / TIME_CONVERSION__ONEHOUR)  ); ticksAbs %= TIME_CONVERSION__ONEHOUR  ;
    SafeSprintf( buf, len, "%02d:", (int32_t)(ticksAbs / TIME_CONVERSION__ONEMINUTE)); ticksAbs %= TIME_CONVERSION__ONEMINUTE;
    SafeSprintf( buf, len, "%02d" , (int32_t)(ticksAbs / TIME_CONVERSION__ONESECOND)); ticksAbs %= TIME_CONVERSION__ONESECOND;

    ticksAbs = (uint32_t)rest;
    if(ticksAbs)
    {
        SafeSprintf( buf, len, ".%07d", (uint32_t)ticksAbs );
    }

    return len != 0;
}

unsigned __int64 CPU_MiliSecondsToSysTicks(unsigned __int64 miliSeconds)
{
    return MS2ST(miliSeconds);
}
