//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef FLASH_LLD_H
#define FLASH_LLD_H

#include "stm32_registry.h"
#include <hal_nf_community.h>

#if (HAL_USE_STM32_FLASH == TRUE)

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

// From STMicroelectronics Cube HAL 
// HAL Status structures definition  
typedef struct SMT32FlashDriver {
  
  // pointer to the FLASH Registers 
  FLASH_TypeDef               *flash;

}SMT32FlashDriver;

///////////////////////////////////////////////////////////////////////////////
// Driver macros.                                                            //
///////////////////////////////////////////////////////////////////////////////

// From STMicroelectronics Cube HAL 
#define FLASH                       ((FLASH_TypeDef *) FLASH_R_BASE)
#define HAL_IS_BIT_SET(REG, BIT)    (((REG) & (BIT)) != RESET)

#define FLASH_FLAG_EOP                 FLASH_SR_EOP            /*!< FLASH End of Operation flag               */
#define FLASH_FLAG_OPERR               FLASH_SR_SOP            /*!< FLASH operation Error flag                */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPERR         /*!< FLASH Write protected error flag          */
#define FLASH_FLAG_PGAERR              FLASH_SR_PGAERR         /*!< FLASH Programming Alignment error flag    */
#define FLASH_FLAG_PGPERR              FLASH_SR_PGPERR         /*!< FLASH Programming Parallelism error flag  */
#define FLASH_FLAG_PGSERR              FLASH_SR_PGSERR         /*!< FLASH Programming Sequence error flag     */
#if defined(FLASH_SR_RDERR)
#define FLASH_FLAG_RDERR               FLASH_SR_RDERR          /*!< Read Protection error flag (PCROP)        */
#endif /* FLASH_SR_RDERR */
#define FLASH_FLAG_BSY                 FLASH_SR_BSY            /*!< FLASH Busy flag                           */ 

// FLASH_Program_Parallelism FLASH Program Parallelism
#define FLASH_PSIZE_BYTE           ((uint32_t)0x00000000U)
#define FLASH_PSIZE_HALF_WORD      ((uint32_t)0x00000100U)
#define FLASH_PSIZE_WORD           ((uint32_t)0x00000200U)
#define FLASH_PSIZE_DOUBLE_WORD    ((uint32_t)0x00000300U)
#define CR_PSIZE_MASK              ((uint32_t)0xFFFFFCFFU)

#define __HAL_FLASH_GET_FLAG(__FLAG__)   ((FLASH->SR & (__FLAG__)))
#define __HAL_FLASH_CLEAR_FLAG(__FLAG__)   (FLASH->SR = (__FLAG__))

#define F0_SERIES_SECTOR_SIZE       ((uint32_t)0x00001000U)

///////////////////////////////////////////////////////////////////////////////
// External declarations.                                                    //
///////////////////////////////////////////////////////////////////////////////

#if STM32FLASH_USE_FLASH
SMT32FlashDriver STM32FLASH;
#endif

#ifdef __cplusplus
extern "C" {
#endif
  
  void flash_lld_init();
  void flash_lld_readBytes(uint32_t startAddress, uint32_t length, uint8_t* buffer);
  int  flash_lld_write(uint32_t startAddress, uint32_t length, const uint8_t* buffer);
  int  flash_lld_isErased(uint32_t startAddress, uint32_t length);
  int  flash_lld_erase(uint32_t address);
  uint8_t flash_lld_getSector(uint32_t address);

#ifdef __cplusplus
}
#endif

#endif // HAL_USE_STM32_FLASH

#endif // FLASH_LLD_H
