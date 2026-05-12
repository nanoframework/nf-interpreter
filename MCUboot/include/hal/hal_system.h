//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Mynewt HAL system stub for MCUboot boot_serial.c.
// Maps hal_system_reset() → CMSIS NVIC_SystemReset() equivalent.
//
// We inline the Cortex-M reset sequence directly to avoid pulling in
// core_cm7.h (which requires the device header to define IRQn_Type etc.),
// since boot_serial.c includes this before hal.h / the device header chain.
// The implementation matches CMSIS NVIC_SystemReset() exactly.

#ifndef HAL_HAL_SYSTEM_H
#define HAL_HAL_SYSTEM_H

__attribute__((noreturn)) static inline void hal_system_reset(void)
{
    // Ensure all outstanding memory accesses are complete before the reset.
    __asm__ volatile("dsb 0xF" ::: "memory");
    // Write SYSRESETREQ to SCB->AIRCR (keep PRIGROUP, set VECTKEY).
    *((volatile unsigned int *)0xE000ED0CUL) = (0x5FAUL << 16UL) | (1UL << 2UL);
    __asm__ volatile("dsb 0xF" ::: "memory");
    // Wait for reset — should not return.
    while (1)
    {
    }
}

#endif // HAL_HAL_SYSTEM_H
