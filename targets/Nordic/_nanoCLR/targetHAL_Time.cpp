//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_Types.h>
#include <nanoCLR_Types.h>
#include <nanoHAL_Time.h>
#include <target_platform.h>
#include <zephyr.h>
#include <date_time.h>

/*
 * Amazingly, the 'new lib' doesn't convert longlong vals using %lld
 * So, I need this routine.
 */
// char *my_longlong2str(uint64_t *num_ptr)
// {
//     static char longnum[64];
//     uint64_t num = *num_ptr;
//     memset(longnum, sizeof(longnum), 0);
//     char *p = &longnum[sizeof(longnum) - 2];
//     do
//     {
//         *p-- = num % 10 + '0';
//         num = num / 10;
//     } while (num != 0);
//     p++;
//     return p;
// }
// char *my_time_to_str(uint64_t *time_val, char *sz, int len)
// {
//     struct tm st_tim;
//     time_t corrected = *time_val / 1000;

//     sz[0] = '\0';
//     if (corrected < 100000)
//         return sz;

//     const struct tm *p = gmtime_r(&corrected, &st_tim);
//     if (p != 0)
//     {
//         sprintf(
//             sz,
//             "%d/%d/%d  %2d:%02d:%02d",
//             p->tm_mon + 1,
//             p->tm_mday,
//             p->tm_year + 1900,
//             p->tm_hour,
//             p->tm_min,
//             p->tm_sec);
//     }
//     return sz;
// }

// Returns the current date time from the system tick or from the RTC if it's available (this depends on the respective
// configuration option)
uint64_t HAL_Time_CurrentDateTime(bool datePartOnly)
{
    // printk ("HAL_Time_CurrentDateTime++ %d\n", datePartOnly);
#if (HAL_USE_RTC == TRUE)
    uint64_t epoc; // epoch time, seconds since 1/1/1970
    date_time_now((int64_t *)&epoc);

    // msec between 1/1/1601 and 1/1/1970 = 11644473600000
    uint64_t nf_epoc = epoc + (11644473600000);
    nf_epoc = nf_epoc * 10000; // Convert to 100nS
    return nf_epoc;

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
    printk("HAL_Time_SetUtcTime++  NEED TO TEST\n");

    HAL_Time_ToSystemTime(utcTime, &systemTime);

#if (HAL_USE_RTC == TRUE)
    struct tm st;
    st.tm_hour = systemTime.wHour;
    st.tm_sec = systemTime.wSecond; // seconds after the minute - [0, 60] including leap second
    st.tm_min = systemTime.wMinute; // minutes after the hour - [0, 59]
    st.tm_hour = systemTime.wHour;  // hours since midnight - [0, 23]
    st.tm_mday = systemTime.wDay;   // day of the month - [1, 31]
    st.tm_mon = systemTime.wMonth;  // months since January - [0, 11]
    st.tm_year = systemTime.wYear;  // years since 1900
    st.tm_wday = 0;                 // Unused.
    st.tm_yday = 0;                 // Unused.
    st.tm_isdst = 0;

    date_time_set(&st);

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

bool DateTimeToString(const uint64_t &time, char *&buf, size_t &len)
{
    SYSTEMTIME st;
    printk("DateTimeToString++\n");

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

char *DateTimeToString(const uint64_t &time)
{
    static char rgBuffer[128];
    char *szBuffer = rgBuffer;
    size_t iBuffer = ARRAYSIZE(rgBuffer);

    DateTimeToString(time, szBuffer, iBuffer);

    return rgBuffer;
}

const char *HAL_Time_CurrentDateTimeToString()
{
    //printk("HAL_Time_CurrentDateTimeToString++\n");
    return DateTimeToString(HAL_Time_CurrentDateTime(false));
}

uint64_t CPU_MillisecondsToTicks(uint64_t ticks)
{
    return 1;
}
