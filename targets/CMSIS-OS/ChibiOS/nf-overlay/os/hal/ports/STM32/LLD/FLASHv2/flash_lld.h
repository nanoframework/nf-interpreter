//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef FLASH_LLD_H
#define FLASH_LLD_H

#include "stm32_registry.h"

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
#define FLASH_KEY1                  ((uint32_t)0x45670123U)
#define FLASH_KEY2                  ((uint32_t)0xCDEF89ABU)
#define SECTOR_MASK                 ((uint32_t)0xFFFFFF07)

//---------------------------------- STM32F4xx ------------------------------//
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx)|| defined(STM32F439xx) ||\
    defined(STM32F469xx) || defined(STM32F479xx) ||\
    defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F412Zx) ||\
    defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
    defined(STM32F401xC) ||\
    defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) ||\
    defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F446xx)

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

#if defined(FLASH_SR_RDERR)
#define FLASH_FLAG_ALL_ERRORS  (FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                        FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_RDERR)

#else
#define FLASH_FLAG_ALL_ERRORS  (FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                        FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR )

#endif // FLASH_SR_RDERR


//---------------------------------- STM32F7xx ------------------------------//
#elif defined(STM32F756xx) || defined(STM32F746xx) || defined(STM32F745xx) || defined(STM32F767xx) || \
    defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx) || defined(STM32F722xx) || \
	  defined(STM32F723xx) || defined(STM32F732xx) || defined(STM32F733xx)

#define FLASH_FLAG_EOP                 FLASH_SR_EOP            /*!< FLASH End of Operation flag               */
#define FLASH_FLAG_OPERR               FLASH_SR_OPERR          /*!< FLASH operation Error flag                */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPERR         /*!< FLASH Write protected error flag          */
#define FLASH_FLAG_PGAERR              FLASH_SR_PGAERR         /*!< FLASH Programming Alignment error flag    */
#define FLASH_FLAG_PGPERR              FLASH_SR_PGPERR         /*!< FLASH Programming Parallelism error flag  */
#define FLASH_FLAG_ERSERR              FLASH_SR_ERSERR         /*!< FLASH Erasing Sequence error flag         */
#define FLASH_FLAG_BSY                 FLASH_SR_BSY            /*!< FLASH Busy flag                           */

#if defined (FLASH_OPTCR2_PCROP)
#define FLASH_FLAG_RDERR               FLASH_SR_RDERR          /*!< FLASH Read protection error flag          */
#define FLASH_FLAG_ALL_ERRORS     (FLASH_FLAG_OPERR   | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                                   FLASH_FLAG_PGPERR  | FLASH_FLAG_ERSERR | FLASH_FLAG_RDERR)
#else
#define FLASH_FLAG_ALL_ERRORS     (FLASH_FLAG_OPERR   | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | \
                                   FLASH_FLAG_PGPERR  | FLASH_FLAG_ERSERR)
#endif /* FLASH_OPTCR2_PCROP */

#endif

// FLASH_Program_Parallelism FLASH Program Parallelism
#define FLASH_PSIZE_BYTE           ((uint32_t)0x00000000U)
#define FLASH_PSIZE_HALF_WORD      ((uint32_t)0x00000100U)
#define FLASH_PSIZE_WORD           ((uint32_t)0x00000200U)
#define FLASH_PSIZE_DOUBLE_WORD    ((uint32_t)0x00000300U)
#define CR_PSIZE_MASK              ((uint32_t)0xFFFFFCFFU)

#define __HAL_FLASH_GET_FLAG(__FLAG__)   ((FLASH->SR & (__FLAG__)))
#define __HAL_FLASH_CLEAR_FLAG(__FLAG__)   (FLASH->SR = (__FLAG__))

// FLASHEx_Sectors FLASH Sectors
/*-------------------------------------- STM32F42xxx/STM32F43xxx/STM32F469xx ------------------------------------*/   
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx)|| defined(STM32F439xx) ||\
    defined(STM32F469xx) || defined(STM32F479xx)
