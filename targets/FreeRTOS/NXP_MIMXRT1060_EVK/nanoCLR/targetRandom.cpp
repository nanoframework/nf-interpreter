//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//
#include "Core.h"
#include "fsl_trng.h"

trng_config_t trngConfig;

void CLR_RT_Random::Initialize()
{   
    TRNG_GetDefaultConfig(&trngConfig);
    TRNG_Init(TRNG, &trngConfig);
}

void CLR_RT_Random::Initialize( int seed )
{
    (void)seed;
}

uint32_t CLR_RT_Random::Next()
{
    uint32_t data = 0x0;
    uint32_t * data_p = &data;
    TRNG_GetRandomData(TRNG, (void *) data_p, sizeof(data));
    return data;
}

double CLR_RT_Random::NextDouble()
{
    // the hardware generator returns a value between 0 - 0xFFFFFFFF
    uint32_t data = 0x0;
    uint32_t * data_p = &data;
    TRNG_GetRandomData(TRNG, (void *) data_p, sizeof(data));
    return ((double) data) / ((double)0xFFFFFFFF);
}

void CLR_RT_Random::NextBytes(unsigned char* buffer, unsigned int count)
{
    TRNG_GetRandomData(TRNG, buffer, count);
    return;
}
