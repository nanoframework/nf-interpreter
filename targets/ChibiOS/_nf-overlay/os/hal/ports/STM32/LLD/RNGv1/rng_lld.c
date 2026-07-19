//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// possible help:
// https://github.com/xpacks/stm32f7-hal/blob/xpack/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rng.c
// https://github.com/ChibiOS/ChibiOS-Contrib/blob/master/os/hal/src/hal_rng.c

#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_NF_USE_RNG == TRUE)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief RNGD1 driver identifier.*/
RNGDriver RNGD1;

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

#define RNG_TIMEOUT_VALUE 200

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
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

    RNGD1.Instance = RNG;
}

void rng_lld_start()
{

    rccEnableRNG(FALSE)

        // Enables the RNG peripheral.
        __RNG_ENABLE(RNGD1);

    // driver is ready
    RNGD1.State = RNG_READY;
}

void rng_lld_stop()
{

    /* Disable the RNG Peripheral */
    CLEAR_BIT(RNGD1.Instance->CR, RNG_CR_IE | RNG_CR_RNGEN);

    /* Clear RNG interrupt status flags */
    CLEAR_BIT(RNGD1.Instance->SR, RNG_SR_CEIS | RNG_SR_SEIS);

    rccDisableRNG();

    // driver is stopped
    RNGD1.State = RNG_STOP;
}

bool rng_lld_generate(size_t size, uint8_t *out)
{
    while (size > 0)
    {
        systime_t start = osalOsGetSystemTimeX();
        systime_t end = start + OSAL_MS2I(RNG_TIMEOUT_VALUE);

        while (__RNG_GET_FLAG(RNGD1, RNG_FLAG_CECS) || __RNG_GET_FLAG(RNGD1, RNG_FLAG_SECS))
        {
            if (!osalTimeIsInRangeX(osalOsGetSystemTimeX(), start, end))
            {
                return false;
            }
        }

        while (!__RNG_GET_FLAG(RNGD1, RNG_FLAG_DRDY))
        {
            if (!osalTimeIsInRangeX(osalOsGetSystemTimeX(), start, end))
            {
                return false;
            }
        }

        uint32_t r = RNGD1.Instance->DR;

        for (size_t i = 0; i < sizeof(uint32_t) && size > 0; i++)
        {
            *out++ = (uint8_t)r;
            r >>= 8;
            size--;
        }
    }

    return true;
}

#if (RNG_USE_MUTUAL_EXCLUSION == TRUE)

void rng_lld_aquire()
{
    osalMutexLock(&RNGD1.Lock);
}

void rng_lld_release()
{
    osalMutexUnlock(&RNGD1.Lock);
}

#endif

#endif /* HAL_NF_USE_RNG */
