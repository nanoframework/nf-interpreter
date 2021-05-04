//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_Types.h>
#include <nanoCLR_Types.h>
#include <nanoHAL_Time.h>
#include <target_platform.h>

#include <stm32l4xx_ll_rtc.h>

#include <tx_api.h>

// Returns the current date time from the RTC
uint64_t HAL_Time_CurrentDateTime(bool datePartOnly)
{
    SYSTEMTIME st;
    st.wDay = (unsigned short)LL_RTC_DATE_GetDay(RTC);
    st.wMonth = (unsigned short)LL_RTC_DATE_GetMonth(RTC);
    st.wYear = (unsigned short)(LL_RTC_DATE_GetYear(RTC) + 2000);
    st.wDayOfWeek = (unsigned short)LL_RTC_DATE_GetWeekDay(RTC);

    // zero 'time' fields if date part only is required
    if (datePartOnly)
    {
        st.wMilliseconds = 0;
        st.wSecond = 0;
        st.wMinute = 0;
        st.wHour = 0;
    }
    else
    {
        // full date&time required, fill in 'time' fields too

        st.wMilliseconds = 0;
        st.wSecond = LL_RTC_TIME_GetSecond(RTC);
        st.wMinute = LL_RTC_TIME_GetMinute(RTC);
        st.wHour = LL_RTC_TIME_GetHour(RTC);
    }

    return HAL_Time_ConvertFromSystemTime(&st);
};

void HAL_Time_SetUtcTime(uint64_t utcTime)
{
    SYSTEMTIME systemTime;

    HAL_Time_ToSystemTime(utcTime, &systemTime);

    // date part
    // year time base is 2000
    LL_RTC_DATE_Config(RTC, systemTime.wDayOfWeek, systemTime.wDay, systemTime.wMonth - 1, systemTime.wYear - 2000);

    // time part
    LL_RTC_TIME_Config(RTC, LL_RTC_TIME_FORMAT_AM_OR_24, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
}

bool HAL_Time_TimeSpanToStringEx(const int64_t &ticks, char *&buf, size_t &len)
{
    uint64_t ticksAbs;
    uint64_t rest;

    if (ticks < 0)
    {
        ticksAbs = -ticks;

        CLR_SafeSprintf(buf, len, "-");
    }
    else
    {
        ticksAbs = ticks;
    }

    rest = ticksAbs % (1000 * TIME_CONVERSION__TICKUNITS);
    ticksAbs = ticksAbs / (1000 * TIME_CONVERSION__TICKUNITS); // Convert to seconds.

    if (ticksAbs > TIME_CONVERSION__ONEDAY) // More than one day.
    {
        CLR_SafeSprintf(buf, len, "%d.", (int32_t)(ticksAbs / TIME_CONVERSION__ONEDAY));
        ticksAbs %= TIME_CONVERSION__ONEDAY;
    }

    CLR_SafeSprintf(buf, len, "%02d:", (int32_t)(ticksAbs / TIME_CONVERSION__ONEHOUR));
    ticksAbs %= TIME_CONVERSION__ONEHOUR;
    CLR_SafeSprintf(buf, len, "%02d:", (int32_t)(ticksAbs / TIME_CONVERSION__ONEMINUTE));
    ticksAbs %= TIME_CONVERSION__ONEMINUTE;
    CLR_SafeSprintf(buf, len, "%02d", (int32_t)(ticksAbs / TIME_CONVERSION__ONESECOND));
    ticksAbs %= TIME_CONVERSION__ONESECOND;

    ticksAbs = (uint32_t)rest;
    if (ticksAbs)
    {
        CLR_SafeSprintf(buf, len, ".%07d", (uint32_t)ticksAbs);
    }

    return len != 0;
}

bool DateTimeToString(const int64_t &time, char *&buf, size_t &len)
{
    SYSTEMTIME st;

    HAL_Time_ToSystemTime(time, &st);

    return CLR_SafeSprintf(
        buf,
        len,
        "%4d/%02d/%02d %02d:%02d:%02d.%03d",
        st.wYear,
        st.wMonth,
        st.wDay,
        st.wHour,
        st.wMinute,
        st.wSecond,
        st.wMilliseconds);
}

char *DateTimeToString(const int64_t &time)
{
    static char rgBuffer[128];
    char *szBuffer = rgBuffer;
    size_t iBuffer = ARRAYSIZE(rgBuffer);

    DateTimeToString(time, szBuffer, iBuffer);

    return rgBuffer;
}

const char *HAL_Time_CurrentDateTimeToString()
{
    return DateTimeToString(HAL_Time_CurrentDateTime(false));
}

uint64_t CPU_MillisecondsToTicks(uint64_t ticks)
{
    return ((ticks * (uint64_t)TX_TIMER_TICKS_PER_SECOND) / 1000);
}
