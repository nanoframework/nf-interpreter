//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

/*
 * Hardware Abstraction Layer for RNG Unit
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

// Initializes the RNG Driver
void rngInit(void)
{
    rng_lld_init();
}

// Configures and activates the CRC peripheral
void rngStart()
{

    // better lock this to setup the driver and start the peripheral
    osalSysLock();

    rng_lld_start();

    osalSysUnlock();
}

// Deactivates the RNG peripheral
void rngStop()
{

    // better lock this to stop the peripheral
    osalSysLock();

    rng_lld_stop();

    osalSysUnlock();
}

#if (RNG_USE_MUTUAL_EXCLUSION == TRUE)

void rngAquireModule()
{

    rng_lld_aquire();
}

void rngReleaseModule()
{

    rng_lld_release();
}

#endif /* RNG_USE_MUTUAL_EXCLUSION == TRUE */

bool rngGenerate(size_t size, uint8_t *out)
{
    RNGD1.State = RNG_ACTIVE;
    bool result = rng_lld_generate(size, out);
    RNGD1.State = RNG_READY;

    return result;
}

#endif /* HAL_NF_USE_RNG */
