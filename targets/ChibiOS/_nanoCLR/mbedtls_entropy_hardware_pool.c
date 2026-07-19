//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_nf_community.h>
#include <mbedtls/entropy.h>
#include <psa/crypto.h>
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen);
// Get len bytes of entropy from the hardware RNG.
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    (void)data;

#if (HAL_NF_USE_RNG == TRUE)

    rngStart();

    if (!rngGenerate(len, output))
    {
        rngStop();
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }

    *olen = len;

    rngStop();

#elif (HAL_USE_TRNG == TRUE)

    trngStart(&TRNGD1, NULL);

    // trngGenerate returns true if an error occurred
    if (trngGenerate(&TRNGD1, len, output) == true)
    {
        trngStop(&TRNGD1);
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }

    *olen = len;

    trngStop(&TRNGD1);

#else
#error "No hardware RNG source configured: enable HAL_NF_USE_RNG or HAL_USE_TRNG"
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

#if (HAL_NF_USE_RNG == TRUE)

    rngStart();

    if (!rngGenerate(output_size, output))
    {
        rngStop();
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    *output_length = output_size;

    rngStop();

#elif (HAL_USE_TRNG == TRUE)

    trngStart(&TRNGD1, NULL);

    // trngGenerate returns true if an error occurred
    if (trngGenerate(&TRNGD1, output_size, output) == true)
    {
        trngStop(&TRNGD1);
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    *output_length = output_size;

    trngStop(&TRNGD1);

#else
#error "No hardware RNG source configured: enable HAL_NF_USE_RNG or HAL_USE_TRNG"
#endif

    return PSA_SUCCESS;
}
