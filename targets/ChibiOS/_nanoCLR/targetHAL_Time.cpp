//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_Types.h>
#include <nanoCLR_Types.h>
#include <nanoHAL_Time.h>
#include <target_platform.h>
#include <hal.h>
#include <ch.h>

#if (HAL_USE_RTC == FALSE)
// Software UTC offset used when no hardware RTC is available.
// Updated by HAL_Time_SetUtcTime() (e.g., from SNTP) and applied on reads.
static int64_t s_utcOffsetTicks = 0;
static bool s_softClockInitialized = false;

static uint16_t ParseBuildMonth(const char *month)
{
    if (month[0] == 'J' && month[1] == 'a' && month[2] == 'n')
        return 1;
    if (month[0] == 'F' && month[1] == 'e' && month[2] == 'b')
        return 2;
    if (month[0] == 'M' && month[1] == 'a' && month[2] == 'r')
        return 3;
    if (month[0] == 'A' && month[1] == 'p' && month[2] == 'r')
        return 4;
    if (month[0] == 'M' && month[1] == 'a' && month[2] == 'y')
        return 5;
    if (month[0] == 'J' && month[1] == 'u' && month[2] == 'n')
        return 6;
    if (month[0] == 'J' && month[1] == 'u' && month[2] == 'l')
        return 7;
    if (month[0] == 'A' && month[1] == 'u' && month[2] == 'g')
        return 8;
    if (month[0] == 'S' && month[1] == 'e' && month[2] == 'p')
        return 9;
    if (month[0] == 'O' && month[1] == 'c' && month[2] == 't')
        return 10;
    if (month[0] == 'N' && month[1] == 'o' && month[2] == 'v')
        return 11;
    if (month[0] == 'D' && month[1] == 'e' && month[2] == 'c')
        return 12;

    return 1;
}

static uint16_t Parse2Digits(const char *value)
{
    return (uint16_t)(((value[0] - '0') * 10) + (value[1] - '0'));
}

static uint16_t ParseDay(const char *value)
{
    if (value[0] == ' ')
    {
        return (uint16_t)(value[1] - '0');
    }

    return Parse2Digits(value);
}

static uint16_t Parse4Digits(const char *value)
{
    return (uint16_t)(
        ((value[0] - '0') * 1000) + ((value[1] - '0') * 100) + ((value[2] - '0') * 10) + (value[3] - '0'));
}

static uint64_t GetBuildTimeAsTicks()
{
    SYSTEMTIME st;
    st.wYear = 0;
    st.wMonth = 0;
    st.wDayOfWeek = 0;
    st.wDay = 0;
    st.wHour = 0;
    st.wMinute = 0;
    st.wSecond = 0;
    st.wMilliseconds = 0;

    // __DATE__ format: "Mmm dd yyyy"
    // __TIME__ format: "hh:mm:ss"
    st.wYear = Parse4Digits(__DATE__ + 7);
    st.wMonth = ParseBuildMonth(__DATE__);
    st.wDay = ParseDay(__DATE__ + 4);
    st.wHour = Parse2Digits(__TIME__ + 0);
    st.wMinute = Parse2Digits(__TIME__ + 3);
    st.wSecond = Parse2Digits(__TIME__ + 6);
    st.wMilliseconds = 0;

    return HAL_Time_ConvertFromSystemTime(&st);
}

static void InitializeSoftClockIfNeeded()
{
    if (!s_softClockInitialized)
    {
        // Start from firmware build timestamp so DateTime is sane before SNTP sync.
        s_utcOffsetTicks = (int64_t)GetBuildTimeAsTicks() - (int64_t)HAL_Time_CurrentTime();
        s_softClockInitialized = true;
    }
}
#endif

// Returns the current date time from the system tick or from the RTC if it's available (this depends on the respective configuration option)
uint64_t  HAL_Time_CurrentDateTime(bool datePartOnly)
{
  #if (HAL_USE_RTC == TRUE)

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

        chSysLock();
        InitializeSoftClockIfNeeded();

        uint64_t currentTime;
        currentTime = (uint64_t)((int64_t)HAL_Time_CurrentTime() + s_utcOffsetTicks);
        chSysUnlock();

	if (datePartOnly)
	{
		SYSTEMTIME st;
		HAL_Time_ToSystemTime(currentTime, &st);

		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		st.wMilliseconds = 0;

		return HAL_Time_ConvertFromSystemTime(&st);
	}
	else
    {
        return currentTime;
    }

  #endif
};

void HAL_Time_SetUtcTime(uint64_t utcTime)
{
    SYSTEMTIME systemTime;

    HAL_Time_ToSystemTime(utcTime, &systemTime);

  #if (HAL_USE_RTC == TRUE)

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

        // No RTC available: keep UTC as an offset over monotonic HAL ticks.
        // This allows SNTP to set a usable wall clock without touching scheduler ticks.
        chSysLock();
        s_utcOffsetTicks = (int64_t)utcTime - (int64_t)HAL_Time_CurrentTime();
        chSysUnlock();

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
