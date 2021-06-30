//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

#if (CH_CFG_ST_RESOLUTION == 64)
	#error "Not supported"
#elif CH_CFG_ST_RESOLUTION == 32
	#define TOPVALUE ((uint64_t)0x100000000ull)
#elif CH_CFG_ST_RESOLUTION == 16
	#define TOPVALUE ((uint64_t)0x10000ull)
#endif

uint64_t HAL_Time_ExtendedCurrentSysTicks(void) {

	static uint64_t extendedCounter = 0;
	static systime_t prevKernelTick = 0;
	systime_t kernelTick = 0;

	// Check if we have overflow during this call
	// It is assumed that this function gets called twice within its overflow range
	// (e.g. for 32-bit counter this is 49.71 days)
	GLOBAL_LOCK();
	{
		kernelTick = chVTGetSystemTimeX();

		if (prevKernelTick > kernelTick)
		{
			extendedCounter += TOPVALUE;
		}
		prevKernelTick = kernelTick;
	}
	GLOBAL_UNLOCK();

	return extendedCounter + kernelTick;
}

// Converts CMSIS sysTicks to .NET ticks (100 nanoseconds)
uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
{
    // convert to microseconds from CMSIS SysTicks
    // this is a rewrite of ChibiOS TIME_I2US(n) macro because it will overflow if doing the math using uint32_t
    // need to convert from microseconds to 100 nanoseconds
    return (((sysTicks * (uint64_t)1000000) + (int64_t)CH_CFG_ST_FREQUENCY - 1) / (int64_t)CH_CFG_ST_FREQUENCY) * 10;
}

// because HAL_Time_SysTicksToTime needs to be called from C we need a proxy to allow it to be called from 'C' code
extern "C"
{

    uint64_t HAL_Time_SysTicksToTime_C(uint64_t sysTicks)
    {
        return HAL_Time_SysTicksToTime(sysTicks);
    }
}

// implementation required for STM Cube package
// Provides a tick value in millisecond.
extern "C"
{

    uint32_t HAL_GetTick(void)
    {
        return TIME_I2MS(chVTGetSystemTimeX());
    }
}

// implementation required for STM Cube package
// This function provides minimum delay (in milliseconds).
extern "C"
{

    void HAL_Delay(uint32_t delay)
    {
        systime_t start = chVTGetSystemTime();
        systime_t end = start + TIME_MS2I(delay);

        do
        {
            // do nothing until the timeout expires
            chThdYield();
        } while (chVTIsSystemTimeWithin(start, end));
    }
}
