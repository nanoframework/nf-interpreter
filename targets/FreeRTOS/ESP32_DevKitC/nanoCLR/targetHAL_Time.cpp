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

// Converts Tickcount to .NET ticks (100 nanoseconds)
int64_t HAL_Time_SysTicksToTime(unsigned int sysTicks) {
    
    // convert to microseconds from FreeRTOS Tickcount
    int64_t microsecondsFromSysTicks = ((( xTaskGetTickCount() ) * 1000000ULL + (int64_t)configTICK_RATE_HZ - 1ULL) / (int64_t)configTICK_RATE_HZ);

    // need to convert from microseconds to 100 nanoseconds
    return  microsecondsFromSysTicks * 10;
}

// Returns the current date time from the system tick or from the RTC if it's available (this depends on the respective configuration option)
int64_t  HAL_Time_CurrentDateTime(bool datePartOnly)
{
#if defined(HAL_USE_RTC)
    SYSTEMTIME st; 
    struct timeval now;
    gettimeofday(&now, NULL);
    
    time_t mtime = now.tv_sec;
    struct tm mtm;
    gmtime_r(&mtime, &mtm);

    st.wDay = (unsigned short) mtm.tm_mday;         // day of the month 1-31
    st.wMonth = (unsigned short) mtm.tm_mon + 1;    // months since January 0-11
    st.wYear = (unsigned short) mtm.tm_year + 1900; // years since 1900
    st.wDayOfWeek = (unsigned short) mtm.tm_wday;   // days since Sunday 0-6

    // zero 'time' fields if date part only is required
    if(datePartOnly)
    {
        st.wMilliseconds = 0;
        st.wSecond = 0;
        st.wMinute = 0;
        st.wHour   = 0;
    }
    else
    {
        // full date&time required, fill in 'time' fields too
        st.wMilliseconds = now.tv_usec / 1000;
        st.wSecond = mtm.tm_sec;
        st.wMinute = mtm.tm_min;
        st.wHour   = mtm.tm_hour;
    }

    return HAL_Time_ConvertFromSystemTime( &st );

#else

     // use system ticks
     return HAL_Time_SysTicksToTime( HAL_Time_CurrentSysTicks() );

#endif
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


uint64_t CPU_MillisecondsToTicks(uint64_t ticks)
{
    return ESP32_TICKS_PER_MS(ticks);
}