#define FLASH_SECTOR_0     ((uint32_t)0U)  /*!< Sector Number 0   */
#define FLASH_SECTOR_1     ((uint32_t)1U)  /*!< Sector Number 1   */
#define FLASH_SECTOR_2     ((uint32_t)2U)  /*!< Sector Number 2   */
#define FLASH_SECTOR_3     ((uint32_t)3U)  /*!< Sector Number 3   */
#define FLASH_SECTOR_4     ((uint32_t)4U)  /*!< Sector Number 4   */
#define FLASH_SECTOR_5     ((uint32_t)5U)  /*!< Sector Number 5   */
#define FLASH_SECTOR_6     ((uint32_t)6U)  /*!< Sector Number 6   */
#define FLASH_SECTOR_7     ((uint32_t)7U)  /*!< Sector Number 7   */
#define FLASH_SECTOR_8     ((uint32_t)8U)  /*!< Sector Number 8   */
#define FLASH_SECTOR_9     ((uint32_t)9U)  /*!< Sector Number 9   */
#define FLASH_SECTOR_10    ((uint32_t)10U) /*!< Sector Number 10  */
#define FLASH_SECTOR_11    ((uint32_t)11U) /*!< Sector Number 11  */
#define FLASH_SECTOR_12    ((uint32_t)12U) /*!< Sector Number 12  */
#define FLASH_SECTOR_13    ((uint32_t)13U) /*!< Sector Number 13  */
#define FLASH_SECTOR_14    ((uint32_t)14U) /*!< Sector Number 14  */
#define FLASH_SECTOR_15    ((uint32_t)15U) /*!< Sector Number 15  */
#define FLASH_SECTOR_16    ((uint32_t)16U) /*!< Sector Number 16  */
#define FLASH_SECTOR_17    ((uint32_t)17U) /*!< Sector Number 17  */
#define FLASH_SECTOR_18    ((uint32_t)18U) /*!< Sector Number 18  */
#define FLASH_SECTOR_19    ((uint32_t)19U) /*!< Sector Number 19  */
#define FLASH_SECTOR_20    ((uint32_t)20U) /*!< Sector Number 20  */
#define FLASH_SECTOR_21    ((uint32_t)21U) /*!< Sector Number 21  */
#define FLASH_SECTOR_22    ((uint32_t)22U) /*!< Sector Number 22  */
#define FLASH_SECTOR_23    ((uint32_t)23U) /*!< Sector Number 23  */
#endif /* STM32F427xx || STM32F437xx || STM32F429xx|| STM32F439xx || STM32F469xx || STM32F479xx */
/*-----------------------------------------------------------------------------------------------------*/  

/*--------------------------------------- STM32F40xxx/STM32F41xxx -------------------------------------*/ 
#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || defined(STM32F412Zx) ||\
    defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx)  
#define FLASH_SECTOR_0     ((uint32_t)0U)  /*!< Sector Number 0   */
#define FLASH_SECTOR_1     ((uint32_t)1U)  /*!< Sector Number 1   */
#define FLASH_SECTOR_2     ((uint32_t)2U)  /*!< Sector Number 2   */
#define FLASH_SECTOR_3     ((uint32_t)3U)  /*!< Sector Number 3   */
#define FLASH_SECTOR_4     ((uint32_t)4U)  /*!< Sector Number 4   */
#define FLASH_SECTOR_5     ((uint32_t)5U)  /*!< Sector Number 5   */
#define FLASH_SECTOR_6     ((uint32_t)6U)  /*!< Sector Number 6   */
#define FLASH_SECTOR_7     ((uint32_t)7U)  /*!< Sector Number 7   */
#define FLASH_SECTOR_8     ((uint32_t)8U)  /*!< Sector Number 8   */
#define FLASH_SECTOR_9     ((uint32_t)9U)  /*!< Sector Number 9   */
#define FLASH_SECTOR_10    ((uint32_t)10U) /*!< Sector Number 10  */
#define FLASH_SECTOR_11    ((uint32_t)11U) /*!< Sector Number 11  */
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F412Zx || STM32F412Vx || STM32F412Rx || STM32F412Cx */
/*-----------------------------------------------------------------------------------------------------*/

/*--------------------------------------------- STM32F401xC -------------------------------------------*/ 
#if defined(STM32F401xC)
#define FLASH_SECTOR_0     ((uint32_t)0U) /*!< Sector Number 0   */
#define FLASH_SECTOR_1     ((uint32_t)1U) /*!< Sector Number 1   */
#define FLASH_SECTOR_2     ((uint32_t)2U) /*!< Sector Number 2   */
#define FLASH_SECTOR_3     ((uint32_t)3U) /*!< Sector Number 3   */
#define FLASH_SECTOR_4     ((uint32_t)4U) /*!< Sector Number 4   */
#define FLASH_SECTOR_5     ((uint32_t)5U) /*!< Sector Number 5   */
#endif /* STM32F401xC */
/*-----------------------------------------------------------------------------------------------------*/

/*--------------------------------------------- STM32F410xx -------------------------------------------*/ 
#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx)
#define FLASH_SECTOR_0     ((uint32_t)0U) /*!< Sector Number 0   */
#define FLASH_SECTOR_1     ((uint32_t)1U) /*!< Sector Number 1   */
#define FLASH_SECTOR_2     ((uint32_t)2U) /*!< Sector Number 2   */
#define FLASH_SECTOR_3     ((uint32_t)3U) /*!< Sector Number 3   */
#define FLASH_SECTOR_4     ((uint32_t)4U) /*!< Sector Number 4   */
#endif /* STM32F410Tx || STM32F410Cx || STM32F410Rx */
/*-----------------------------------------------------------------------------------------------------*/

