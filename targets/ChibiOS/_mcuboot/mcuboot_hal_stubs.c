//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// HAL stubs for the standalone MCUboot bootloader binary (STM32F7xx).
//
// STM32 HAL functions needed by the nf-overlay QSPI bridge and target_ext_flash.c,
// provided as thin ChibiOS wrappers or no-ops.

#include "ch.h"
#include "hal.h"

// Return milliseconds since boot using ChibiOS system clock.
// Used by HAL_QSPI_Command/Receive/AutoPolling for timeout tracking.
uint32_t HAL_GetTick(void)
{
    return (uint32_t)(((uint64_t)chVTGetSystemTimeX() * 1000U) / CH_CFG_ST_FREQUENCY);
}

// Blocking millisecond delay using ChibiOS.
void HAL_Delay(uint32_t Delay)
{
    chThdSleepMilliseconds(Delay);
}

// Stub; W25Q512_EraseChip() is never called from MCUboot.
void Watchdog_Reset(void)
{
}
