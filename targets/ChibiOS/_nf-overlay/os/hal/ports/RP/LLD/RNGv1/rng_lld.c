//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_NF_USE_RP_RNG == TRUE)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#if defined(RP2040)

// RP2040: ROSC random bit register
#define ROSC_BASE      0x40060000UL
#define ROSC_RANDOMBIT (*(volatile uint32_t *)(ROSC_BASE + 0x1CUL))

#elif defined(RP2350)

// RP2350: hardware TRNG peripheral
#define TRNG_BASE              0x400c0000UL
#define TRNG_EHR_DATA0         (*(volatile uint32_t *)(TRNG_BASE + 0x00UL))
#define TRNG_RNG_ISR           (*(volatile uint32_t *)(TRNG_BASE + 0x104UL))
#define TRNG_SAMPLE_CNT1       (*(volatile uint32_t *)(TRNG_BASE + 0x108UL))
#define TRNG_CONFIG            (*(volatile uint32_t *)(TRNG_BASE + 0x10CUL))
#define TRNG_RNG_ISR_EHR_VALID (1UL << 0)
#define TRNG_CONFIG_RNG_EN     (1UL << 1)

// With SAMPLE_CNT1 = 0, a full 192-bit EHR fills in well under 1 ms.
#define RNG_TIMEOUT_VALUE      20

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
}

void rng_lld_start(void)
{
    // RP2040: ROSC is always running, nothing to enable

#if defined(RP2350)
    // smallest sample interval so an EHR fills in <1 ms;
    TRNG_SAMPLE_CNT1 = 0;
    TRNG_CONFIG |= TRNG_CONFIG_RNG_EN;
#endif

    RNGD1.State = RNG_READY;
}

void rng_lld_stop(void)
{
#if defined(RP2350)
    TRNG_CONFIG &= ~TRNG_CONFIG_RNG_EN;
#endif

    RNGD1.State = RNG_STOP;
}

uint32_t rng_lld_GenerateRandomNumber(void)
{
#if defined(RP2040)

    uint32_t value = 0;
    for (int i = 0; i < 32; i++)
    {
        value = (value << 1) | (ROSC_RANDOMBIT & 1u);
    }
    RNGD1.RandomNumber = value;

#elif defined(RP2350)

    for (uint32_t elapsed = 0; elapsed < RNG_TIMEOUT_VALUE; elapsed++)
    {
        if ((TRNG_RNG_ISR & TRNG_RNG_ISR_EHR_VALID) != 0)
        {
            RNGD1.RandomNumber = TRNG_EHR_DATA0;

            // Clear EHR_VALID to trigger collection of the next EHR
            TRNG_RNG_ISR = TRNG_RNG_ISR_EHR_VALID;

            return RNGD1.RandomNumber;
        }

        // Yield to other threads between checks
        osalThreadSleepMilliseconds(1);
    }

    return 0;

#endif

    return RNGD1.RandomNumber;
}

uint32_t rng_lld_GetLastRandomNumber(void)
{
    return RNGD1.RandomNumber;
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

#endif /* HAL_NF_USE_RP_RNG */
