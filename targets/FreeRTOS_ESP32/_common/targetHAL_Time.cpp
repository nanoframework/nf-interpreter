//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_Types.h>
#include <nanoCLR_Types.h>
#include <nanoHAL_Time.h>
#include <esp32_idf.h>

// Converts FreeRTOS Tickcount to .NET ticks (100 nanoseconds)
uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
{
    return (((int64_t)sysTicks * (int64_t)1000000 + (int64_t)configTICK_RATE_HZ - 1) / (int64_t)configTICK_RATE_HZ) *
           10;
}
