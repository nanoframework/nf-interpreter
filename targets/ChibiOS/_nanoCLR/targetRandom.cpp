//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "Core.h"
#include <hal.h>
#include <hal_nf_community.h>

void CLR_RT_Random::Initialize()
{
#if (HAL_NF_USE_STM32_RNG == TRUE)

    rngStart();

#elif (HAL_USE_TRNG == TRUE)

    trngStart(&TRNGD1, NULL);

#else
#error "No hardware RNG source configured: enable HAL_NF_USE_STM32_RNG or HAL_USE_TRNG"
#endif
}

void CLR_RT_Random::Initialize(int seed)
{
    (void)seed;

#if (HAL_NF_USE_STM32_RNG == TRUE)

    rngStart();

#elif (HAL_USE_TRNG == TRUE)

    trngStart(&TRNGD1, NULL);

#else
#error "No hardware RNG source configured: enable HAL_NF_USE_STM32_RNG or HAL_USE_TRNG"
#endif
}

uint32_t CLR_RT_Random::Next()
{
#if (HAL_NF_USE_STM32_RNG == TRUE)

    return rngGenerateRandomNumber();

#elif (HAL_USE_TRNG == TRUE)

    uint32_t randomValue = 0;
    trngGenerate(&TRNGD1, sizeof(randomValue), (uint8_t *)&randomValue);

    return randomValue;

#else
#error "No hardware RNG source configured: enable HAL_NF_USE_STM32_RNG or HAL_USE_TRNG"
#endif
}

double CLR_RT_Random::NextDouble()
{
    // the hardware generator returns a value between 0 - 0xFFFFFFFF

#if (HAL_NF_USE_STM32_RNG == TRUE)

    return ((double)rngGenerateRandomNumber()) / ((double)0xFFFFFFFF);

#elif (HAL_USE_TRNG == TRUE)

    uint32_t randomValue = 0;
    trngGenerate(&TRNGD1, sizeof(randomValue), (uint8_t *)&randomValue);
    return ((double)randomValue) / ((double)0xFFFFFFFF);

#else
#error "No hardware RNG source configured: enable HAL_NF_USE_STM32_RNG or HAL_USE_TRNG"
#endif
}

void CLR_RT_Random::NextBytes(unsigned char *buffer, unsigned int count)
{
#if (HAL_NF_USE_STM32_RNG == TRUE)
    unsigned int i;

    for (i = 0; i < count; i++)
    {
        buffer[i] = (unsigned char)rngGenerateRandomNumber();
    }

#elif (HAL_USE_TRNG == TRUE)

    trngGenerate(&TRNGD1, count, buffer);

#else
#error "No hardware RNG source configured: enable HAL_NF_USE_STM32_RNG or HAL_USE_TRNG"
#endif
}
