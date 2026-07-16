//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef RNG_LLD_H
#define RNG_LLD_H

#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_NF_USE_RNG == TRUE)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Driver state machine possible states.
 */
typedef enum
{
    RNG_UNINIT, /* Not initialized.                           */
    RNG_STOP,   /* Stopped.                                   */
    RNG_READY,  /* Ready.                                     */
    RNG_ACTIVE  /* Generating random number.                  */
} rngState;

/**
 * @brief   Structure representing an RNG driver.
 */
typedef struct RNGDriver
{

    // Driver state.
    rngState State;

    // Last Generated random number.
    uint32_t RandomNumber;

#if RNG_USE_MUTUAL_EXCLUSION
    // Mutex protecting the peripheral
    mutex_t Lock;
#endif // RNG_USE_MUTUAL_EXCLUSION

} RNGDriver;

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

    void rng_lld_init(void);
    void rng_lld_start(void);
    void rng_lld_stop(void);
    bool rng_lld_generate(size_t size, uint8_t *out);
    uint32_t rng_lld_GenerateRandomNumber(void);
    uint32_t rng_lld_GetLastRandomNumber(void);

#if (RNG_USE_MUTUAL_EXCLUSION == TRUE)

    void rng_lld_aquire(void);
    void rng_lld_release(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* HAL_NF_USE_RNG */

#endif /* RNG_LLD_H */
