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

// Returns the current date time from the RTC 
uint64_t  HAL_Time_CurrentDateTime(bool datePartOnly)
{
    SYSTEMTIME st; 
 
    struct timeval tv;

    gettimeofday(&tv, NULL);

    // Convert from Unix time(year since 1900) to SYSTEMTIME(Years since 1601)
    int64_t time = ((int64_t)tv.tv_sec * (int64_t)TIME_CONVERSION__TO_SECONDS) + TIME_UNIX_EPOCH_AS_TICKS;

    HAL_Time_ToSystemTime(time, &st );

    // zero 'time' fields if date part only is required
    if(datePartOnly)
    {
        st.wMilliseconds = 0;
        st.wSecond = 0;
        st.wMinute = 0;
        st.wHour = 0;
    }

    return HAL_Time_ConvertFromSystemTime( &st );
};

void HAL_Time_SetUtcTime(uint64_t utcTime)
{
    SYSTEMTIME systemTime;

    HAL_Time_ToSystemTime(utcTime, &systemTime);

    struct tm newTime;

    newTime.tm_year = systemTime.wYear - 1900;      // years since 1900
    newTime.tm_mon = systemTime.wMonth - 1;         // months since January 0-11
    newTime.tm_mday = systemTime.wDay;              // day of the month 1-31
    newTime.tm_wday = systemTime.wDayOfWeek;        // days since Sunday 0-6
    newTime.tm_hour = (uint32_t)systemTime.wHour;   // hours since midnight 0-23
    newTime.tm_min = (uint32_t)systemTime.wMinute;  // minutes after the hour 0-59
    newTime.tm_sec = (uint32_t)systemTime.wSecond;  // seconds after the minute	 0-59

    time_t t = mktime(&newTime);
    struct timeval now = { .tv_sec = t, .tv_usec = 0 };
    settimeofday(&now, NULL);
}

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
