//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_Types.h>
#include <nanoCLR_Types.h>
#include <nanoHAL_Time.h>
#include <target_platform.h>
#include <Esp32_os.h>

#define ESP32_TICKS_PER_MS(x)            ( ((uint64_t)x * configTICK_RATE_HZ) / 1000)

// Converts Tickcount to .NET ticks (100 nanoseconds)
signed __int64 HAL_Time_SysTicksToTime(unsigned int sysTicks) {
    
    // convert to microseconds from FreeRtyos Tickcount
    int64_t microsecondsFromSysTicks = ((( xTaskGetTickCount() ) * 1000000ULL + (int64_t)configTICK_RATE_HZ - 1ULL) / (int64_t)configTICK_RATE_HZ);

    // need to convert from microseconds to 100 nanoseconds
    return  microsecondsFromSysTicks * 10;
}

// Returns the current date time from the system tick or from the RTC if it's available (this depends on the respective configuration option)
signed __int64  HAL_Time_CurrentDateTime(bool datePartOnly)
{
// #if defined(HAL_USE_RTC)

//     // use RTC to get date time
//     SYSTEMTIME st; 
//     RTCDateTime _dateTime;

//     rtcGetTime(&RTCD1, &_dateTime);

//     st.wDay = (unsigned short) _dateTime.day;
//     st.wMonth = (unsigned short) _dateTime.month;
//     st.wYear = (unsigned short) _dateTime.year;
//     st.wDayOfWeek = (unsigned short) _dateTime.dayofweek;

//     // zero 'time' fields if date part only is required
//     if(datePartOnly)
//     {
//         st.wMilliseconds = 0;
//         st.wSecond = 0;
//         st.wMinute = 0;
//         st.wMinute = 0;
//     }
//     else
//     {
//         // full date&time required, fill in 'time' fields too
        
//         st.wMilliseconds =(unsigned short) (_dateTime.millisecond % 1000);
//         _dateTime.millisecond /= 1000;
//         st.wSecond = (unsigned short) (_dateTime.millisecond % 60);
//         _dateTime.millisecond /= 60;
//         st.wMinute = (unsigned short) (_dateTime.millisecond % 60);
//         _dateTime.millisecond /= 60;
//         st.wMinute = (unsigned short) (_dateTime.millisecond % 24);
//     }

//     return HAL_Time_ConvertFromSystemTime( &st );

// #else

    // use system ticks
    return HAL_Time_SysTicksToTime( HAL_Time_CurrentSysTicks() );

// #endif
};

bool HAL_Time_TimeSpanToStringEx( const int64_t& ticks, char*& buf, size_t& len )
{
    uint64_t ticksAbs;
    uint64_t rest;

    if(ticks < 0)
    {
        ticksAbs = -ticks;

        CLR_SafeSprintf( buf, len, "-" );
    }
    else
    {
        ticksAbs = ticks;
    }

    rest      = ticksAbs % ( 1000 * TIME_CONVERSION__TICKUNITS);
    ticksAbs  = ticksAbs / ( 1000 * TIME_CONVERSION__TICKUNITS);  // Convert to seconds.

    if(ticksAbs > TIME_CONVERSION__ONEDAY) // More than one day.
    {
        CLR_SafeSprintf( buf, len, "%d.", (int32_t)(ticksAbs / TIME_CONVERSION__ONEDAY) ); ticksAbs %= TIME_CONVERSION__ONEDAY;
    }

    CLR_SafeSprintf( buf, len, "%02d:", (int32_t)(ticksAbs / TIME_CONVERSION__ONEHOUR)  ); ticksAbs %= TIME_CONVERSION__ONEHOUR  ;
    CLR_SafeSprintf( buf, len, "%02d:", (int32_t)(ticksAbs / TIME_CONVERSION__ONEMINUTE)); ticksAbs %= TIME_CONVERSION__ONEMINUTE;
    CLR_SafeSprintf( buf, len, "%02d" , (int32_t)(ticksAbs / TIME_CONVERSION__ONESECOND)); ticksAbs %= TIME_CONVERSION__ONESECOND;

    ticksAbs = (uint32_t)rest;
    if(ticksAbs)
    {
        CLR_SafeSprintf( buf, len, ".%07d", (uint32_t)ticksAbs );
    }

    return len != 0;
}

bool DateTimeToString(const int64_t& time, char*& buf, size_t& len )
{
    SYSTEMTIME st;

    HAL_Time_ToSystemTime( time, &st );

    return CLR_SafeSprintf(buf, len, "%4d/%02d/%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
}

char* DateTimeToString(const int64_t& time)
{
    static char rgBuffer[128];
    char*  szBuffer =           rgBuffer;
    size_t iBuffer  = ARRAYSIZE(rgBuffer);

    DateTimeToString( time, szBuffer, iBuffer );

    return rgBuffer;
}

const char* HAL_Time_CurrentDateTimeToString()
{
    return DateTimeToString(HAL_Time_CurrentDateTime(false));
}

unsigned __int64 CPU_MiliSecondsToSysTicks(unsigned __int64 miliSeconds)
{
    return  ESP32_TICKS_PER_MS(miliSeconds);
}
