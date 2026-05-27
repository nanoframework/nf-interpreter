//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <psa/crypto.h>

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen);

// RP2040 ROSC random bit register
#define ROSC_BASE       0x40060000
#define ROSC_RANDOMBIT  (*(volatile uint32_t *)(ROSC_BASE + 0x1C))

static uint32_t rp2040_get_random_u32(void)
{
    uint32_t value = 0;

    for (int i = 0; i < 32; i++)
    {
        value = (value << 1) | (ROSC_RANDOMBIT & 1u);
    }

    return value;
}

// Get len bytes of entropy from the RP2040 ROSC.
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    (void)data;

    for (size_t i = 0; i < len; i++)
    {
        output[i] = (unsigned char)(rp2040_get_random_u32() & 0xFF);
    }

    *olen = len;

    return 0;
}

psa_status_t mbedtls_psa_external_get_random(
    mbedtls_psa_external_random_context_t *context,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    (void)context;

    for (size_t i = 0; i < output_size; i++)
    {
        output[i] = (uint8_t)(rp2040_get_random_u32() & 0xFF);
    }

    *output_length = output_size;

    return PSA_SUCCESS;
}
