//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef FLASH_LLD_H
#define FLASH_LLD_H

#include "stm32_registry.h"
#include <hal_nf_community.h>

#if (HAL_NF_USE_STM32_FLASH == TRUE)

///////////////////////////////////////////////////////////////////////////////
// Driver constants.                                                         //
///////////////////////////////////////////////////////////////////////////////
#define FLASH_KEY1 0x45670123U
#define FLASH_KEY2 0xCDEF89ABU

#if defined(STM32L4P5xx) || defined(STM32L4Q5xx) || defined(STM32L4R5xx) || defined(STM32L4R7xx) ||                    \
    defined(STM32L4R9xx) || defined(STM32L4S5xx) || defined(STM32L4S7xx) || defined(STM32L4S9xx)
#define FLASH_NB_DOUBLE_WORDS_IN_ROW 64
#else
#define FLASH_NB_DOUBLE_WORDS_IN_ROW 32
#endif

// FLASH_Type_Program FLASH Program Type
// Program a double-word (64-bit) at a specified address.
#define FLASH_TYPEPROGRAM_DOUBLEWORD ((uint32_t)0x00)

// Fast program a 32 row double-word (64-bit) at a specified address.
// And another 32 row double-word (64-bit) will be programmed
#define FLASH_TYPEPROGRAM_FAST ((uint32_t)0x01)

// Fast program a 32 row double-word (64-bit) at a specified address.
// And this is the last 32 row double-word (64-bit) programmed
#define FLASH_TYPEPROGRAM_FAST_AND_LAST ((uint32_t)0x02)

///////////////////////////////////////////////////////////////////////////////
// Driver pre-compile time settings.                                         //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Derived constants and error checks.                                       //
///////////////////////////////////////////////////////////////////////////////
/**
 * @brief  FLASH Cache structure definition
 */
typedef enum
{
    FLASH_CACHE_DISABLED = 0,
    FLASH_CACHE_ICACHE_ENABLED,
    FLASH_CACHE_DCACHE_ENABLED,
    FLASH_CACHE_ICACHE_DCACHE_ENABLED
} FLASH_CacheTypeDef;

#define FLASH_TIMEOUT_VALUE ((uint32_t)50000)

#if defined(STM32L4P5xx) || defined(STM32L4Q5xx) || defined(STM32L4R5xx) || defined(STM32L4R7xx) ||                    \
    defined(STM32L4R9xx) || defined(STM32L4S5xx) || defined(STM32L4S7xx) || defined(STM32L4S9xx)
#define FLASH_PAGE_SIZE          ((uint32_t)0x1000)
#define FLASH_PAGE_SIZE_128_BITS ((uint32_t)0x2000)
#else
#define FLASH_PAGE_SIZE ((uint32_t)0x800)
#endif

/** @defgroup FLASH_Banks FLASH Banks
 * @{
 */
#define FLASH_BANK_1 ((uint32_t)0x01)
#if defined(STM32L471xx) || defined(STM32L475xx) || defined(STM32L476xx) || defined(STM32L485xx) ||                    \
    defined(STM32L486xx) || defined(STM32L496xx) || defined(STM32L4A6xx) || defined(STM32L4P5xx) ||                    \
    defined(STM32L4Q5xx) || defined(STM32L4R5xx) || defined(STM32L4R7xx) || defined(STM32L4R9xx) ||                    \
    defined(STM32L4S5xx) || defined(STM32L4S7xx) || defined(STM32L4S9xx)
#define FLASH_BANK_2    ((uint32_t)0x02)
#define FLASH_BANK_BOTH ((uint32_t)(FLASH_BANK_1 | FLASH_BANK_2))
#else
#define FLASH_BANK_BOTH ((uint32_t)(FLASH_BANK_1))
#endif

/** @defgroup FLASH_Private_Constants FLASH Private Constants
 * @{
 */
