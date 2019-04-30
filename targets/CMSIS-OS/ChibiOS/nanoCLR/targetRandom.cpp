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
    TRNG_Init(TRNG0, &trngConfig);
}

void CLR_RT_Random::Initialize( int seed )
{
    (void)seed;
    TRNG_GetDefaultConfig(&trngConfig);
    TRNG_Init(TRNG0, &trngConfig);
}

uint32_t CLR_RT_Random::Next()
{
    uint32_t data = 0;
    TRNG_GetRandomData(TRNG, &data, sizeof(data));
    return data;
}

double CLR_RT_Random::NextDouble()
{
    uint32_t data = 0;
    TRNG_GetRandomData(TRNG, &data, sizeof(data));
    // the hardware generator returns a value between 0 - 0xFFFFFFFF
    return ((double)data()) / ((double)0xFFFFFFFF);
}

void CLR_RT_Random::NextBytes(unsigned char* buffer, unsigned int count)
{
    TRNG_GetRandomData(TRNG, buffer, count);    
}
