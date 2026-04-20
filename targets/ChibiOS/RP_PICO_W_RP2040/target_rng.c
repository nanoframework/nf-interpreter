//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// RP2040 RNG stub — provides rngStart()/rngStop()/rngGenerateRandomNumber()
// using the Ring Oscillator (ROSC) random bit register.

#include <stdint.h>
#include <hal.h>

#define ROSC_BASE       0x40060000
#define ROSC_RANDOMBIT  (*(volatile uint32_t *)(ROSC_BASE + 0x1C))

void rngStart(void)
{
    // ROSC is always running on RP2040
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
        value = (value << 1) | (ROSC_RANDOMBIT & 1u);
    }
    return value;
}