#if defined(STM32L471xx) || defined(STM32L475xx) || defined(STM32L476xx) || defined(STM32L485xx) ||                    \
    defined(STM32L486xx) || defined(STM32L496xx) || defined(STM32L4A6xx) || defined(STM32L4P5xx) ||                    \
    defined(STM32L4Q5xx) || defined(STM32L4R5xx) || defined(STM32L4R7xx) || defined(STM32L4R9xx) ||                    \
    defined(STM32L4S5xx) || defined(STM32L4S7xx) || defined(STM32L4S9xx)
#define FLASH_BANK_SIZE (FLASH_SIZE >> 1U)
#else
#define FLASH_BANK_SIZE (FLASH_SIZE)
#endif

#define STM32_FLASH_LINE_SIZE 8U
#define STM32_FLASH_LINE_MASK (STM32_FLASH_LINE_SIZE - 1U)

///////////////////////////////////////////////////////////////////////////////
// Driver data structures and types.                                         //
///////////////////////////////////////////////////////////////////////////////

// From STMicroelectronics Cube HAL
// HAL Status structures definition
typedef struct STM32FlashDriver
{

    // pointer to the FLASH Registers
    FLASH_TypeDef *flash;

} STM32FlashDriver;

/**
 * @brief  HAL Status structures definition
 */
typedef enum
{
    HAL_OK = 0x00U,
    HAL_ERROR = 0x01U,
    HAL_BUSY = 0x02U,
    HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;

///////////////////////////////////////////////////////////////////////////////
// Driver macros.                                                            //
///////////////////////////////////////////////////////////////////////////////

// From STMicroelectronics Cube HAL
#define HAL_IS_BIT_SET(REG, BIT) (((REG) & (BIT)) != RESET)

/**
 * @brief  Check whether the specified FLASH flag is set or not.
 * @param  __FLAG__ specifies the FLASH flag to check.
 *   This parameter can be one of the following values:
 *     @arg FLASH_FLAG_EOP: FLASH End of Operation flag
 *     @arg FLASH_FLAG_OPERR: FLASH Operation error flag
 *     @arg FLASH_FLAG_PROGERR: FLASH Programming error flag
 *     @arg FLASH_FLAG_WRPERR: FLASH Write protection error flag
 *     @arg FLASH_FLAG_PGAERR: FLASH Programming alignment error flag
 *     @arg FLASH_FLAG_SIZERR: FLASH Size error flag
 *     @arg FLASH_FLAG_PGSERR: FLASH Programming sequence error flag
 *     @arg FLASH_FLAG_MISERR: FLASH Fast programming data miss error flag
 *     @arg FLASH_FLAG_FASTERR: FLASH Fast programming error flag
 *     @arg FLASH_FLAG_RDERR: FLASH PCROP read  error flag
 *     @arg FLASH_FLAG_OPTVERR: FLASH Option validity error flag
 *     @arg FLASH_FLAG_BSY: FLASH write/erase operations in progress flag
 *     @arg FLASH_FLAG_PEMPTY : FLASH Boot from not programmed flash (apply only for STM32L43x/STM32L44x devices)
 *     @arg FLASH_FLAG_ECCC: FLASH one ECC error has been detected and corrected
 *     @arg FLASH_FLAG_ECCD: FLASH two ECC errors have been detected
 * @retval The new state of FLASH_FLAG (SET or RESET).
 */
#define __HAL_FLASH_GET_FLAG(__FLAG__)                                                                                 \
    ((((__FLAG__)&FLASH_FLAG_ECCR_ERRORS) != 0U) ? (READ_BIT(FLASH->ECCR, (__FLAG__)) != 0U)                           \
                                                 : (READ_BIT(FLASH->SR, (__FLAG__)) != 0U))

/**
 * @brief  Clear the FLASH's pending flags.
 * @param  __FLAG__ specifies the FLASH flags to clear.
 *   This parameter can be any combination of the following values:
 *     @arg FLASH_FLAG_EOP: FLASH End of Operation flag
 *     @arg FLASH_FLAG_OPERR: FLASH Operation error flag
 *     @arg FLASH_FLAG_PROGERR: FLASH Programming error flag
 *     @arg FLASH_FLAG_WRPERR: FLASH Write protection error flag
 *     @arg FLASH_FLAG_PGAERR: FLASH Programming alignment error flag
 *     @arg FLASH_FLAG_SIZERR: FLASH Size error flag
 *     @arg FLASH_FLAG_PGSERR: FLASH Programming sequence error flag
 *     @arg FLASH_FLAG_MISERR: FLASH Fast programming data miss error flag
 *     @arg FLASH_FLAG_FASTERR: FLASH Fast programming error flag
 *     @arg FLASH_FLAG_RDERR: FLASH PCROP read  error flag
 *     @arg FLASH_FLAG_OPTVERR: FLASH Option validity error flag
 *     @arg FLASH_FLAG_ECCC: FLASH one ECC error has been detected and corrected
 *     @arg FLASH_FLAG_ECCD: FLASH two ECC errors have been detected
 *     @arg FLASH_FLAG_ALL_ERRORS: FLASH All errors flags
 * @retval None
 */
#define __HAL_FLASH_CLEAR_FLAG(__FLAG__)                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if (((__FLAG__)&FLASH_FLAG_ECCR_ERRORS) != 0U)                                                                 \
        {                                                                                                              \
            SET_BIT(FLASH->ECCR, ((__FLAG__)&FLASH_FLAG_ECCR_ERRORS));                                                 \
        }                                                                                                              \
        if (((__FLAG__) & ~(FLASH_FLAG_ECCR_ERRORS)) != 0U)                                                            \
        {                                                                                                              \
            WRITE_REG(FLASH->SR, ((__FLAG__) & ~(FLASH_FLAG_ECCR_ERRORS)));                                            \
        }                                                                                                              \
    } while (0)

