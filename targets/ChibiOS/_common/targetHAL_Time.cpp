//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>

#ifdef __cplusplus
extern "C"
{
    uint32_t HAL_GetTick(void);
    void HAL_Delay(uint32_t delay);
}
#endif

// implementation required for STM Cube package
// Provides a tick value in millisecond.
uint32_t HAL_GetTick(void)
{
    return TIME_I2MS(chVTGetSystemTimeX());
}

// implementation required for STM Cube package
// This function provides minimum delay (in milliseconds).
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
