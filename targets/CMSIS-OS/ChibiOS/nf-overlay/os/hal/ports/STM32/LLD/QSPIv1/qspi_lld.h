//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

//Help: https://github.com/xpacks/stm32f7-hal/blob/xpack/Drivers/STM32F7xx_HAL_Driver/Inc/stm32f7xx_hal_qspi.h

#ifndef QSPI_LLD_H
#define QSPI_LLD_H

#include <hal.h>

#if defined(STM32F4XX)
#include <stm32f4xx_hal.h>
#endif
#if defined(STM32F7XX)
#include <stm32f7xx_hal.h>
#endif

#if (HAL_NF_USE_STM32_QSPI == TRUE)

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/


/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @brief   QUADSPI1 interrupt priority level setting.
 */
#if !defined(STM32_QSPI_QUADSPI1_IRQ_PRIORITY)
#define STM32_QSPI_QUADSPI1_IRQ_PRIORITY    10
#endif

#define QSPI_CLK_ENABLE()             __HAL_RCC_QSPI_CLK_ENABLE()
#define QSPI_CLK_DISABLE()            __HAL_RCC_QSPI_CLK_DISABLE()
#define QSPI_FORCE_RESET()            __HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()          __HAL_RCC_QSPI_RELEASE_RESET()

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

extern QSPI_HandleTypeDef QSPID1;

#ifdef __cplusplus
extern "C" {
#endif

  void qspi_lld_init(void);
  void qspi_lld_start(QSPI_HandleTypeDef* qspip);
  void qspi_lld_stop(QSPI_HandleTypeDef* qspi);

#ifdef __cplusplus
}
#endif

#endif /* HAL_NF_USE_STM32_QSPI */

#endif /* QSPI_LLD_H */
