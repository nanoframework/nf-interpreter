//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

//possible help:
// https://github.com/xpacks/stm32f7-hal/blob/xpack/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_qspi.c

#include <hal.h>
#include <hal_nf_community.h>

#if (HAL_NF_USE_STM32_QSPI == TRUE)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/** @brief QSPID1 driver identifier.*/
QSPI_HandleTypeDef QSPID1;

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Low level QSPI driver initialization.
 *
 * @notapi
 */
void qspi_lld_init()
{

}

void qspi_lld_start(QSPI_HandleTypeDef* qspi)
{
    QSPI_CLK_ENABLE();
    QSPI_FORCE_RESET();
    QSPI_RELEASE_RESET();

    nvicEnableVector(STM32_QUADSPI1_NUMBER, STM32_QSPI_QUADSPI1_IRQ_PRIORITY);

    qspi->Instance = QUADSPI;

    qspi->State  = HAL_QSPI_STATE_RESET;  

    // driver is ready
    qspi->State = HAL_QSPI_STATE_READY;
}

void qspi_lld_stop(QSPI_HandleTypeDef* qspi)
{
    /* If in ready state then disables the QUADSPI clock.*/
    if (qspi->State == HAL_QSPI_STATE_READY)
    {
        /* WSPI disable.*/
        qspi->Instance->CR = 0U;

        // /* Releasing the DMA.*/
        // dmaStreamFreeI(wspip->dma);
        // wspip->dma = NULL;

        /* Stopping involved clocks.*/
        rccDisableQUADSPI1();
    }
    
    // driver is stopped
    qspi->State = HAL_QSPI_STATE_RESET;
}

// QSPI interrupt handler, following ChibiOS style
OSAL_IRQ_HANDLER(STM32_QUADSPI1_HANDLER) {

    OSAL_IRQ_PROLOGUE();

    HAL_QSPI_IRQHandler(&QSPID1);

    OSAL_IRQ_EPILOGUE();
}

#endif /* HAL_NF_USE_STM32_QSPI */
