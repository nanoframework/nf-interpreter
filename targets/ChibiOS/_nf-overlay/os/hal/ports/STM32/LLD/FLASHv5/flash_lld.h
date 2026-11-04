//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef FLASH_LLD_H
#define FLASH_LLD_H

#include <stm32_registry.h>
#include <hal_nf_community.h>

#if (HAL_NF_USE_STM32_FLASH == TRUE)

///////////////////////////////////////////////////////////////////////////////
// Driver constants.                                                         //
///////////////////////////////////////////////////////////////////////////////

// size of the flash word (minimum programming unit) in bytes
#define STM32_FLASH_LINE_SIZE (FLASH_NB_32BITWORD_IN_FLASHWORD * 4U)
#define STM32_FLASH_LINE_MASK (STM32_FLASH_LINE_SIZE - 1U)

///////////////////////////////////////////////////////////////////////////////
// Driver pre-compile time settings.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Derived constants and error checks.                                       //
///////////////////////////////////////////////////////////////////////////////

#if !defined(STM32H7XX)
#error "This flash driver supports only STM32H7 series"
#endif

// dual bank devices are not supported (yet)
#if defined(DUAL_BANK)
#error "Dual bank STM32H7 devices are not supported by this driver"
#endif

///////////////////////////////////////////////////////////////////////////////
// Driver data structures and types.                                         //
///////////////////////////////////////////////////////////////////////////////

typedef struct STM32FlashDriver
{

    // pointer to the FLASH Registers
    FLASH_TypeDef *flash;

} STM32FlashDriver;

///////////////////////////////////////////////////////////////////////////////
// Driver macros.                                                            //
///////////////////////////////////////////////////////////////////////////////

// From STMicroelectronics Cube HAL
#define FLASH_KEY1 ((uint32_t)0x45670123U)
#define FLASH_KEY2 ((uint32_t)0xCDEF89ABU)
#define FLASH_OPT_KEY1 ((uint32_t)0x08192A3BU)
#define FLASH_OPT_KEY2 ((uint32_t)0x4C5D6E7FU)

#define FLASH_FLAG_ALL_ERRORS_BANK1                                                                                    \
    (FLASH_SR_WRPERR | FLASH_SR_PGSERR | FLASH_SR_STRBERR | FLASH_SR_INCERR | FLASH_SR_OPERR | FLASH_SR_RDPERR |       \
     FLASH_SR_RDSERR | FLASH_SR_SNECCERR | FLASH_SR_DBECCERR | FLASH_SR_CRCRDERR)

///////////////////////////////////////////////////////////////////////////////
// External declarations.                                                    //
///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
#endif

    void flash_lld_init();
    void flash_lld_readBytes(uint32_t startAddress, uint32_t length, uint8_t *buffer);
    int flash_lld_write(uint32_t startAddress, uint32_t length, const uint8_t *buffer);
    int flash_lld_isErased(uint32_t startAddress, uint32_t length);
    int flash_lld_erase(uint32_t address);
    uint8_t flash_lld_getSector(uint32_t address);

#ifdef __cplusplus
}
#endif

#endif // HAL_NF_USE_STM32_FLASH

#endif // FLASH_LLD_H
