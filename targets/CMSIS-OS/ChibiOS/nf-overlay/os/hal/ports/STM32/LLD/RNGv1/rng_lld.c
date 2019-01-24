//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


//possible help:
// https://github.com/xpacks/stm32f7-hal/blob/xpack/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rng.c
// https://github.com/ChibiOS/ChibiOS-Contrib/blob/master/os/hal/src/hal_rng.c

#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_USE_STM32_RNG == TRUE)

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

#define RNG_TIMEOUT_VALUE     200

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
void rng_lld_init(void) {
    RNGD1.State  = RNG_STOP;

    #if (RNG_USE_MUTUAL_EXCLUSION == TRUE)
    osalMutexObjectInit(&RNGD1.Lock);
    #endif   

    RNGD1.Instance = RNG;
}


void rng_lld_start() {

  rccEnableRNG(FALSE)

  // Enables the RNG peripheral.
  __RNG_ENABLE(RNGD1);

  // driver is ready
  RNGD1.State = RNG_READY;
}


void rng_lld_stop() {

  /* Disable the RNG Peripheral */
  CLEAR_BIT(RNGD1.Instance->CR, RNG_CR_IE | RNG_CR_RNGEN);

  /* Clear RNG interrupt status flags */
  CLEAR_BIT(RNGD1.Instance->SR, RNG_SR_CEIS | RNG_SR_SEIS);

  rccDisableRNG();
  
  // driver is stopped
  RNGD1.State = RNG_STOP;
}


uint32_t rng_lld_GenerateRandomNumber() {
  systime_t start = chVTGetSystemTime();
  systime_t end = start + TIME_MS2I(RNG_TIMEOUT_VALUE);


    /* Check if data register contains valid random data */
    while(__RNG_GET_FLAG(RNGD1, RNG_FLAG_DRDY) == RESET)
    {
      if(!chVTIsSystemTimeWithin(start, end))
      {   
        return 0;
      } 

    }

    /* Get a 32bit Random number */
    RNGD1.RandomNumber = RNGD1.Instance->DR;

  return RNGD1.RandomNumber;
}


uint32_t rng_lld_GetLastRandomNumber() {
  return RNGD1.RandomNumber;
}


#if (RNG_USE_MUTUAL_EXCLUSION == TRUE)

void rng_lld_aquire() {
    osalMutexLock(&RNGD1.Lock);
}

void rng_lld_release() {
    osalMutexUnlock(&RNGD1.Lock);
}

#endif

#endif /* HAL_USE_STM32_RNG */