/*---------------------------------- STM32F401xE/STM32F411xE/STM32F446xx ------------------------------*/
#if defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F446xx)
#define FLASH_SECTOR_0     ((uint32_t)0U) /*!< Sector Number 0   */
#define FLASH_SECTOR_1     ((uint32_t)1U) /*!< Sector Number 1   */
#define FLASH_SECTOR_2     ((uint32_t)2U) /*!< Sector Number 2   */
#define FLASH_SECTOR_3     ((uint32_t)3U) /*!< Sector Number 3   */
#define FLASH_SECTOR_4     ((uint32_t)4U) /*!< Sector Number 4   */
#define FLASH_SECTOR_5     ((uint32_t)5U) /*!< Sector Number 5   */
#define FLASH_SECTOR_6     ((uint32_t)6U) /*!< Sector Number 6   */
#define FLASH_SECTOR_7     ((uint32_t)7U) /*!< Sector Number 7   */
#endif /* STM32F401xE || STM32F411xE || STM32F446xx */
/*-----------------------------------------------------------------------------------------------------*/

/*---------------------------------- STM32F7xx ------------------------------*/
#if defined(STM32F756xx) || defined(STM32F746xx) || defined(STM32F745xx) || defined(STM32F767xx) || \
    defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx) || defined(STM32F722xx) || \
	  defined(STM32F723xx) || defined(STM32F732xx) || defined(STM32F733xx)

#if (FLASH_SECTOR_TOTAL == 24)
#define FLASH_SECTOR_8     ((uint32_t)8U)  /*!< Sector Number 8   */
#define FLASH_SECTOR_9     ((uint32_t)9U)  /*!< Sector Number 9   */
#define FLASH_SECTOR_10    ((uint32_t)10U) /*!< Sector Number 10  */
#define FLASH_SECTOR_11    ((uint32_t)11U) /*!< Sector Number 11  */
#define FLASH_SECTOR_12    ((uint32_t)12U) /*!< Sector Number 12  */
#define FLASH_SECTOR_13    ((uint32_t)13U) /*!< Sector Number 13  */
#define FLASH_SECTOR_14    ((uint32_t)14U) /*!< Sector Number 14  */
#define FLASH_SECTOR_15    ((uint32_t)15U) /*!< Sector Number 15  */
#define FLASH_SECTOR_16    ((uint32_t)16U) /*!< Sector Number 16  */
#define FLASH_SECTOR_17    ((uint32_t)17U) /*!< Sector Number 17  */
#define FLASH_SECTOR_18    ((uint32_t)18U) /*!< Sector Number 18  */
#define FLASH_SECTOR_19    ((uint32_t)19U) /*!< Sector Number 19  */
#define FLASH_SECTOR_20    ((uint32_t)20U) /*!< Sector Number 20  */
#define FLASH_SECTOR_21    ((uint32_t)21U) /*!< Sector Number 21  */
#define FLASH_SECTOR_22    ((uint32_t)22U) /*!< Sector Number 22  */
#define FLASH_SECTOR_23    ((uint32_t)23U) /*!< Sector Number 23  */
#else
// need to define here a 'dummy'' SECTOR_11, for F7 series this required when FLASH_SECTOR_TOTAL is 8
#define FLASH_SECTOR_11    ((uint32_t)11U) /*!< Sector Number 11  */
#endif /* FLASH_SECTOR_TOTAL == 24 */


#endif // efined(STM32F756xx) || defined(STM32F746xx) || defined(STM32F745xx) || defined(STM32F767xx) || .... etc

#define __HAL_FLASH_INSTRUCTION_CACHE_ENABLE()  (FLASH->ACR |= FLASH_ACR_ICEN)
#define __HAL_FLASH_INSTRUCTION_CACHE_DISABLE()   (FLASH->ACR &= (~FLASH_ACR_ICEN))

#define __HAL_FLASH_DATA_CACHE_DISABLE()   (FLASH->ACR &= (~FLASH_ACR_DCEN))
#define __HAL_FLASH_DATA_CACHE_ENABLE()  (FLASH->ACR |= FLASH_ACR_DCEN)
#define __HAL_FLASH_INSTRUCTION_CACHE_RESET() do {FLASH->ACR |= FLASH_ACR_ICRST;  \
                                                  FLASH->ACR &= ~FLASH_ACR_ICRST; \
                                                 }while(0U)
#define __HAL_FLASH_DATA_CACHE_RESET() do {FLASH->ACR |= FLASH_ACR_DCRST;  \
                                           FLASH->ACR &= ~FLASH_ACR_DCRST; \
                                          }while(0U)

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
  bool flash_lld_write(uint32_t startAddress, uint32_t length, const uint8_t* buffer);
  bool flash_lld_isErased(uint32_t startAddress, uint32_t length);
  bool flash_lld_erase(uint32_t address);
  uint8_t flash_lld_getSector(uint32_t address);

#ifdef __cplusplus
}
#endif

#endif // HAL_USE_STM32_FLASH

#endif // FLASH_LLD_H
