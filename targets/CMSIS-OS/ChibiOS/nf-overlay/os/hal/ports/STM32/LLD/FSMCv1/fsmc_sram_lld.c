//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2014 Uladzimir Pylinsky aka barthess
// See LICENSE file in the project root for full license information.
//

#include <hal.h>
#include <hal_nf_community.h>
#include "fsmc_sram_lld.h"

#if (STM32_USE_FSMC_SRAM == TRUE)

///////////////////////////////////////////////////////////////////////////////
// Driver local definitions.                                                 //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver exported variables.                                                //
///////////////////////////////////////////////////////////////////////////////
//   SRAM1 driver identifier.
#if STM32_SRAM_USE_FSMC_SRAM1
SRAMDriver SRAMD1;
#endif

//   SRAM2 driver identifier.
#if STM32_SRAM_USE_FSMC_SRAM2
SRAMDriver SRAMD2;
#endif

//   SRAM3 driver identifier.
#if STM32_SRAM_USE_FSMC_SRAM3
SRAMDriver SRAMD3;
#endif

//   SRAM4 driver identifier.
#if STM32_SRAM_USE_FSMC_SRAM4
SRAMDriver SRAMD4;
#endif

///////////////////////////////////////////////////////////////////////////////
// Driver local variables and types.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver local functions.                                                   //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver interrupt handlers.                                                //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver exported functions.                                                //
///////////////////////////////////////////////////////////////////////////////

//   Low level SRAM driver initialization.
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

//   Configures and activates the SRAM peripheral.
// sramp         pointer to the @p SRAMDriver object
// cfgp          pointer to the @p SRAMConfig object
void fsmcSramStart(SRAMDriver *sramp, const SRAMConfig *cfgp) {

  if (FSMCD1.state == FSMC_STOP)
    fsmc_start(&FSMCD1);

  osalDbgAssert((sramp->state == SRAM_STOP) || (sramp->state == SRAM_READY),
              "invalid state");

  if (sramp->state == SRAM_STOP) {
    sramp->sram->BTR  = cfgp->btr;
    sramp->sram->BWTR = cfgp->bwtr;
    sramp->sram->BCR  = cfgp->bcr | FSMC_BCR_MBKEN;
    sramp->state = SRAM_READY;
  }
}

//   Deactivates the SRAM peripheral.
// sramp         pointer to the @p SRAMDriver object
void fsmcSramStop(SRAMDriver *sramp) {

  if (sramp->state == SRAM_READY) {
    uint32_t mask = FSMC_BCR_MBKEN;

#if (defined(STM32F427xx) || defined(STM32F437xx) || \
     defined(STM32F429xx) || defined(STM32F439xx) || \
	 defined(STM32F469xx) || defined(STM32F479xx) || \
     defined(STM32F745xx) || defined(STM32F746xx) || \
     defined(STM32F756xx) || defined(STM32F767xx) || \
     defined(STM32F769xx) || defined(STM32F777xx) || \
     defined(STM32F779xx))
    mask |= FSMC_BCR_CCLKEN;
#endif

    sramp->sram->BCR &= ~mask;
    sramp->state = SRAM_STOP;
  }
}

#endif // STM32_USE_FSMC_SRAM */
