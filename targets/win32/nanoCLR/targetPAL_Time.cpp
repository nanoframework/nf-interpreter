//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"
#include <Win32TimerQueue.h>

void TimerCallback()
{
    GLOBAL_LOCK(irq);
    HAL_COMPLETION::DequeueAndExec();
}

void Time_SetCompare(UINT64 CompareValue)
{
    static std::unique_ptr<Microsoft::Win32::Timer> pCompletionsTimer;

    // convert to milliseconds for OS timer
    auto compareMs = CompareValue / (CPU_TicksPerSecond() * (uint64_t)1000);
    ASSERT(compareMs < UINT32_MAX);

    if (compareMs == 0)
    {
        TimerCallback();
    }
    else
    {
        pCompletionsTimer = std::make_unique<Microsoft::Win32::Timer>((UINT32)compareMs, TimerCallback);
    }
}
