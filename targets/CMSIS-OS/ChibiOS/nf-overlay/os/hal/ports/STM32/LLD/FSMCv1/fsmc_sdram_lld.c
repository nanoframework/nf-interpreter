//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2014 Uladzimir Pylinsky aka barthess and Nick Klimov aka progfin
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_nf_community.h>

#if (defined(STM32F427xx) || defined(STM32F437xx) || \
     defined(STM32F429xx) || defined(STM32F439xx) || \
	 defined(STM32F469xx) || defined(STM32F479xx) || \
     defined(STM32F745xx) || defined(STM32F746xx) || \
     defined(STM32F756xx) || defined(STM32F767xx) || \
     defined(STM32F769xx) || defined(STM32F777xx) || \
     defined(STM32F779xx))

#if (STM32_USE_FSMC_SDRAM == TRUE)

#include "fsmc_sdram_lld.h"

///////////////////////////////////////////////////////////////////////////////
// Driver local definitions.                                                 //
///////////////////////////////////////////////////////////////////////////////

// FMC_Command_Mode
#define FMCCM_NORMAL              ((uint32_t)0x00000000)
#define FMCCM_CLK_ENABLED         ((uint32_t)0x00000001)
#define FMCCM_PALL                ((uint32_t)0x00000002)
#define FMCCM_AUTO_REFRESH        ((uint32_t)0x00000003)
#define FMCCM_LOAD_MODE           ((uint32_t)0x00000004)
#define FMCCM_SELFREFRESH         ((uint32_t)0x00000005)
#define FMCCM_POWER_DOWN          ((uint32_t)0x00000006)

///////////////////////////////////////////////////////////////////////////////
// Driver exported variables.                                                //
///////////////////////////////////////////////////////////////////////////////
//  SDRAM driver identifier.
SDRAMDriver SDRAMD;

///////////////////////////////////////////////////////////////////////////////
// Driver local variables and types.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver local functions.                                                   //
///////////////////////////////////////////////////////////////////////////////

//   Wait until the SDRAM controller is ready.
static void _sdram_wait_ready(void) {
  // Wait until the SDRAM controller is ready 
  while (SDRAMD.sdram->SDSR & FMC_SDSR_BUSY);
}

//  Executes the SDRAM memory initialization sequence.
// cfgp         pointer to the @p SDRAMConfig object
static void _sdram_init_sequence(const SDRAMConfig *cfgp) {

  uint32_t command_target = 0;

#if STM32_SDRAM_USE_FSMC_SDRAM1
  command_target |= FMC_SDCMR_CTB1;
#endif
#if STM32_SDRAM_USE_FSMC_SDRAM2
  command_target |= FMC_SDCMR_CTB2;
#endif

  // Step 3: Configure a clock configuration enable command.
  _sdram_wait_ready();
  SDRAMD.sdram->SDCMR = FMCCM_CLK_ENABLED | command_target;

  // Step 4: Insert delay (tipically 100uS).
  osalThreadSleepMilliseconds(1);

  // Step 5: Configure a PALL (precharge all) command.
  _sdram_wait_ready();
  SDRAMD.sdram->SDCMR = FMCCM_PALL | command_target;

  // Step 6.1: Configure a Auto-Refresh command: send the first command.
  _sdram_wait_ready();
  SDRAMD.sdram->SDCMR = FMCCM_AUTO_REFRESH | command_target |
      (cfgp->sdcmr & FMC_SDCMR_NRFS);

  // Step 6.2: Send the second command.
  _sdram_wait_ready();
  SDRAMD.sdram->SDCMR = FMCCM_AUTO_REFRESH | command_target |
      (cfgp->sdcmr & FMC_SDCMR_NRFS);

  // Step 7: Program the external memory mode register.
  _sdram_wait_ready();
  SDRAMD.sdram->SDCMR = FMCCM_LOAD_MODE | command_target |
      (cfgp->sdcmr & FMC_SDCMR_MRD);

  // Step 8: Set clock.
  _sdram_wait_ready();
  SDRAMD.sdram->SDRTR = cfgp->sdrtr & FMC_SDRTR_COUNT;

  _sdram_wait_ready();
}

///////////////////////////////////////////////////////////////////////////////
// Driver interrupt handlers.                                                //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver exported functions.                                                //
///////////////////////////////////////////////////////////////////////////////

//   Low level SDRAM driver initialization.
 
void fsmcSdramInit(void) {

  stm32FsmcInit();

  SDRAMD.sdram = FSMCD1.sdram;
  SDRAMD.state = SDRAM_STOP;
}

//   Configures and activates the SDRAM peripheral.
// sdramp        pointer to the @p SDRAMDriver object
// cfgp          pointer to the @p SDRAMConfig object
void fsmcSdramStart(SDRAMDriver *sdramp, const SDRAMConfig *cfgp) {

  if (FSMCD1.state == FSMC_STOP)
    fsmc_start(&FSMCD1);

  osalDbgAssert((sdramp->state == SDRAM_STOP) || (sdramp->state == SDRAM_READY),
              "SDRAM. Invalid state.");

  if (sdramp->state == SDRAM_STOP) {

    // Even if you need only bank2 you must properly set up SDCR and SDTR
    //   regitsters for bank1 too. Both banks will be tuned equally assuming
    //   connected memory ICs are equal.
    sdramp->sdram->SDCR1 = cfgp->sdcr;
    sdramp->sdram->SDTR1 = cfgp->sdtr;
    sdramp->sdram->SDCR2 = cfgp->sdcr;
    sdramp->sdram->SDTR2 = cfgp->sdtr;

    _sdram_init_sequence(cfgp);

    sdramp->state = SDRAM_READY;
  }
}

//   Deactivates the SDRAM peripheral.
// sdramp         pointer to the @p SDRAMDriver object
void fsmcSdramStop(SDRAMDriver *sdramp) {

  uint32_t command_target = 0;

#if STM32_SDRAM_USE_FSMC_SDRAM1
  command_target |= FMC_SDCMR_CTB1;
#endif
#if STM32_SDRAM_USE_FSMC_SDRAM2
  command_target |= FMC_SDCMR_CTB2;
#endif

  if (sdramp->state == SDRAM_READY) {
    SDRAMD.sdram->SDCMR = FMCCM_POWER_DOWN | command_target;
    sdramp->state = SDRAM_STOP;
  }
}

#endif // STM32_USE_FSMC_SDRAM 

#endif //STM32F427xx / STM32F429xx / STM32F437xx / STM32F439xx / STM32F469xx / STM32F479xx / STM32F745xx / STM32F746xx 
       // STM32F756xx / STM32F767xx / STM32F769xx / STM32F777xx / STM32F779xx