/**
 * @brief  Enable the FLASH instruction cache.
 * @retval none
 */
#define __HAL_FLASH_INSTRUCTION_CACHE_ENABLE() SET_BIT(FLASH->ACR, FLASH_ACR_ICEN)

/**
 * @brief  Disable the FLASH instruction cache.
 * @retval none
 */
#define __HAL_FLASH_INSTRUCTION_CACHE_DISABLE() CLEAR_BIT(FLASH->ACR, FLASH_ACR_ICEN)

/**
 * @brief  Enable the FLASH data cache.
 * @retval none
 */
#define __HAL_FLASH_DATA_CACHE_ENABLE() SET_BIT(FLASH->ACR, FLASH_ACR_DCEN)

/**
 * @brief  Disable the FLASH data cache.
 * @retval none
 */
#define __HAL_FLASH_DATA_CACHE_DISABLE() CLEAR_BIT(FLASH->ACR, FLASH_ACR_DCEN)

/**
 * @brief  Reset the FLASH instruction Cache.
 * @note   This function must be used only when the Instruction Cache is disabled.
 * @retval None
 */
#define __HAL_FLASH_INSTRUCTION_CACHE_RESET()                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        SET_BIT(FLASH->ACR, FLASH_ACR_ICRST);                                                                          \
        CLEAR_BIT(FLASH->ACR, FLASH_ACR_ICRST);                                                                        \
    } while (0)

/**
 * @brief  Reset the FLASH data Cache.
 * @note   This function must be used only when the data Cache is disabled.
 * @retval None
 */
#define __HAL_FLASH_DATA_CACHE_RESET()                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        SET_BIT(FLASH->ACR, FLASH_ACR_DCRST);                                                                          \
        CLEAR_BIT(FLASH->ACR, FLASH_ACR_DCRST);                                                                        \
    } while (0)

/** @defgroup FLASH_Flags FLASH Flags
 * @{
 */

