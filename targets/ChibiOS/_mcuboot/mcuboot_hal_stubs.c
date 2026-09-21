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

#include <mcuboot_media_import.h>

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

// Media import: feed the watchdog once per copied chunk. Watchdog_Reset() is the real
// IWDG kick when NF_FEATURE_WATCHDOG is on, the no-op above otherwise.
void mcuboot_media_watchdog_feed(void)
{
    Watchdog_Reset();
}

// Media import: this device's identity for the per-device "used" marker.
// STM32 parts expose a 96-bit unique id at UID_BASE; rendered as 24 lowercase hex chars.
__attribute__((weak)) size_t mcuboot_media_device_id(char *hex, size_t cap)
{
#if defined(UID_BASE)
    static const char digits[] = "0123456789abcdef";
    const uint8_t *uid = (const uint8_t *)UID_BASE;
    const size_t uidLen = 12U;

    if (cap < (uidLen * 2U) + 1U)
    {
        return 0;
    }

    for (size_t i = 0; i < uidLen; i++)
    {
        hex[2U * i] = digits[uid[i] >> 4];
        hex[(2U * i) + 1U] = digits[uid[i] & 0x0FU];
    }

    hex[uidLen * 2U] = '\0';

    return uidLen * 2U;
#else
    (void)hex;
    (void)cap;
    return 0;
#endif
}
