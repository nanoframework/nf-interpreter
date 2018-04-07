/*
    ChibiOS/HAL - Copyright (C) 2014 Uladzimir Pylinsky aka barthess

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    hal_FSMC_sram.c
 * @brief   SRAM Driver subsystem low level driver source.
 *
 * @addtogroup SRAM
 * @{
 */
#include "hal.h"
#include "FSMC_sram_lld.h"

#if (STM32_USE_FSMC_SRAM == TRUE)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/
/**
 * @brief   SRAM1 driver identifier.
 */
#if STM32_SRAM_USE_FSMC_SRAM1
SRAMDriver SRAMD1;
#endif

/**
 * @brief   SRAM2 driver identifier.
 */
#if STM32_SRAM_USE_FSMC_SRAM2
SRAMDriver SRAMD2;
#endif

/**
 * @brief   SRAM3 driver identifier.
 */
#if STM32_SRAM_USE_FSMC_SRAM3
SRAMDriver SRAMD3;
#endif

/**
 * @brief   SRAM4 driver identifier.
 */
#if STM32_SRAM_USE_FSMC_SRAM4
SRAMDriver SRAMD4;
#endif

/*===========================================================================*/
/* Driver local types.                                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

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
 * @brief   Low level SRAM driver initialization.
 *
 * @notapi
 */
void fsmcSramInit(void) {

  stm32FsmcInit();

#if STM32_SRAM_USE_FSMC_SRAM1
  SRAMD1.sram = FSMCD1.sram1;
  SRAMD1.state = SRAM_STOP;
#endif /* STM32_SRAM_USE_FSMC_SRAM1 */

#if STM32_SRAM_USE_FSMC_SRAM2
  SRAMD2.sram = FSMCD1.sram2;
  SRAMD2.state = SRAM_STOP;
#endif /* STM32_SRAM_USE_FSMC_SRAM2 */

#if STM32_SRAM_USE_FSMC_SRAM3
  SRAMD3.sram = FSMCD1.sram3;
  SRAMD3.state = SRAM_STOP;
#endif /* STM32_SRAM_USE_FSMC_SRAM3 */

#if STM32_SRAM_USE_FSMC_SRAM4
  SRAMD4.sram = FSMCD1.sram4;
  SRAMD4.state = SRAM_STOP;
#endif /* STM32_SRAM_USE_FSMC_SRAM4 */
}

/**
 * @brief   Configures and activates the SRAM peripheral.
 *
 * @param[in] sramp         pointer to the @p SRAMDriver object
 * @param[in] cfgp          pointer to the @p SRAMConfig object
 *
 * @notapi
 */
void fsmcSramStart(SRAMDriver *sramp, const SRAMConfig *cfg) {

  if (FSMCD1.state == FSMC_STOP)
    stm32FsmcStart(&FSMCD1);

  osalDbgAssert((sramp->state == SRAM_STOP) || (sramp->state == SRAM_READY),
              "invalid state");

  if (sramp->state == SRAM_STOP) {
    sramp->sram->BTR  = cfg->btr;
    sramp->sram->BWTR = cfg->bwtr;
    sramp->sram->BCR  = cfg->bcr | FSMC_BCR_MBKEN;
    sramp->state = SRAM_READY;
  }
}

/**
 * @brief   Deactivates the SRAM peripheral.
 *
 * @param[in] sramp         pointer to the @p SRAMDriver object
 *
 * @notapi
 */
void fsmcSramStop(SRAMDriver *sram) {

  if (sram->state == SRAM_READY) {
    uint32_t mask = FSMC_BCR_MBKEN;
#if (defined(STM32F427xx) || defined(STM32F437xx) || \
     defined(STM32F429xx) || defined(STM32F439xx) || \
     defined(STM32F7))
    mask |= FSMC_BCR_CCLKEN;
#endif
    sram->sram->BCR &= ~mask;
    sram->state = SRAM_STOP;
  }
}

#endif /* STM32_USE_FSMC_SRAM */
