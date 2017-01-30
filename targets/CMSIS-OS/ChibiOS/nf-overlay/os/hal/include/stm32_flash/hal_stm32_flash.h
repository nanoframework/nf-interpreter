//
// Copyright (c) 2017 The nano Framework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HAL_ST_FLASH_H
#define HAL_ST_FLASH_H

#if (HAL_USE_STM32_FLASH == TRUE)

#include "flash_lld.h"

///////////////////////////////////////////////////////////////////////////////
// Driver constants.                                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver pre-compile time settings.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Derived constants and error checks.                                       //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver data structures and types.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver macros.                                                            //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// External declarations.                                                    //
///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

  void stm32FlashInit(void);
  void stm32FlashObjectInit(SMT32FlashDriver* flash);
  void stm32FlashReadBytes(uint32_t startAddress, uint32_t length, uint8_t* buffer);
  bool stm32FlashWrite(uint32_t startAddress, uint32_t length, const uint8_t* buffer);
  bool stm32FlashIsErased(uint32_t startAddress, uint32_t length);
  bool stm32FlashErase(uint32_t address);

#ifdef __cplusplus
}
#endif

#endif // HAL_USE_STM32_FLASH

#endif // HAL_ST_FLASH_H
