//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_NF_USE_RNG == TRUE)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#if defined(RP2040)

// RP2040: ROSC random bit register
#define ROSC_BASE      0x40060000UL
#define ROSC_RANDOMBIT (*(volatile uint32_t *)(ROSC_BASE + 0x1CUL))

#elif defined(RP2350)

// RP2350 TRNG peripheral (RP2350 datasheet section 12.12)
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
#define TRNG_SAMPLE_CNT1       (*(volatile uint32_t *)(TRNG_BASE + 0x130UL))
#define TRNG_DEBUG_CONTROL     (*(volatile uint32_t *)(TRNG_BASE + 0x138UL))
#define TRNG_BUSY              (*(volatile uint32_t *)(TRNG_BASE + 0x1B8UL))

#define TRNG_RNG_ISR_VN_ERR       (1UL << 3)
#define TRNG_RNG_ISR_CRNGT_ERR    (1UL << 2)
#define TRNG_RNG_ISR_AUTOCORR_ERR (1UL << 1)
#define TRNG_RNG_ISR_EHR_VALID    (1UL << 0)

#define TRNG_RNG_ICR_ALL                                                                                               \
    (TRNG_RNG_ISR_VN_ERR | TRNG_RNG_ISR_CRNGT_ERR | TRNG_RNG_ISR_AUTOCORR_ERR | TRNG_RNG_ISR_EHR_VALID)
#define TRNG_TRNG_VALID_EHR_VALID (1UL << 0)
#define TRNG_RND_SRC_EN           (1UL << 0)

#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief RNGD1 driver identifier.*/
RNGDriver RNGD1;

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level RNG driver initialization.
 *
 * @notapi
 */
void rng_lld_init(void)
{
    RNGD1.State = RNG_STOP;

#if (RNG_USE_MUTUAL_EXCLUSION == TRUE)
    osalMutexObjectInit(&RNGD1.Lock);
#endif

#if defined(RP2350)

    rp_peripheral_unreset(RESETS_ALLREG_TRNG);

    TRNG_RND_SOURCE_ENABLE = 0;
    // Sample one ROSC bit into EHR every cycle
    TRNG_SAMPLE_CNT1 = 0;
    // Disable checks and bypass decorrelators
    TRNG_DEBUG_CONTROL = -1;
    TRNG_RNG_ICR = TRNG_RNG_ICR_ALL;

#endif
}

void rng_lld_start(void)
{
#if defined(RP2350)
    // Per Pico SDK: set up sampling parameters, then ENABLE first, THEN clear ICR.
    TRNG_SAMPLE_CNT1 = 0;
    TRNG_DEBUG_CONTROL = (uint32_t)-1;
    TRNG_RND_SOURCE_ENABLE = TRNG_RND_SRC_EN;
    // Clear all flags AFTER enable - triggers fresh collection
    TRNG_RNG_ICR = (uint32_t)-1;
#endif

    RNGD1.State = RNG_READY;
}

void rng_lld_stop(void)
{
#if defined(RP2350)
    TRNG_RND_SOURCE_ENABLE = 0;
#endif

    RNGD1.State = RNG_STOP;
}

bool rng_lld_generate(size_t size, uint8_t *out)
{
    RNGD1.State = RNG_ACTIVE;

#if defined(RP2040)

    while (size > 0)
    {
        uint32_t value = 0;
        for (int i = 0; i < 32; i++)
        {
            value = (value << 1) | (ROSC_RANDOMBIT & 1u);
        }

        for (size_t i = 0; i < sizeof(uint32_t) && size > 0; i++)
        {
            *out++ = (uint8_t)value;
            value >>= 8;
            size--;
        }
    }

    RNGD1.State = RNG_READY;

    return true;

#elif defined(RP2350)

    // Per Pico SDK (pico_rand/rand.c capture_additional_trng_samples):
    // - rng_lld_start() already enabled source and cleared ICR, triggering first collection
    // - Here we just wait for BUSY LOW (data ready), then read ALL 6 EHR words
    // - Reading EHR_DATA5 (last word) hardware-triggers the next collection cycle automatically
    // - Do NOT write ICR inside this loop: it interrupts mid-collection and breaks re-arming

    while (size > 0)
    {
        // Wait for collection to complete
        uint32_t timeout_ms = 50;
        while (timeout_ms > 0 && TRNG_BUSY)
        {
            // 10 ms is a good compromise between responsiveness and CPU load, since the TRNG collection time is ~15ms
            osalThreadSleepMilliseconds(10);
            timeout_ms = timeout_ms - 10;
        }

        if (TRNG_BUSY)
        {
            RNGD1.State = RNG_READY;
            return false;
        }

        // ALWAYS read all 6 EHR words into a local buffer.
        uint32_t ehr[6];
        ehr[0] = TRNG_EHR_DATA0;
        ehr[1] = TRNG_EHR_DATA1;
        ehr[2] = TRNG_EHR_DATA2;
        ehr[3] = TRNG_EHR_DATA3;
        ehr[4] = TRNG_EHR_DATA4;
        ehr[5] = TRNG_EHR_DATA5;

        // Copy needed bytes from local EHR buffer to output
        for (int r = 0; r < 6 && size > 0; r++)
        {
            uint32_t word = ehr[r];
            for (size_t b = 0; b < sizeof(uint32_t) && size > 0; b++)
            {
                *out++ = (uint8_t)word;
                word >>= 8;
                size--;
            }
        }
    }

    RNGD1.State = RNG_READY;

    return true;

#else
    (void)size;
    (void)out;

    RNGD1.State = RNG_READY;

    return false;
#endif
}

#if (RNG_USE_MUTUAL_EXCLUSION == TRUE)

void rng_lld_aquire(void)
{
    osalMutexLock(&RNGD1.Lock);
}

void rng_lld_release(void)
{
    osalMutexUnlock(&RNGD1.Lock);
}

#endif

#endif /* HAL_NF_USE_RNG */
