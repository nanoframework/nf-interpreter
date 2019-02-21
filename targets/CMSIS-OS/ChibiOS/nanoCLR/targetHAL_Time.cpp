//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_Types.h>
#include <nanoCLR_Types.h>
#include <nanoHAL_Time.h>
#include <target_platform.h>
#include <hal.h>
#include <ch.h>

// Returns the current date time from the system tick or from the RTC if it's available (this depends on the respective configuration option)
uint64_t  HAL_Time_CurrentDateTime(bool datePartOnly)
{
  #if defined(HAL_USE_RTC)

    // use RTC to get date time
    SYSTEMTIME st; 
    RTCDateTime _dateTime;

    rtcGetTime(&RTCD1, &_dateTime);

    st.wDay = (unsigned short) _dateTime.day;
    st.wMonth = (unsigned short) _dateTime.month;
    st.wYear = (unsigned short) (_dateTime.year + 1980);    // ChibiOS is counting years since 1980
    st.wDayOfWeek = (unsigned short) _dateTime.dayofweek;

    // zero 'time' fields if date part only is required
    if(datePartOnly)
    {
        st.wMilliseconds = 0;
        st.wSecond = 0;
        st.wMinute = 0;
        st.wHour = 0;
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
        st.wHour = (unsigned short) (_dateTime.millisecond % 24);
    }

    return HAL_Time_ConvertFromSystemTime( &st );

  #else
	if (datePartOnly)
	{
		SYSTEMTIME st;
		HAL_Time_ToSystemTime(HAL_Time_CurrentTime(), &st);

		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		st.wMilliseconds = 0;

		return HAL_Time_ConvertFromSystemTime(&st);
	}
	else
    {
        return HAL_Time_CurrentTime();
    }
  #endif
};

void HAL_Time_SetUtcTime(uint64_t utcTime)
{
    SYSTEMTIME systemTime;

    HAL_Time_ToSystemTime(utcTime, &systemTime);

  #if defined(HAL_USE_RTC)

    // set RTC
    RTCDateTime newTime;

    newTime.year = systemTime.wYear - 1980;  // ChibiOS time base is 1980-01-01
    newTime.month = systemTime.wMonth;
    newTime.day = systemTime.wDay;
    newTime.dayofweek = systemTime.wDayOfWeek;
    newTime.millisecond = ((((uint32_t)systemTime.wHour * 3600) + 
                            ((uint32_t)systemTime.wMinute * 60) + 
                            (uint32_t)systemTime.wSecond) * 1000);

    // set RTC time
    rtcSetTime(&RTCD1, &newTime);

  #else
    // TODO FIXME
    // need to add implementation when RTC is not being used
    // can't mess with the systicks because the scheduling can fail
  #endif
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

bool DateTimeToString(const uint64_t& time, char*& buf, size_t& len )
{
    SYSTEMTIME st;

    HAL_Time_ToSystemTime( time, &st );

    return CLR_SafeSprintf(buf, len, "%4d/%02d/%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
}

char* DateTimeToString(const uint64_t& time)
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
    // this is a rewrite of ChibiOS TIME_MS2I(n) macro because it will overflow if doing the math using uint32_t
    return (((ticks * (uint64_t)CH_CFG_ST_FREQUENCY) + 999) / (uint64_t)1000);
}
