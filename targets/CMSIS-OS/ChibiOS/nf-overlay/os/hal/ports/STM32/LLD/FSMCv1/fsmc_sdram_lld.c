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
/*
   SDRAM routines added by Nick Klimov aka progfin.
 */

/**
 * @file    hal_FSMC_sdram.c
 * @brief   SDRAM Driver subsystem low level driver source.
 *
 * @addtogroup SDRAM
 * @{
 */

#include "hal.h"

#if (defined(STM32F427xx) || defined(STM32F437xx) || \
     defined(STM32F429xx) || defined(STM32F439xx) || \
     defined(STM32F769xx))

#if (STM32_USE_FSMC_SDRAM == TRUE)

#include "FSMC_sdram_lld.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/**
 * FSMC_Command_Mode
 */
#define FSMCCM_NORMAL              ((uint32_t)0x00000000)
#define FSMCCM_CLK_ENABLED         ((uint32_t)0x00000001)
#define FSMCCM_PALL                ((uint32_t)0x00000002)
#define FSMCCM_AUTO_REFRESH        ((uint32_t)0x00000003)
#define FSMCCM_LOAD_MODE           ((uint32_t)0x00000004)
#define FSMCCM_SELFREFRESH         ((uint32_t)0x00000005)
#define FSMCCM_POWER_DOWN          ((uint32_t)0x00000006)

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/
/**
 * @brief   SDRAM driver identifier.
 */
SDRAMDriver SDRAMD;

/*===========================================================================*/
/* Driver local types.                                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief   Wait until the SDRAM controller is ready.
 *
 * @notapi
 */
static void _sdram_wait_ready(void) {
  /* Wait until the SDRAM controller is ready */
  while (SDRAMD.sdram->SDSR & FMC_SDSR_BUSY);
}

/**
 * @brief   Executes the SDRAM memory initialization sequence.
 *
 * @param[in] cfgp         pointer to the @p SDRAMConfig object
 *
 * @notapi
 */
static void _sdram_init_sequence(const SDRAMConfig *cfg)
{

  uint32_t command_target = 0;
  __IO uint32_t tmpr = 0;

#if STM32_SDRAM_USE_FSMC_SDRAM1
  command_target |= FMC_SDCMR_CTB1;
#endif
#if STM32_SDRAM_USE_FSMC_SDRAM2
  command_target |= FMC_SDCMR_CTB2;
#endif

  // Step 3: Configure a clock configuration enable command
  _sdram_wait_ready();

  tmpr = FSMCCM_CLK_ENABLED | command_target | 0 | 0;
  SDRAMD.sdram->SDCMR = tmpr;

  // Step 4: Insert delay (tipically 100uS)
  osalThreadSleepMilliseconds(2);

  // Step 5: Configure a PALL (precharge all) command
  _sdram_wait_ready();

  tmpr = FSMCCM_PALL | command_target | 0 | 0;
  SDRAMD.sdram->SDCMR = tmpr;

  // Step 6 Configure a Auto-Refresh command
  _sdram_wait_ready();

  tmpr  = FSMCCM_AUTO_REFRESH | command_target | ((8-1) << 5) | 0;
  SDRAMD.sdram->SDCMR = tmpr;

  // Step 7: Program the external memory mode register
  _sdram_wait_ready();

  tmpr = FSMCCM_LOAD_MODE | command_target | 0 | cfg->sdcmr;
  SDRAMD.sdram->SDCMR = tmpr;

  // Step 8: Set clock
  _sdram_wait_ready();

  SDRAMD.sdram->SDRTR |= cfg->sdrtr;

  _sdram_wait_ready();
}

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level SDRAM driver initialization.
 */
void fsmcSdramInit(void) {

  stm32FsmcInit();

  SDRAMD.sdram = FSMCD1.sdram;
  SDRAMD.state = SDRAM_STOP;
}

/**
 * @brief   Configures and activates the SDRAM peripheral.
 *
 * @param[in] sdramp        pointer to the @p SDRAMDriver object
 * @param[in] cfgp          pointer to the @p SDRAMConfig object
 */
void fsmcSdramStart(SDRAMDriver *sdram, const SDRAMConfig *cfg)
{
  uint32_t tmpr1 = 0;

  if (FSMCD1.state == FSMC_STOP)
    stm32FsmcStart(&FSMCD1);

  osalDbgAssert((sdram->state == SDRAM_STOP) || (sdram->state == SDRAM_READY),
              "SDRAM. Invalid state.");

  if (sdram->state == SDRAM_STOP)
  {
    
    // Initializes the FMC_SDRAM device according to the specified control parameters
    // load load current SDCR register value
    tmpr1 = sdram->sdram->SDCR[0];

    // Clear NC, NR, MWID, NB, CAS, WP, SDCLK, RBURST, and RPIPE bits
    tmpr1 &= ((uint32_t)~(FMC_SDCR1_NC  | FMC_SDCR1_NR | FMC_SDCR1_MWID | \
                          FMC_SDCR1_NB  | FMC_SDCR1_CAS | FMC_SDCR1_WP   | \
                          FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE));

    tmpr1 |= cfg->sdcr;

    sdram->sdram->SDCR[0] = tmpr1; 

    // Initializes the FMC_SDRAM device timing according to the specified parameters
    // load current SDTR register value
    tmpr1 = sdram->sdram->SDTR[0];

    // Clear TMRD, TXSR, TRAS, TRC, TWR, TRP and TRCD bits
    tmpr1 &= ((uint32_t)~(FMC_SDTR1_TMRD  | FMC_SDTR1_TXSR | FMC_SDTR1_TRAS | \
                          FMC_SDTR1_TRC  | FMC_SDTR1_TWR | FMC_SDTR1_TRP | \
                          FMC_SDTR1_TRCD));

    tmpr1 |= cfg->sdtr;

    sdram->sdram->SDTR[0] = tmpr1;

    _sdram_init_sequence(cfg);

    sdram->state = SDRAM_READY;
  }
}

/**
 * @brief   Deactivates the SDRAM peripheral.
 *
 * @param[in] sdramp         pointer to the @p SDRAMDriver object
 *
 * @notapi
 */
void fsmcSdramStop(SDRAMDriver *sdram) {

  uint32_t command_target = 0;

#if STM32_SDRAM_USE_FSMC_SDRAM1
  command_target |= FMC_SDCMR_CTB1;
#endif
#if STM32_SDRAM_USE_FSMC_SDRAM2
  command_target |= FMC_SDCMR_CTB2;
#endif

  if (sdram->state == SDRAM_READY) {
    SDRAMD.sdram->SDCMR = FSMCCM_POWER_DOWN | command_target;
    sdram->state = SDRAM_STOP;
  }
}

#endif /* STM32_USE_FSMC_SDRAM */

#endif /* STM32F427xx / STM32F429xx / STM32F437xx / STM32F439xx */
