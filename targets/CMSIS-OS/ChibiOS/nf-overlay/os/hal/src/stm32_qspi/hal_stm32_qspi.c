//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

/*
 * Hardware Abstraction Layer for QSPI
 */
#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_NF_USE_STM32_QSPI == TRUE)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/


// Initializes the QSPI Driver
void qspiInit()
{
  qspi_lld_init();
}

// Configures and activates the QSPI peripheral
void qspiStart(QSPI_HandleTypeDef* qspi)
{
  // better lock this to setup the driver and start the peripheral
  osalSysLock();

  qspi_lld_start(qspi);

  osalSysUnlock();
}

// Deactivates the QSPI peripheral
void qspiStop(QSPI_HandleTypeDef* qspi)
{
    // better lock this to stop the peripheral
    osalSysLock();
  
    qspi_lld_stop(qspi);
    
    osalSysUnlock();
}

#endif /* HAL_NF_USE_STM32_QSPI */
