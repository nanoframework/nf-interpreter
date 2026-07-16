//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <mbedtls/entropy.h>
#include <psa/crypto.h>

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen);

// RP2350 TRNG peripheral registers (datasheet section 12.12)
#define TRNG_BASE              0x400F0000UL
#define TRNG_RNG_ISR           (*(volatile uint32_t *)(TRNG_BASE + 0x104UL))
#define TRNG_RNG_ICR           (*(volatile uint32_t *)(TRNG_BASE + 0x108UL))
#define TRNG_TRNG_VALID        (*(volatile uint32_t *)(TRNG_BASE + 0x110UL))
#define TRNG_EHR_DATA0         (*(volatile uint32_t *)(TRNG_BASE + 0x114UL))
#define TRNG_EHR_DATA1         (*(volatile uint32_t *)(TRNG_BASE + 0x118UL))
#define TRNG_EHR_DATA2         (*(volatile uint32_t *)(TRNG_BASE + 0x11CUL))
#define TRNG_EHR_DATA3         (*(volatile uint32_t *)(TRNG_BASE + 0x120UL))
#define TRNG_EHR_DATA4         (*(volatile uint32_t *)(TRNG_BASE + 0x124UL))
#define TRNG_EHR_DATA5         (*(volatile uint32_t *)(TRNG_BASE + 0x128UL))
#define TRNG_RND_SOURCE_ENABLE (*(volatile uint32_t *)(TRNG_BASE + 0x12CUL))
#define TRNG_TRNG_SW_RESET     (*(volatile uint32_t *)(TRNG_BASE + 0x140UL))
#define TRNG_RST_BITS_COUNTER  (*(volatile uint32_t *)(TRNG_BASE + 0x1BCUL))

#define TRNG_ISR_ERRORS (1UL << 3 | 1UL << 2 | 1UL << 1) // VN_ERR | CRNGT_ERR | AUTOCORR_ERR
#define TRNG_ISR_EHR_VALID (1UL << 0)
#define TRNG_ICR_ALL       (TRNG_ISR_ERRORS | TRNG_ISR_EHR_VALID)
#define TRNG_VALID_EHR     (1UL << 0)
#define TRNG_RND_SRC_EN    (1UL << 0)

// RESETS_ALLREG_TRNG (bit 25) and RESETS struct are defined in rp2350.h via hal.h.

// Maximum poll loops waiting for EHR_VALID (~500 ms budget at 150 MHz)
#define c_TRNG_POLL_LIMIT   5000000UL

// ---------------------------------------------------------------------------
// Initialise TRNG and prepare entropy source.
// Must be called with source disabled; safe to call multiple times (idempotent).
// ---------------------------------------------------------------------------
static void trng_init(void)
{
    // Take TRNG out of reset using the atomic CLR alias (RESETS->CLR.RESET).
    RESETS->CLR.RESET = RESETS_ALLREG_TRNG;
    while ((RESETS->RESET_DONE & RESETS_ALLREG_TRNG) == 0)
    {
    }

    // Reset bit counter must be done while source is disabled (spec requirement).
    TRNG_RND_SOURCE_ENABLE = 0;
    TRNG_TRNG_SW_RESET = 1;
    TRNG_TRNG_SW_RESET = 0;
    TRNG_RST_BITS_COUNTER = 1;
    TRNG_RNG_ICR = TRNG_ICR_ALL;

    // Enable entropy source; hardware default SAMPLE_CNT1 (0xFFFF) is retained.
    TRNG_RND_SOURCE_ENABLE = TRNG_RND_SRC_EN;
}

// ---------------------------------------------------------------------------
// Read one full EHR batch (192 bits = 24 bytes) into buf[0..5].
// Returns 0 on success, -1 on unrecoverable error (too many retries).
// ---------------------------------------------------------------------------
static int trng_read_batch(uint32_t buf[6])
{
    for (int retries = 0; retries < 8; retries++)
    {
        // Wait for EHR_VALID.
        uint32_t poll = 0;
        while (poll < c_TRNG_POLL_LIMIT)
        {
            uint32_t isr = TRNG_RNG_ISR;

            // AUTOCORR_ERR halts the RNG until reset; recover immediately.
            if (isr & TRNG_ISR_ERRORS)
            {
                trng_init();
                poll = 0;
                continue;
            }

            if ((TRNG_TRNG_VALID & TRNG_VALID_EHR) || (isr & TRNG_ISR_EHR_VALID))
            {
                // Read all six EHR words – required to fully consume the batch.
                buf[0] = TRNG_EHR_DATA0;
                buf[1] = TRNG_EHR_DATA1;
                buf[2] = TRNG_EHR_DATA2;
                buf[3] = TRNG_EHR_DATA3;
                buf[4] = TRNG_EHR_DATA4;
                buf[5] = TRNG_EHR_DATA5;

                // Acknowledge; TRNG immediately starts collecting the next batch.
                TRNG_RNG_ICR = TRNG_ISR_EHR_VALID;

                return 0;
            }

            poll++;
        }

        // Timeout – try a fresh init before retrying.
        trng_init();
    }

    return -1;
}

// ---------------------------------------------------------------------------
// Fill output with entropy from the RP2350 TRNG.
// Reads 24 bytes (one full 192-bit EHR batch) at a time so every sampled bit
// contributes to output instead of discarding 23 of every 24 bytes.
// ---------------------------------------------------------------------------
static int rp2350_trng_fill(unsigned char *output, size_t len)
{
    trng_init();

    size_t filled = 0;

    while (filled < len)
    {
        uint32_t batch[6];

        if (trng_read_batch(batch) != 0)
        {
            return -1;
        }

        // Copy as many bytes from this 24-byte batch as still needed.
        size_t remaining = len - filled;
        size_t take = (remaining < sizeof(batch)) ? remaining : sizeof(batch);
        const unsigned char *src = (const unsigned char *)batch;

        for (size_t i = 0; i < take; i++)
        {
            output[filled + i] = src[i];
        }

        filled += take;
    }

    return 0;
}

// ---------------------------------------------------------------------------
// mbedTLS entropy callback
// ---------------------------------------------------------------------------
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    (void)data;

    if (rp2350_trng_fill(output, len) != 0)
    {
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
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

    if (rp2350_trng_fill(output, output_size) != 0)
    {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    *output_length = output_size;

    return PSA_SUCCESS;
}
