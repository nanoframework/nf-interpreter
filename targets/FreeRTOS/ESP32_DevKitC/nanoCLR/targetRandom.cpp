//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//
#include "Core.h"

void CLR_RT_Random::Initialize()
{
}

void CLR_RT_Random::Initialize( int seed )
{
    (void)seed;
}

uint32_t CLR_RT_Random::Next()
{
    return esp_random();
}

double CLR_RT_Random::NextDouble()
{
    // the hardware generator returns a value between 0 - 0xFFFFFFFF
    return ((double)esp_random()) / ((double)0xFFFFFFFF);
}

void CLR_RT_Random::NextBytes(unsigned char* buffer, unsigned int count)
{
    unsigned int i;

    for(i = 0; i < count; i++)
    {
        buffer[i] = (unsigned char)esp_random();
    }
}
