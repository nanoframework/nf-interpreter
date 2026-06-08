//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// HAL stubs for the standalone MCUboot bootloader binary (STM32F7xx).
//
// STM32 HAL functions needed by the nf-overlay QSPI bridge and target_ext_flash.c,
// provided as thin ChibiOS wrappers or no-ops.
//
// All definitions are weak so board-specific files can override them without
// conflicts.  For example, ORGPAL_PALTHREE supplies its own HAL_GetTick() in
// mcuboot_flash_map_boot.c which uses a different clock source.

#include "ch.h"
#include "hal.h"

// Return milliseconds since boot using ChibiOS system clock.
// Used by HAL_QSPI_Command/Receive/AutoPolling for timeout tracking.
__attribute__((weak)) uint32_t HAL_GetTick(void)
{
    return (uint32_t)(((uint64_t)chVTGetSystemTimeX() * 1000U) / CH_CFG_ST_FREQUENCY);
}

// Blocking millisecond delay using ChibiOS.
__attribute__((weak)) void HAL_Delay(uint32_t Delay)
{
    chThdSleepMilliseconds(Delay);
}

// Stub; W25Q512_EraseChip() is never called from MCUboot.
__attribute__((weak)) void Watchdog_Reset(void)
{
}

// Weak default log output: no-op.
// Boards that want MCUboot log output override this in their BSP file
// (e.g. route messages via chprintf to a UART or USB-CDC channel).
__attribute__((weak)) void nf_mcuboot_log_write(const char *msg)
{
    (void)msg;
}
