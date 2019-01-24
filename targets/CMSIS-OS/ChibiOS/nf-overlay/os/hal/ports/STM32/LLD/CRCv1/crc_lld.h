//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef CRC_LLD_H
#define CRC_LLD_H

#include "stm32_registry.h"
#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_USE_STM32_CRC == TRUE)

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

// Driver configuration structure.
typedef struct crcConfig {

  // indicates CRC length
  uint32_t                 CRCLength;

  // 7, 8, 16 or 32-bit long value for a polynomial degree
  uint32_t                 GeneratingPolynomial;

  // Input data format
  uint32_t                 InputDataFormat;

  // The final XOR value
  uint32_t                 FinalValue;

  // Specifies input data inversion mode
  bool                     InputDataInversionMode;

  // Specifies output data (i.e. CRC) inversion mode
  bool                     OutputDataInversionMode;

} crcConfig;

typedef struct CRCDriver {
  
    CRC_TypeDef             *Instance;  //Register base address

    // Driver state.
    crcState                State;

    // Current configuration data.
    const crcConfig         *Config;

#if CRC_USE_MUTUAL_EXCLUSION
    // Mutex protecting the peripheral
    mutex_t                 Lock;
#endif // CRC_USE_MUTUAL_EXCLUSION

} CRCDriver;


// From STMicroelectronics Cube HAL 
///////////////////////////////////////////

#if defined(STM32L0xx_MCUCONF)
// this series uses different names for the buses

#define rccEnableCRC(lp) rccEnableAPB1(RCC_AHBENR_CRCEN, lp)
#define rccDisableCRC() rccDisableAPB1(RCC_AHBENR_CRCEN)
#define rccResetCRC() rccResetAPB1(RCC_AHB1RSTR_CRCRST)

#endif

/** @defgroup CRC_Default_Polynomial_Value    Default CRC generating polynomial
  * @{
  */
#define DEFAULT_CRC32_POLY      0x04C11DB7U

/** @defgroup CRC_Default_InitValue    Default CRC computation initialization value
  * @{
  */
#define DEFAULT_CRC_INITVALUE   0xFFFFFFFFU

/** @defgroup CRC_Default_Polynomial    Indicates whether or not default polynomial is used
  * @{
  */
#define DEFAULT_POLYNOMIAL_ENABLE       ((uint8_t)0x00U)
#define DEFAULT_POLYNOMIAL_DISABLE      ((uint8_t)0x01U)

/** @defgroup CRC_Default_InitValue_Use    Indicates whether or not default init value is used
  * @{
  */                                      
#define DEFAULT_INIT_VALUE_ENABLE      ((uint8_t)0x00U)
#define DEFAULT_INIT_VALUE_DISABLE     ((uint8_t)0x01U)

/** @defgroup CRC_Polynomial_Sizes Polynomial sizes to configure the IP
  * @{
  */
#define CRC_POLYLENGTH_32B                  ((uint32_t)0x00000000U)
#define CRC_POLYLENGTH_16B                  ((uint32_t)CRC_CR_POLYSIZE_0)
#define CRC_POLYLENGTH_8B                   ((uint32_t)CRC_CR_POLYSIZE_1)
#define CRC_POLYLENGTH_7B                   ((uint32_t)CRC_CR_POLYSIZE)

/** @defgroup CRC_Polynomial_Size_Definitions CRC polynomial possible sizes actual definitions
  * @{
  */
#define HAL_CRC_LENGTH_32B     32U
#define HAL_CRC_LENGTH_16B     16U
#define HAL_CRC_LENGTH_8B       8U
#define HAL_CRC_LENGTH_7B       7U

/* WARNING: CRC_INPUT_FORMAT_UNDEFINED is created for reference purposes but
 * an error is triggered in HAL_CRC_Init() if InputDataFormat field is set 
 * to CRC_INPUT_FORMAT_UNDEFINED: the format MUST be defined by the user for 
 * the CRC APIs to provide a correct result */   
#define CRC_INPUTDATA_FORMAT_UNDEFINED             ((uint32_t)0x00000000U)
#define CRC_INPUTDATA_FORMAT_BYTES                 ((uint32_t)0x00000001U)
#define CRC_INPUTDATA_FORMAT_HALFWORDS             ((uint32_t)0x00000002U)
#define CRC_INPUTDATA_FORMAT_WORDS                 ((uint32_t)0x00000003U)

/** @defgroup CRCEx_Input_Data_Inversion CRC Extended input data inversion modes
  * @{
  */
#define CRC_INPUTDATA_INVERSION_NONE              ((uint32_t)0x00000000U)
#define CRC_INPUTDATA_INVERSION_BYTE              ((uint32_t)CRC_CR_REV_IN_0)
#define CRC_INPUTDATA_INVERSION_HALFWORD          ((uint32_t)CRC_CR_REV_IN_1)
#define CRC_INPUTDATA_INVERSION_WORD              ((uint32_t)CRC_CR_REV_IN)

/** @defgroup CRCEx_Output_Data_Inversion CRC Extended output data inversion modes
  * @{
  */
#define CRC_OUTPUTDATA_INVERSION_DISABLE         ((uint32_t)0x00000000U)
#define CRC_OUTPUTDATA_INVERSION_ENABLE          ((uint32_t)CRC_CR_REV_OUT)

///////////////////////////////////////////
 

///////////////////////////////////////////////////////////////////////////////
// Driver macros.                                                            //
///////////////////////////////////////////////////////////////////////////////

// From STMicroelectronics Cube HAL 

///////////////////////////////////////////////////////////////////////////////
// External declarations.                                                    //
///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif
  
  void crc_lld_init(void);
  void crc_lld_start(const crcConfig *config);
  void crc_lld_stop();
  void crc_lld_reset();
  uint32_t crc_lld_compute(const void* buffer, int size, uint32_t initialCrc);

#if (CRC_USE_MUTUAL_EXCLUSION == TRUE)
  void crc_lld_aquire();
  void crc_lld_release();
#endif

uint32_t crc_lld_reverseCRC32(uint32_t targetCRC);
uint32_t crc_lld_fastCRC32(uint32_t initialCrc, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif // HAL_USE_STM32_CRC

#endif // CRC_LLD_H
