//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <mbedtls/entropy.h>
#include <psa/crypto.h>

// RNG HAL function - low level driver interface
extern uint32_t rng_lld_GenerateRandomNumber(void);

// Forward declarations
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen);
psa_status_t mbedtls_psa_external_get_random(
    mbedtls_psa_external_random_context_t *context,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

// ---------------------------------------------------------------------------
// mbedTLS entropy callback - uses HAL RNG driver
// ---------------------------------------------------------------------------
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    (void)data;

    if (len == 0)
    {
        *olen = 0;
        return 0;
    }

    // Use HAL RNG driver to generate random bytes
    for (size_t i = 0; i < len; i += 4)
    {
        uint32_t rand = rng_lld_GenerateRandomNumber();
        size_t take = (len - i < 4) ? (len - i) : 4;
        
        for (size_t j = 0; j < take; j++)
        {
            output[i + j] = (unsigned char)((rand >> (j * 8)) & 0xFF);
        }
    }

    *olen = len;
    return 0;
}

// ---------------------------------------------------------------------------
// PSA external random callback (used by mbedTLS 3.x PSA crypto layer)
// ---------------------------------------------------------------------------
psa_status_t mbedtls_psa_external_get_random(
    mbedtls_psa_external_random_context_t *context,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    (void)context;

    if (output_size == 0)
    {
        *output_length = 0;
        return PSA_SUCCESS;
    }

    // Use HAL RNG driver to generate random bytes
    for (size_t i = 0; i < output_size; i += 4)
    {
        uint32_t rand = rng_lld_GenerateRandomNumber();
        size_t take = (output_size - i < 4) ? (output_size - i) : 4;
        
        for (size_t j = 0; j < take; j++)
        {
            output[i + j] = (uint8_t)((rand >> (j * 8)) & 0xFF);
        }
    }

    *output_length = output_size;
    return PSA_SUCCESS;
}
