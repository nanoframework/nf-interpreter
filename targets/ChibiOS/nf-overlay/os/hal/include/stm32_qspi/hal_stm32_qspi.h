//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef HAL_NF_STM32_QSPI_H
#define HAL_NF_STM32_QSPI_H

#if (HAL_NF_USE_STM32_QSPI == TRUE)
#if defined(STM32F0xx_MCUCONF)
#error "CAN'T ENABLE QSPI FOR STM32F0 series"
#endif

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Configuration options
 * @{
 */

/**
 * @brief   Enables the @p qspiAcquireBus() and @p qspiReleaseBus() APIs.
 * @note    Disabling this option saves both code and data space.
 */
// #if !defined(QSPI_USE_MUTUAL_EXCLUSION)
// #define QSPI_USE_MUTUAL_EXCLUSION        TRUE
// #endif

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

#include "qspi_lld.h"

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/* QSPI Error codes */
#define QSPI_OK            ((uint8_t)0x00)
#define QSPI_ERROR         ((uint8_t)0x01)
#define QSPI_BUSY          ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED     ((uint8_t)0x08)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

  void qspiInit(void);
  void qspiStart(QSPI_HandleTypeDef* qspip);
  void qspiStop(QSPI_HandleTypeDef* qspi);

#ifdef __cplusplus
}
#endif

#endif /* HAL_NF_USE_STM32_QSPI */

#endif /* HAL_NF_STM32_QSPI_H_ */

