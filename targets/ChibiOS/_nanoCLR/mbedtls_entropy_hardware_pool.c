//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_nf_community.h>
#include <psa/crypto.h>
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen);
// Get len bytes of entropy from the hardware RNG.
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    (void)data;

#if (HAL_NF_USE_STM32_RNG == TRUE)

    // start random generator
    rngStart();

    for (size_t i = 0; i < len; i++)
    {
        // our generator returns 32bits numbers
        *output = rngGenerateRandomNumber();

        output++;
    }

    // callers require this to be set
    *olen = len;

    // stop random generator
    rngStop();

#elif (HAL_USE_TRNG == TRUE)

    trngStart(&TRNGD1, NULL);

    trngGenerate(&TRNGD1, len, output);

    // callers require this to be set
    *olen = len;

    trngStop(&TRNGD1);

#endif

    return 0;
}

psa_status_t mbedtls_psa_external_get_random(
    mbedtls_psa_external_random_context_t *context,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    (void)context;

#if HAL_NF_USE_STM32_RNG == TRUE

    // start random generator
    rngStart();

    for (size_t i = 0; i < output_size; i++)
    {
        // our generator returns 32bits numbers
        *output = (uint8_t)rngGenerateRandomNumber();

        output++;
    }

    // callers require this to be set
    *output_length = output_size;

    // stop random generator
    rngStop();

#elif (HAL_USE_TRNG == TRUE)

    trngStart(&TRNGD1, NULL);

    trngGenerate(&TRNGD1, output_size, output);

    // callers require this to be set
    *output_length = output_size;

    trngStop(&TRNGD1);

#endif

    return PSA_SUCCESS;
}
