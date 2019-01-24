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

/** 
  * @brief  HAL Status structures definition  
  */  
typedef enum 
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

///////////////////////////////////////////////////////////////////////////////
// Driver macros.                                                            //
///////////////////////////////////////////////////////////////////////////////

// From STMicroelectronics Cube HAL 
#define FLASH                       ((FLASH_TypeDef *) FLASH_R_BASE)
#define HAL_IS_BIT_SET(REG, BIT)    (((REG) & (BIT)) != RESET)

#define FLASH_PEKEY1               ((uint32_t)0x89ABCDEFU) /*!< Flash program erase key1 */
#define FLASH_PEKEY2               ((uint32_t)0x02030405U) /*!< Flash program erase key: used with FLASH_PEKEY2
                                                               to unlock the write access to the FLASH_PECR register and
                                                               data EEPROM */

#define FLASH_PRGKEY1              ((uint32_t)0x8C9DAEBFU) /*!< Flash program memory key1 */
#define FLASH_PRGKEY2              ((uint32_t)0x13141516U) /*!< Flash program memory key2: used with FLASH_PRGKEY2
                                                               to unlock the program memory */
#define FLASH_TIMEOUT_VALUE      (50000U) /* 50 s */
#define FLASH_SIZE_DATA_REGISTER FLASHSIZE_BASE

#define FLASH_SIZE                (uint32_t)((*((uint32_t *)FLASHSIZE_BASE)&0xFFFF) * 1024U)
#define FLASH_PAGE_SIZE           ((uint32_t)128U)  /*!< FLASH Page Size in bytes */

/**
  * @brief  Get the specified FLASH flag status. 
  * @param  __FLAG__ specifies the FLASH flag to check.
  *          This parameter can be one of the following values:
  *            @arg @ref FLASH_FLAG_BSY         FLASH Busy flag
  *            @arg @ref FLASH_FLAG_EOP         FLASH End of Operation flag 
  *            @arg @ref FLASH_FLAG_ENDHV       FLASH End of High Voltage flag
  *            @arg @ref FLASH_FLAG_READY       FLASH Ready flag after low power mode
  *            @arg @ref FLASH_FLAG_PGAERR      FLASH Programming Alignment error flag
  *            @arg @ref FLASH_FLAG_SIZERR      FLASH Size error flag
  *            @arg @ref FLASH_FLAG_OPTVERR     FLASH Option validity error flag (not valid with STM32L031xx/STM32L041xx)
  *            @arg @ref FLASH_FLAG_RDERR       FLASH Read protected error flag
  *            @arg @ref FLASH_FLAG_WRPERR      FLASH Write protected error flag 
  *            @arg @ref FLASH_FLAG_FWWERR      FLASH Fetch While Write Error flag
  *            @arg @ref FLASH_FLAG_NOTZEROERR  Not Zero area error flag  
  * @retval The new state of __FLAG__ (SET or RESET).
  */
#define __HAL_FLASH_GET_FLAG(__FLAG__)   (((FLASH->SR) & (__FLAG__)) == (__FLAG__))

/**
  * @brief  Clear the specified FLASH flag.
  * @param  __FLAG__ specifies the FLASH flags to clear.
  *          This parameter can be any combination of the following values:
  *            @arg @ref FLASH_FLAG_EOP         FLASH End of Operation flag 
  *            @arg @ref FLASH_FLAG_PGAERR      FLASH Programming Alignment error flag
  *            @arg @ref FLASH_FLAG_SIZERR      FLASH Size error flag
  *            @arg @ref FLASH_FLAG_OPTVERR     FLASH Option validity error flag (not valid with STM32L031xx/STM32L041xx)
  *            @arg @ref FLASH_FLAG_RDERR       FLASH Read protected error flag
  *            @arg @ref FLASH_FLAG_WRPERR      FLASH Write protected error flag 
  *            @arg @ref FLASH_FLAG_FWWERR      FLASH Fetch While Write Error flag
  *            @arg @ref FLASH_FLAG_NOTZEROERR  Not Zero area error flag  
  * @retval none
  */
#define __HAL_FLASH_CLEAR_FLAG(__FLAG__)   ((FLASH->SR) = (__FLAG__))

/** @defgroup FLASH_Flags FLASH Flags 
  * @{
  */ 

#define FLASH_FLAG_BSY             FLASH_SR_BSY        /*!< FLASH Busy flag */
#define FLASH_FLAG_EOP             FLASH_SR_EOP        /*!< FLASH End of Programming flag */
#define FLASH_FLAG_ENDHV           FLASH_SR_HVOFF      /*!< FLASH End of High Voltage flag */
#define FLASH_FLAG_READY           FLASH_SR_READY      /*!< FLASH Ready flag after low power mode */
#define FLASH_FLAG_WRPERR          FLASH_SR_WRPERR     /*!< FLASH Write protected error flag */
#define FLASH_FLAG_PGAERR          FLASH_SR_PGAERR     /*!< FLASH Programming Alignment error flag */
#define FLASH_FLAG_SIZERR          FLASH_SR_SIZERR     /*!< FLASH Size error flag  */
#define FLASH_FLAG_OPTVERR         FLASH_SR_OPTVERR    /*!< FLASH Option Validity error flag  */
#define FLASH_FLAG_RDERR           FLASH_SR_RDERR      /*!< FLASH Read protected error flag */
#define FLASH_FLAG_FWWERR          FLASH_SR_FWWERR     /*!< FLASH Write or Errase operation aborted */
#define FLASH_FLAG_NOTZEROERR      FLASH_SR_NOTZEROERR /*!< FLASH Read protected error flag */

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
