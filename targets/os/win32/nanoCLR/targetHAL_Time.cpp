//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

UINT64 FileTimeToUint64(const FILETIME ft)
{
    ULARGE_INTEGER x;
    x.LowPart = ft.dwLowDateTime;
    x.HighPart = ft.dwHighDateTime;
    return x.QuadPart;
}

void HAL_Time_Sleep_MicroSeconds(unsigned int uSec)
{
    // UNDONE: FIXME: return EmulatorNative::GetITimeDriver()->Sleep_MicroSeconds( uSec );
}

void HAL_Time_Sleep_MicroSeconds_InterruptEnabled(unsigned int uSec)
{
    // UNDONE: FIXME: return EmulatorNative::GetITimeDriver()->Sleep_MicroSecondsInterruptsEnabled( uSec );
}

unsigned int HAL_Time_CurrentSysTicks()
{
    FILETIME ft = {0};
    GetSystemTimePreciseAsFileTime(&ft);

    auto sysTicks = FileTimeToUint64(ft) / CPU_TicksPerSecond() / 1000;

    return (uint32_t)sysTicks;
}

// Converts CMSIS sysTicks to .NET ticks (100 nanoseconds)
uint64_t HAL_Time_SysTicksToTime(unsigned int sysTicks)
{
    _ASSERTE(sysTicks <= 0x7FFFFFFF);

    return CPU_MicrosecondsToTicks(sysTicks * CPU_TicksPerSecond());
}

uint64_t HAL_Time_CurrentTime()
{
    return HAL_Time_SysTicksToTime(HAL_Time_CurrentSysTicks());
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

void HAL_Time_GetDriftParameters(signed int *a, signed int *b, signed __int64 *c)
{
    *a = 1;
    *b = 1;
    *c = 0;
}

unsigned int CPU_SystemClock()
{
    return 0; // UNDONE: FIXME: EmulatorNative::GetITimeDriver()->SystemClock;
}
