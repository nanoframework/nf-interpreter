//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// RP2350 RNG stub — provides rngStart()/rngStop()/rngGenerateRandomNumber()
// using the Ring Oscillator (ROSC) random bit register.
//
// RP2350 ROSC base is 0x400E8000 (differs from RP2040's 0x40060000).
// The RANDOMBIT register is at offset 0x1C on both chips.

#include <stdint.h>
#include <hal.h>

#define RP2350_ROSC_BASE       0x400E8000U
#define RP2350_ROSC_RANDOMBIT  (*(volatile uint32_t *)(RP2350_ROSC_BASE + 0x1CU))

void rngStart(void)
{
    // ROSC is always running on RP2350
}

void rngStop(void)
{
    // Nothing to do
}

uint32_t rngGenerateRandomNumber(void)
{
    uint32_t value = 0;
    for (int i = 0; i < 32; i++)
    {
        value = (value << 1) | (RP2350_ROSC_RANDOMBIT & 1u);
    }
    return value;
}
