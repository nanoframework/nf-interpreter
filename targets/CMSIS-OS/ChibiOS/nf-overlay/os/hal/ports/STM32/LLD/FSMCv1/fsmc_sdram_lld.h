//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2014 Uladzimir Pylinsky aka barthess and Nick Klimov aka progfin
// See LICENSE file in the project root for full license information.
//

#ifndef HAL_FMC_SDRAM_H_
#define HAL_FMC_SDRAM_H_

#include <hal_nf_community.h>

#if (defined(STM32F427xx) || defined(STM32F437xx) || \
     defined(STM32F429xx) || defined(STM32F439xx) || \
	 defined(STM32F469xx) || defined(STM32F479xx) || \
     defined(STM32F745xx) || defined(STM32F746xx) || \
     defined(STM32F756xx) || defined(STM32F767xx) || \
     defined(STM32F769xx) || defined(STM32F777xx) || \
     defined(STM32F779xx))

#include "hal_stm32_fsmc.h"

#if (STM32_USE_FSMC_SDRAM == TRUE)

///////////////////////////////////////////////////////////////////////////////
// Driver constants.                                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver pre-compile time settings.                                         //
///////////////////////////////////////////////////////////////////////////////
//  Configuration options

// SDRAM driver enable switch.
// If set to @p TRUE the support for SDRAM1 is included.
#if !defined(STM32_SDRAM_USE_FSMC_SDRAM1)
#define STM32_SDRAM_USE_FSMC_SDRAM1                  FALSE
#else
#define STM32_SDRAM1_MAP_BASE                        FSMC_Bank5_MAP_BASE
#endif

// SDRAM driver enable switch.
// If set to @p TRUE the support for SDRAM2 is included.
#if !defined(STM32_SDRAM_USE_FSMC_SDRAM2)
#define STM32_SDRAM_USE_FSMC_SDRAM2                  FALSE
#else
#define STM32_SDRAM2_MAP_BASE                        FSMC_Bank6_MAP_BASE
#endif

///////////////////////////////////////////////////////////////////////////////
// Derived constants and error checks.                                       //
///////////////////////////////////////////////////////////////////////////////

#if !STM32_SDRAM_USE_FSMC_SDRAM1 && !STM32_SDRAM_USE_FSMC_SDRAM2
#error "SDRAM driver activated but no SDRAM peripheral assigned"
#endif

#if (STM32_SDRAM_USE_FSMC_SDRAM1 || STM32_SDRAM_USE_FSMC_SDRAM2) && !STM32_HAS_FSMC
#error "FMC not present in the selected device"
#endif

///////////////////////////////////////////////////////////////////////////////
// Driver data structures and types.                                         //
///////////////////////////////////////////////////////////////////////////////

// Driver state machine possible states.
typedef enum 
{
  SDRAM_UNINIT = 0,                   //*< Not initialized.                
  SDRAM_STOP = 1,                     //*< Stopped.                        
  SDRAM_READY = 2,                    //*< Ready.                          
} sdramstate_t;

// Type of a structure representing an SDRAM driver.
typedef struct SDRAMDriver SDRAMDriver;

// Driver configuration structure.
// It could be empty on some architectures.
typedef struct {
  // SDRAM control register.
  // Its value will be used for both banks.
  uint32_t      sdcr;

  // SDRAM timing register.
  // Its value will be used for both banks.
  uint32_t      sdtr;

  // SDRAM command mode register.
  // Only its MRD and NRFS bits will be used.
  uint32_t      sdcmr;

  // SDRAM refresh timer register.
  // Only its COUNT bits will be used.
  uint32_t      sdrtr;

} SDRAMConfig;

// Structure representing an SDRAM driver.
struct SDRAMDriver {
  // Driver state.
  sdramstate_t              state;

  // Pointer to the FMC SDRAM registers block.
  FSMC_SDRAM_TypeDef        *sdram;
};

///////////////////////////////////////////////////////////////////////////////
// Driver macros.                                                            //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// External declarations.                                                    //
///////////////////////////////////////////////////////////////////////////////

extern SDRAMDriver SDRAMD;

#ifdef __cplusplus
extern "C" {
#endif

  void fsmcSdramInit(void);
  void fsmcSdramStart(SDRAMDriver *sdramp, const SDRAMConfig *cfgp);
  void fsmcSdramStop(SDRAMDriver *sdramp);

#ifdef __cplusplus
}
#endif

#endif // STM32_USE_FSMC_SDRAM 

#endif //STM32F427xx / STM32F429xx / STM32F437xx / STM32F439xx / STM32F469xx / STM32F479xx / STM32F745xx / STM32F746xx 
       // STM32F756xx / STM32F767xx / STM32F769xx / STM32F777xx / STM32F779xx

#endif // HAL_FMC_SDRAM_H_
