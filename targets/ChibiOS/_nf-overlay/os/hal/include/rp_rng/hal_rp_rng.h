//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HAL_NF_RP_RNG_H
#define HAL_NF_RP_RNG_H

#if (HAL_NF_USE_RP_RNG == TRUE)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */

/**
 * @brief   Enables the @p rngAcquireModule() and @p rngReleaseModule() APIs.
 * @note    Disabling this option saves both code and data space.
 */
#if !defined(RNG_USE_MUTUAL_EXCLUSION)
#define RNG_USE_MUTUAL_EXCLUSION FALSE
#endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

#include "rng_lld.h"

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

    void rngInit(void);
    void rngStart(void);
    void rngStop(void);
    uint32_t rngGenerateRandomNumber(void);
    uint32_t rngGetLastRandomNumber(void);

#if RNG_USE_MUTUAL_EXCLUSION == TRUE
    void rngAcquireModule(void);
    void rngReleaseModule(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // HAL_NF_USE_RP_RNG

#endif // HAL_NF_RP_RNG_H
