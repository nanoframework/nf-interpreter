//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "Core.h"
#include <em_device.h>
#include <sli_crypto_driver_trng.h>

void CLR_RT_Random::Initialize()
{
    // no need to do anything here
}

void CLR_RT_Random::Initialize(int seed)
{
    (void)seed;

    // no need to do anything here
}

uint32_t CLR_RT_Random::Next()
{
    uint32_t rand;
    size_t outLength;

    sli_crypto_trng_get_random((unsigned char *)&rand, sizeof(rand), &outLength);

    return rand;
}

double CLR_RT_Random::NextDouble()
{
    uint32_t rand;
    size_t outLength;

    sli_crypto_trng_get_random((unsigned char *)&rand, sizeof(rand), &outLength);

    // the hardware generator returns a value between 0 - 0xFFFFFFFF
    return ((double)rand) / ((double)0xFFFFFFFF);
}

void CLR_RT_Random::NextBytes(unsigned char *buffer, unsigned int count)
{
    size_t outLength;

    sli_crypto_trng_get_random(buffer, count, &outLength);
}
