//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoWeak.h>
#include "FreeRTOS.h"
#include "task.h"
#include "time.h"
#include <sys/time.h>

#if configUSE_16_BIT_TICKS == 0
#define TOPVALUE ((uint64_t)0x100000000ull)
#elif configUSE_16_BIT_TICKS == 1
#define TOPVALUE ((uint64_t)0x10000ull)
#else
#error "Tick resolution not supported."
#endif

uint64_t HAL_Time_CurrentSysTicks(void)
{

    static uint64_t extendedCounter = 0;
    static uint32_t prevKernelTick = 0;
    uint32_t kernelTick = 0;

    // Check if we have overflow during this call
    // It is assumed that this function gets called twice within its overflow range
    // (e.g. for 32-bit counter this is 49.71 days)
    GLOBAL_LOCK();
    {
        kernelTick = xTaskGetTickCountFromISR();

        if (prevKernelTick > kernelTick)
        {
            extendedCounter += TOPVALUE;
        }
        prevKernelTick = kernelTick;
    }
    GLOBAL_UNLOCK();

    return extendedCounter + kernelTick;
}

// Converts FreeRTOS Tickcount to .NET ticks (100 nanoseconds)
uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
{
    return (((int64_t)sysTicks * (int64_t)1000000 + (int64_t)configTICK_RATE_HZ - 1) / (int64_t)configTICK_RATE_HZ) *
           10;
}

// because HAL_Time_SysTicksToTime needs to be called from C we need a proxy to allow it to be called from 'C' code
extern "C"
{

    uint64_t HAL_Time_SysTicksToTime_C(uint64_t sysTicks)
    {
        return HAL_Time_SysTicksToTime(sysTicks);
    }
}
