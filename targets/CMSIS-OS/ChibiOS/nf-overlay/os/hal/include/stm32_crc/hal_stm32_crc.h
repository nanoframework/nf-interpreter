//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HAL_NF_STM32_CRC_H
#define HAL_NF_STM32_CRC_H

#if (HAL_NF_USE_STM32_CRC == TRUE)

///////////////////////////////////////////////////////////////////////////////
// Driver constants.                                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver pre-compile time settings.                                         //
///////////////////////////////////////////////////////////////////////////////

// Enables the crcAcquireModule() and crcReleaseModule() APIs.
// Disabling this option saves both code and data space.
#if !defined(CRC_USE_MUTUAL_EXCLUSION)
#define CRC_USE_MUTUAL_EXCLUSION        TRUE
#endif

///////////////////////////////////////////////////////////////////////////////
// Derived constants and error checks.                                       //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver data structures and types.                                         //
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief   Driver state machine possible states.
 */
typedef enum {
  CRC_UNINIT,          // Not initialized
  CRC_STOP,            // Stopped
  CRC_READY,           // Ready
  CRC_ACTIVE           // Calculating CRC
} crcState;

#include "crc_lld.h"

///////////////////////////////////////////////////////////////////////////////
// Driver macros.                                                            //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// External declarations.                                                    //
///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

  void crcInit(void);
  void crcStart(const crcConfig *config);
  void crcStop();
  void crcReset();
  uint32_t crcCompute(const void* buffer, const uint32_t size, const uint32_t initialCrc);

#if (CRC_USE_MUTUAL_EXCLUSION == TRUE)
  void crcAquireModule();
  void crcReleaseModule();
#endif

#ifdef __cplusplus
}
#endif

#endif // HAL_NF_USE_STM32_CRC

#endif // HAL_NF_STM32_CRC_H
