//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_TIME_H
#define TARGET_HAL_TIME_H

#include <cmsis_os.h>

#define HAL_Time_CurrentSysTicks chVTGetSystemTimeX

inline __attribute__((always_inline)) uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
{
    // convert to microseconds from CMSIS SysTicks
    return sysTicks * CH_CFG_ST_FREQUENCY * 10;
}

#endif // TARGET_HAL_TIME_H
