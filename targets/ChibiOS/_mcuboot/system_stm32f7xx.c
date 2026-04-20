//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Minimal SystemInit() for the standalone MCUboot bootloader on STM32F769ZI.
//
// This implementation intentionally avoids configuring the PLL or changing the
// system clock.  After power-on reset the STM32F769 runs on the HSI oscillator
// at 16 MHz, which is sufficient for:
//   - Flash programming (FLASHv2 write speed is latency-bound, not clock-bound)
//   - SPI1 @ fPCLK/2 = 8 MHz for AT25SF641 transfers
//   - MCUboot image validation (SHA-256 hash + ECDSA P-256 verify)
//
// Actions performed:
//   1. Enable the FPU (required for Cortex-M7 with FPU; CMSIS uses float ops).
//   2. Set SCB->VTOR to the bootloader flash base (0x08000000) so that
//      exceptions are dispatched from the correct vector table.
//   3. Enable instruction and data caches for faster flash reads during
//      MCUboot image validation.
//
// Note: Cache invalidation before/after flash erase/write is handled by the
// STM32F7 hardware (FLASHv2 flushes the ART accelerator automatically) and by
// the bare-metal flash driver (stm32f7_flash_bare.c).

#include <stdint.h>
#include <stm32f769xx.h>
#include <core_cm7.h>

void SystemInit(void)
{
    // 1. Enable the FPU.
    //    CP10 and CP11 must both be set to "full access" (0b11).
    SCB->CPACR |= ((3UL << 10U * 2U) | (3UL << 11U * 2U));
    __DSB();
    __ISB();

    // 2. Set VTOR to the start of bootloader flash.
    //    The reset value of VTOR is 0x00000000; on STM32F7 after a normal boot
    //    the flash alias at 0x00000000 maps to 0x08000000, but setting VTOR
    //    explicitly avoids any ambiguity.
    SCB->VTOR = FLASH_BASE; // 0x08000000
    __DSB();

    // 3. Enable instruction cache (ICACHE).
    if (!(SCB->CCR & SCB_CCR_IC_Msk))
    {
        SCB_EnableICache();
    }

    // 4. Enable data cache (DCACHE).
    if (!(SCB->CCR & SCB_CCR_DC_Msk))
    {
        SCB_EnableDCache();
    }
}
