//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) 2014 Uladzimir Pylinsky aka barthess
// See LICENSE file in the project root for full license information.
//

#ifndef HAL_FSMC_SRAM_H_
#define HAL_FSMC_SRAM_H_

#include "hal_stm32_fsmc.h"
#include <hal_nf_community.h>

#if (STM32_USE_FSMC_SRAM == TRUE)

///////////////////////////////////////////////////////////////////////////////
// Driver constants.                                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver pre-compile time settings.                                         //
///////////////////////////////////////////////////////////////////////////////

// SRAM driver enable switch.
// If set to @p TRUE the support for SRAM1 is included.
#if !defined(STM32_SRAM_USE_FSMC_SRAM1)
#define STM32_SRAM_USE_FSMC_SRAM1                  FALSE
#endif

// SRAM driver enable switch.
// If set to @p TRUE the support for SRAM2 is included.
#if !defined(STM32_SRAM_USE_FSMC_SRAM2)
#define STM32_SRAM_USE_FSMC_SRAM2                  FALSE
#endif

// SRAM driver enable switch.
// If set to @p TRUE the support for SRAM3 is included.
#if !defined(STM32_SRAM_USE_FSMC_SRAM3)
#define STM32_SRAM_USE_FSMC_SRAM3                  FALSE
#endif

// SRAM driver enable switch.
// If set to @p TRUE the support for SRAM4 is included.
#if !defined(STM32_SRAM_USE_FSMC_SRAM4)
#define STM32_SRAM_USE_FSMC_SRAM4                  FALSE
#endif

///////////////////////////////////////////////////////////////////////////////
// Derived constants and error checks.                                       //
///////////////////////////////////////////////////////////////////////////////

#if !STM32_SRAM_USE_FSMC_SRAM1 && !STM32_SRAM_USE_FSMC_SRAM2 && \
    !STM32_SRAM_USE_FSMC_SRAM3 && !STM32_SRAM_USE_FSMC_SRAM4
#error "SRAM driver activated but no SRAM peripheral assigned"
#endif

#if (STM32_SRAM_USE_FSMC_SRAM1 || STM32_SRAM_USE_FSMC_SRAM2 || \
    STM32_SRAM_USE_FSMC_SRAM3 || STM32_SRAM_USE_FSMC_SRAM4) && !STM32_HAS_FSMC
#error "FSMC not present in the selected device"
#endif

///////////////////////////////////////////////////////////////////////////////
// Driver data structures and types.                                         //
///////////////////////////////////////////////////////////////////////////////

// Driver state machine possible states.
typedef enum {
  SRAM_UNINIT = 0,                   // Not initialized
  SRAM_STOP = 1,                     // Stopped
  SRAM_READY = 2,                    // Ready
} sramstate_t;

// Type of a structure representing an NAND driver.
typedef struct SRAMDriver SRAMDriver;

//  Driver configuration structure.
// It could be empty on some architectures.
//  Some bits in BCR register will be forced by driver.
typedef struct {
  uint32_t  bcr;
  uint32_t  btr;
  uint32_t  bwtr;
} SRAMConfig;

//  Structure representing an NAND driver.
struct SRAMDriver {
  // Driver state
  sramstate_t               state;
  
  // Pointer to the FSMC SRAM registers block. 
  FSMC_SRAM_NOR_TypeDef     *sram;
};

///////////////////////////////////////////////////////////////////////////////
// Driver macros.                                                            //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// External declarations.                                                    //
///////////////////////////////////////////////////////////////////////////////

#if STM32_SRAM_USE_FSMC_SRAM1 && !defined(__DOXYGEN__)
extern SRAMDriver SRAMD1;
#endif

#if STM32_SRAM_USE_FSMC_SRAM2 && !defined(__DOXYGEN__)
extern SRAMDriver SRAMD2;
#endif

#if STM32_SRAM_USE_FSMC_SRAM3 && !defined(__DOXYGEN__)
extern SRAMDriver SRAMD3;
#endif

#if STM32_SRAM_USE_FSMC_SRAM4 && !defined(__DOXYGEN__)
extern SRAMDriver SRAMD4;
#endif

#ifdef __cplusplus
extern "C" {
#endif

  void fsmcSramInit(void);
  void fsmcSramStart(SRAMDriver *sramp, const SRAMConfig *cfgp);
  void fsmcSramStop(SRAMDriver *sramp);

#ifdef __cplusplus
}
#endif

#endif // STM32_USE_FSMC_SRAM 

#endif // HAL_FSMC_SRAM_H_ 
