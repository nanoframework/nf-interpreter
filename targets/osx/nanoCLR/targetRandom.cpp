//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <cstdint>
#include <random>

uint32_t nanoCLR_OSX_NextRandomU32()
{
    static std::random_device randomDevice;
    return randomDevice();
}

double nanoCLR_OSX_NextRandomDouble()
{
    static std::random_device randomDevice;
    return static_cast<double>(randomDevice()) / static_cast<double>(0xFFFFFFFFU);
}

void nanoCLR_OSX_FillRandomBytes(uint8_t *buffer, unsigned int count)
{
    if (buffer == nullptr)
    {
        return;
    }

    static std::random_device randomDevice;

    for (unsigned int i = 0; i < count; i++)
    {
        buffer[i] = static_cast<uint8_t>(randomDevice());
    }
}
