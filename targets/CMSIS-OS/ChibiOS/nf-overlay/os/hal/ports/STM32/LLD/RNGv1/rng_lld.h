//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

//Help: https://github.com/xpacks/stm32f7-hal/blob/xpack/Drivers/STM32F7xx_HAL_Driver/Inc/stm32f7xx_hal_rng.h

#ifndef RNG_LLD_H
#define RNG_LLD_H

#include "stm32_registry.h"
#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_USE_STM32_RNG == TRUE)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

// RNG Flag definitions
#define RNG_FLAG_DRDY   RNG_SR_DRDY  /*!< Data ready                 */
#define RNG_FLAG_CECS   RNG_SR_CECS  /*!< Clock error current status */
#define RNG_FLAG_SECS   RNG_SR_SECS  /*!< Seed error current status  */

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
 * @brief   Structure representing an RNG driver.
 */
typedef struct RNGDriver {
    //Register base address
    RNG_TypeDef             *Instance;  

    // Driver state.
    rngState                State;

    // Last Generated random number.
    uint32_t                RandomNumber; 

#if RNG_USE_MUTUAL_EXCLUSION
    // Mutex protecting the peripheral
    mutex_t                 Lock;
#endif // RNG_USE_MUTUAL_EXCLUSION

} RNGDriver;

// From STMicroelectronics Cube HAL 
/////////////////////////////////////////////////////////////

#if defined(STM32L0XX)
// this series uses different names for the buses

#define rccEnableRNG(lp)  rccEnableAPB2(RCC_AHBENR_RNGEN, lp)
#define rccDisableRNG()   rccDisableAPB2(RCC_AHBENR_RNGEN)

#else

#define rccEnableRNG(lp)  rccEnableAHB2(RCC_AHB2ENR_RNGEN, lp)
#define rccDisableRNG()   rccDisableAHB2(RCC_AHB2ENR_RNGEN)

#endif

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

// Enables the RNG peripheral.
#define __RNG_ENABLE(__HANDLE__) ((__HANDLE__).Instance->CR |=  RNG_CR_RNGEN)

// Disables the RNG peripheral.
#define __RNG_DISABLE(__HANDLE__) ((__HANDLE__).Instance->CR &= ~RNG_CR_RNGEN)

// Check the selected RNG flag status.
//          This parameter can be one of the following values:
//            @arg RNG_FLAG_DRDY: Data ready                
//            @arg RNG_FLAG_CECS: Clock error current status
//            @arg RNG_FLAG_SECS: Seed error current status 
#define __RNG_GET_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__).Instance->SR & (__FLAG__)) == (__FLAG__))


  //  Clears the selected RNG flag status.
  //   WARNING: This is a dummy macro for HAL code alignment,
  //         flags RNG_FLAG_DRDY, RNG_FLAG_CECS and RNG_FLAG_SECS are read-only.
#define __RNG_CLEAR_FLAG(__HANDLE__, __FLAG__) 

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void rng_lld_init(void);
  void rng_lld_start(void);
  void rng_lld_stop(void);
  uint32_t rng_lld_GenerateRandomNumber(void);
  uint32_t rng_lld_GetLastRandomNumber(void);

#if (RNG_USE_MUTUAL_EXCLUSION == TRUE)

  void rng_lld_aquire();
  void rng_lld_release();

#endif 

#ifdef __cplusplus
}
#endif

#endif /* HAL_USE_STM32_RNG */

#endif /* RNG_LLD_H */
