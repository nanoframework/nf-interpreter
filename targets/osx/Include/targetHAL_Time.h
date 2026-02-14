//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_HAL_TIME_H
#define TARGET_HAL_TIME_H

#include <cstdint>

uint64_t HAL_Time_CurrentSysTicks();
void HAL_Time_Sleep_MicroSeconds(unsigned int uSec);
void HAL_Time_Sleep_MicroSeconds_InterruptEnabled(unsigned int uSec);

#endif // TARGET_HAL_TIME_H