#define FLASH_FLAG_EOP     FLASH_SR_EOP     /*!< FLASH End of operation flag */
#define FLASH_FLAG_OPERR   FLASH_SR_OPERR   /*!< FLASH Operation error flag */
#define FLASH_FLAG_PROGERR FLASH_SR_PROGERR /*!< FLASH Programming error flag */
#define FLASH_FLAG_WRPERR  FLASH_SR_WRPERR  /*!< FLASH Write protection error flag */
#define FLASH_FLAG_PGAERR  FLASH_SR_PGAERR  /*!< FLASH Programming alignment error flag */
#define FLASH_FLAG_SIZERR  FLASH_SR_SIZERR  /*!< FLASH Size error flag  */
#define FLASH_FLAG_PGSERR  FLASH_SR_PGSERR  /*!< FLASH Programming sequence error flag */
#define FLASH_FLAG_MISERR  FLASH_SR_MISERR  /*!< FLASH Fast programming data miss error flag */
#define FLASH_FLAG_FASTERR FLASH_SR_FASTERR /*!< FLASH Fast programming error flag */
#define FLASH_FLAG_RDERR   FLASH_SR_RDERR   /*!< FLASH PCROP read error flag */
#define FLASH_FLAG_OPTVERR FLASH_SR_OPTVERR /*!< FLASH Option validity error flag  */
#define FLASH_FLAG_BSY     FLASH_SR_BSY     /*!< FLASH Busy flag */
#if defined(STM32L412xx) || defined(STM32L422xx) || defined(STM32L431xx) || defined(STM32L432xx) ||                    \
    defined(STM32L433xx) || defined(STM32L442xx) || defined(STM32L443xx) || defined(STM32L451xx) ||                    \
    defined(STM32L452xx) || defined(STM32L462xx) || defined(STM32L4P5xx) || defined(STM32L4Q5xx) ||                    \
    defined(STM32L4R5xx) || defined(STM32L4R7xx) || defined(STM32L4R9xx) || defined(STM32L4S5xx) ||                    \
    defined(STM32L4S7xx) || defined(STM32L4S9xx)
#define FLASH_FLAG_PEMPTY FLASH_SR_PEMPTY /*!< FLASH Program empty */
#define FLASH_FLAG_SR_ERRORS                                                                                           \
    (FLASH_FLAG_OPERR | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR |               \
     FLASH_FLAG_PGSERR | FLASH_FLAG_MISERR | FLASH_FLAG_FASTERR | FLASH_FLAG_RDERR | FLASH_FLAG_OPTVERR |              \
     FLASH_FLAG_PEMPTY)
#else
#define FLASH_FLAG_SR_ERRORS                                                                                           \
    (FLASH_FLAG_OPERR | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR |               \
     FLASH_FLAG_PGSERR | FLASH_FLAG_MISERR | FLASH_FLAG_FASTERR | FLASH_FLAG_RDERR | FLASH_FLAG_OPTVERR)
#endif
#if defined(STM32L4P5xx) || defined(STM32L4Q5xx) || defined(STM32L4R5xx) || defined(STM32L4R7xx) ||                    \
    defined(STM32L4R9xx) || defined(STM32L4S5xx) || defined(STM32L4S7xx) || defined(STM32L4S9xx)
#define FLASH_FLAG_ECCC (FLASH_ECCR_ECCC | FLASH_ECCR_ECCC2) /*!< FLASH ECC correction */
#define FLASH_FLAG_ECCD (FLASH_ECCR_ECCD | FLASH_ECCR_ECCD2) /*!< FLASH ECC detection */
#else
#define FLASH_FLAG_ECCC FLASH_ECCR_ECCC /*!< FLASH ECC correction */
#define FLASH_FLAG_ECCD FLASH_ECCR_ECCD /*!< FLASH ECC detection */
#endif
#define FLASH_FLAG_ECCR_ERRORS (FLASH_FLAG_ECCD | FLASH_FLAG_ECCC)

#define FLASH_FLAG_ALL_ERRORS                                                                                          \
    (FLASH_FLAG_OPERR | FLASH_FLAG_PROGERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR |               \
     FLASH_FLAG_PGSERR | FLASH_FLAG_MISERR | FLASH_FLAG_FASTERR | FLASH_FLAG_RDERR | FLASH_FLAG_OPTVERR |              \
     FLASH_FLAG_ECCR_ERRORS)

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
