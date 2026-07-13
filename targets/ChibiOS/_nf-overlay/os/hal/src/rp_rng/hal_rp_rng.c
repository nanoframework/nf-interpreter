//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

/*
 * Hardware Abstraction Layer for RP2040/RP2350 RNG
 */
#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_NF_USE_RNG == TRUE)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

void rngInit(void)
{
    rng_lld_init();
}

void rngStart(void)
{
    osalSysLock();
    rng_lld_start();
    osalSysUnlock();
}

void rngStop(void)
{
    osalSysLock();
    rng_lld_stop();
    osalSysUnlock();
}

#if (RNG_USE_MUTUAL_EXCLUSION == TRUE)

void rngAcquireModule(void)
{
    rng_lld_aquire();
}

void rngReleaseModule(void)
{
    rng_lld_release();
}

#endif /* RNG_USE_MUTUAL_EXCLUSION == TRUE */

uint32_t rngGenerateRandomNumber(void)
{
    return rng_lld_GenerateRandomNumber();
}

uint32_t rngGetLastRandomNumber(void)
{
    uint32_t randomNumber;
    randomNumber = rng_lld_GetLastRandomNumber();

    return randomNumber;
}

#endif /* HAL_NF_USE_RNG */
