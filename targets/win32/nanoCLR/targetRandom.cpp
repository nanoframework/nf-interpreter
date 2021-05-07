//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"
#include <Core.h>
#include <random>

void CLR_RT_Random::Initialize()
{
}

void CLR_RT_Random::Initialize(int seed)
{
    (void)seed;
}

uint32_t CLR_RT_Random::Next()
{
    std::random_device randomDevice;

    return randomDevice();
}

double CLR_RT_Random::NextDouble()
{
    std::random_device randomDevice;

    return ((double)randomDevice()) / ((double)0xFFFFFFFF);
}

void CLR_RT_Random::NextBytes(unsigned char *buffer, unsigned int count)
{
    std::random_device randomDevice;

    for (unsigned int i = 0; i < count; i++)
    {
        buffer[i] = (unsigned char)randomDevice();
    }
}
