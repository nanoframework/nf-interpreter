//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Bare-metal STM32F7 FLASHv2 driver for the standalone MCUboot bootloader.
//
// This driver provides minimal flash unlock/write/erase operations using
// direct CMSIS register access.  It has NO ChibiOS, HAL, or RTOS dependencies
// and is intended exclusively for the MCUboot bootloader binary context.
//
// Hardware: STM32F769ZI FLASHv2 peripheral.
// Write unit: 32-bit word (PSIZE = 0b10), aligned to 4 bytes.
//             Matches MCUBOOT_FLASH_WRITE_ALIGNMENT = 4.

#ifndef STM32F7_FLASH_BARE_H
#define STM32F7_FLASH_BARE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // Initialise the flash peripheral (currently a no-op; the peripheral is
    // always powered on after reset).  Called once from main().
    void stm32f7_flash_init(void);

    // Write len bytes from buf to internal flash starting at addr.
    // addr and len must both be multiples of 4 (MCUBOOT_FLASH_WRITE_ALIGNMENT).
    // The destination region must be erased before calling this function.
    // Returns 0 on success, -1 on flash programming error.
    int stm32f7_flash_write(uint32_t addr, uint32_t len, const uint8_t *buf);

    // Erase the flash sector that contains addr.
    // addr must be the first address of the sector (aligned to sector boundary).
    // Returns 0 on success, -1 on erase error.
    int stm32f7_flash_erase(uint32_t addr);

#ifdef __cplusplus
}
#endif

#endif // STM32F7_FLASH_BARE_H
