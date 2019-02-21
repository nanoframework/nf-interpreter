//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_Types.h>
#include <nanoCLR_Types.h>
#include <nanoHAL_Time.h>
#include <Esp32_os.h>

// Converts FreeRTOS Tickcount to .NET ticks (100 nanoseconds)
uint64_t HAL_Time_SysTicksToTime(unsigned int sysTicks) 
{
    return (((int64_t)sysTicks * (int64_t)1000000 + (int64_t)configTICK_RATE_HZ - 1) / (int64_t)configTICK_RATE_HZ) * 10;
}

// because HAL_Time_SysTicksToTime needs to be called from C we need a proxy to allow it to be called from 'C' code
extern "C" {
    
    uint64_t HAL_Time_SysTicksToTime_C(unsigned int sysTicks)
    {
        return HAL_Time_SysTicksToTime(sysTicks);
    }
}
