//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "hal.h"

#if (HAL_USE_STM32_FLASH == TRUE)

///////////////////////////////////////////////////////////////////////////////
// Driver local definitions.                                                 //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver exported variables.                                                //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver local variables and types.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver local functions.                                                   //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Driver exported functions.                                                //
///////////////////////////////////////////////////////////////////////////////

void stm32FlashInit(void) {
  flash_lld_init();
}

void stm32FlashObjectInit(SMT32FlashDriver* flash) {

}


void stm32FlashReadBytes(uint32_t startAddress, uint32_t length, uint8_t* buffer) {
  osalDbgCheck((startAddress > FLASH_BASE) && (length > 0U) && (buffer != NULL));

  flash_lld_readBytes(startAddress, length, buffer);
}

bool stm32FlashWrite(uint32_t startAddress, uint32_t length, const uint8_t* buffer) {
  osalDbgCheck((startAddress > FLASH_BASE) && (length > 0U) && (buffer != NULL));

  return flash_lld_write(startAddress, length, buffer);
}

bool stm32FlashIsErased(uint32_t startAddress, uint32_t length) {
  osalDbgCheck((startAddress > FLASH_BASE) && (length > 0U));

  return flash_lld_isErased(startAddress, length);
}

bool stm32FlashErase(uint32_t address) {
  osalDbgCheck(address > FLASH_BASE);

  return flash_lld_erase(address);
}

uint8_t stm32FlashGetSector(uint32_t address) {
  return flash_lld_getSector(address);
}


#endif

