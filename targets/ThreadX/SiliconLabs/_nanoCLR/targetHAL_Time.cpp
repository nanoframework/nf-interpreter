//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_Types.h>
#include <nanoCLR_Types.h>
#include <nanoHAL_Time.h>
#include <target_platform.h>

#include <tx_api.h>
#include <sl_sleeptimer.h>

// Returns the current date time from the RTC
uint64_t HAL_Time_CurrentDateTime(bool datePartOnly)
{
    (void)datePartOnly;

    // return 0;
#if (HAL_USE_RTC == TRUE)

    SYSTEMTIME st;
    sl_sleeptimer_date_t date;

    // use RTCC to get date time
    sl_sleeptimer_convert_time_to_date_64(sl_sleeptimer_get_time_64(), 0, &date);

    st.wDay = (unsigned short)date.month_day;
    st.wMonth = (unsigned short)date.month;
    st.wYear = (unsigned short)date.year + 1900; // Gecko is counting years since 1900
    st.wDayOfWeek = (unsigned short)date.day_of_week;

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
        st.wSecond = (unsigned short)(date.sec);
        st.wMinute = (unsigned short)(date.min);
        st.wHour = (unsigned short)(date.hour);
    }

    return HAL_Time_ConvertFromSystemTime(&st);

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
    (void)utcTime;

    SYSTEMTIME systemTime;
    sl_sleeptimer_date_t date;

    HAL_Time_ToSystemTime(utcTime, &systemTime);

#if (HAL_USE_RTC == TRUE)

    sl_sleeptimer_build_datetime_64(
        &date,
        systemTime.wYear - 1900,
        systemTime.wMonth,
        systemTime.wDay,
        systemTime.wHour,
        systemTime.wMinute,
        systemTime.wSecond,
        0);

    // set RTC time
    sl_sleeptimer_set_datetime(&date);

#else

    // TODO FIXME
    // need to add implementation when RTC is not being used
    // can't mess with the systicks because the scheduling can fail

#endif
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
