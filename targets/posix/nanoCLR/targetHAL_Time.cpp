//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <thread>

#include <nanoCLR_Types.h>
#include <nanoHAL_Time.h>

void HAL_Time_Sleep_MicroSeconds(unsigned int uSec)
{
    std::this_thread::sleep_for(std::chrono::microseconds(uSec));
}

void HAL_Time_Sleep_MicroSeconds_InterruptEnabled(unsigned int uSec)
{
    std::this_thread::sleep_for(std::chrono::microseconds(uSec));
}

uint64_t HAL_Time_CurrentSysTicks()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    // Convert to .NET ticks (100ns units).
    return (static_cast<uint64_t>(ts.tv_sec) * 10000000ULL) + (static_cast<uint64_t>(ts.tv_nsec) / 100ULL);
}

// Offset between Windows FILETIME epoch (1601-01-01) and Unix epoch (1970-01-01)
// in 100-nanosecond units.
static constexpr uint64_t c_epochOffset100ns = 116444736000000000ULL;

// Converts CMSIS/monotonic sysTicks to .NET ticks (100 nanoseconds).
// For the POSIX host, sysTicks are already 100ns units so we pass through.
uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
{
    return sysTicks;
}

uint64_t HAL_Time_CurrentDateTime(bool datePartOnly)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    uint64_t ticks = c_epochOffset100ns + static_cast<uint64_t>(ts.tv_sec) * 10000000ULL +
                     static_cast<uint64_t>(ts.tv_nsec) / 100ULL;

    if (datePartOnly)
    {
        // Truncate to start of UTC day (86400 s * 10 000 000 ticks/s)
        ticks -= ticks % (864000000000ULL);
    }
    return ticks;
}

bool HAL_Time_TimeSpanToStringEx(const int64_t &ticks, char *&buf, size_t &len)
{
    uint64_t ticksAbs;

    if (ticks < 0)
    {
        ticksAbs = static_cast<uint64_t>(-ticks);
        CLR_SafeSprintf(buf, len, "-");
    }
    else
    {
        ticksAbs = static_cast<uint64_t>(ticks);
    }

    uint64_t rest = ticksAbs % (1000ULL * TIME_CONVERSION__TICKUNITS);
    ticksAbs /= (1000ULL * TIME_CONVERSION__TICKUNITS); // seconds

    if (ticksAbs > (uint64_t)TIME_CONVERSION__ONEDAY)
    {
        CLR_SafeSprintf(buf, len, "%d.", (int32_t)(ticksAbs / TIME_CONVERSION__ONEDAY));
        ticksAbs %= TIME_CONVERSION__ONEDAY;
    }

    CLR_SafeSprintf(buf, len, "%02d:", (int32_t)(ticksAbs / TIME_CONVERSION__ONEHOUR));
    ticksAbs %= TIME_CONVERSION__ONEHOUR;
    CLR_SafeSprintf(buf, len, "%02d:", (int32_t)(ticksAbs / TIME_CONVERSION__ONEMINUTE));
    ticksAbs %= TIME_CONVERSION__ONEMINUTE;
    CLR_SafeSprintf(buf, len, "%02d", (int32_t)(ticksAbs / TIME_CONVERSION__ONESECOND));

    if (rest != 0)
    {
        CLR_SafeSprintf(buf, len, ".%07d", (uint32_t)rest);
    }

    return len != 0;
}

const char *HAL_Time_CurrentDateTimeToString()
{
    // thread_local: each thread has its own buffer so concurrent callers don't corrupt each other.
    thread_local char buf[128];
    char *p = buf;
    size_t remaining = sizeof(buf);
    uint64_t ticks = HAL_Time_CurrentDateTime(false);

    // Convert back to unix time
    uint64_t unixTicks = ticks - c_epochOffset100ns;
    time_t sec = static_cast<time_t>(unixTicks / 10000000ULL);
    struct tm tmBuf{};
    struct tm *t = gmtime_r(&sec, &tmBuf); // reentrant: no shared static internal buffer
    if (t)
    {
        CLR_SafeSprintf(p, remaining, "%04d/%02d/%02d %02d:%02d:%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                        t->tm_hour, t->tm_min, t->tm_sec);
    }
    return buf;
}

// ------------------------------------------------------------------
// PAL Time — POSIX stub (no platform timer hardware to initialise).
// ------------------------------------------------------------------

extern "C" HRESULT Time_Initialize()
{
    return S_OK;
}

extern "C" HRESULT Time_Uninitialize()
{
    return S_OK;
}

