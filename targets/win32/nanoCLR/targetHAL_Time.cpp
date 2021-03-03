//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

void HAL_Time_Sleep_MicroSeconds(unsigned int uSec)
{
    // UNDONE: FIXME: return EmulatorNative::GetITimeDriver()->Sleep_MicroSeconds( uSec );
}

void HAL_Time_Sleep_MicroSeconds_InterruptEnabled(unsigned int uSec)
{
    // UNDONE: FIXME: return EmulatorNative::GetITimeDriver()->Sleep_MicroSecondsInterruptsEnabled( uSec );
}

uint64_t HAL_Time_CurrentSysTicks()
{
    FILETIME ft = {0};
    GetSystemTimePreciseAsFileTime(&ft);
    SYSTEMTIME lpSystemTime;
    bool t = FileTimeToSystemTime(&ft, &lpSystemTime);
    return HAL_Time_ConvertFromSystemTime(&lpSystemTime);
}

// Converts CMSIS sysTicks to .NET ticks (100 nanoseconds)
uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
{
    _ASSERTE(sysTicks < 0x8000000000000000);

    return sysTicks;
}

uint64_t HAL_Time_CurrentDateTime(bool datePartOnly)
{
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
        // on Windows we'll just return what is the current time from HAL
        return HAL_Time_CurrentTime();
    }
}

bool HAL_Time_TimeSpanToStringEx(const signed __int64 &ticks, char *&buf, size_t &len)
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
    return DateTimeToString(HAL_Time_CurrentDateTime(false));
}
