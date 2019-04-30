//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//
#include "Core.h"
#include <hal.h>
#include <hal_nf_community.h>

void CLR_RT_Random::Initialize()
{
    rngStart();
}

void CLR_RT_Random::Initialize( int seed )
{
    (void)seed;

    rngStart();
}

uint32_t CLR_RT_Random::Next()
{
    return rngGenerateRandomNumber();
}

double CLR_RT_Random::NextDouble()
{
    // the hardware generator returns a value between 0 - 0xFFFFFFFF
    return ((double)rngGenerateRandomNumber()) / ((double)0xFFFFFFFF);
}

void CLR_RT_Random::NextBytes(unsigned char* buffer, unsigned int count)
{
    unsigned int i;

    for(i = 0; i < count; i++)
    {
        buffer[i] = (unsigned char)rngGenerateRandomNumber();
    }
}
