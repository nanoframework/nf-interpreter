//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2016 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef STM32F7xx_HAL_CONF_H
#define STM32F7xx_HAL_CONF_H

#define STM32F769xx

#ifdef __cplusplus
extern "C"
{
#endif

/* ########################## Module Selection ############################## */
/**
 * @brief This is the list of modules to be used in the HAL driver
 */
#define HAL_MODULE_ENABLED
// #define HAL_ADC_MODULE_ENABLED
// #define HAL_CAN_MODULE_ENABLED
// #define HAL_CAN_LEGACY_MODULE_ENABLED
// #define HAL_CEC_MODULE_ENABLED
// #define HAL_CRC_MODULE_ENABLED
// #define HAL_CRYP_MODULE_ENABLED
// #define HAL_DAC_MODULE_ENABLED
// #define HAL_DCMI_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
// #define HAL_DMA2D_MODULE_ENABLED
// #define HAL_ETH_MODULE_ENABLED
// #define HAL_FLASH_MODULE_ENABLED
// #define HAL_NAND_MODULE_ENABLED
// #define HAL_NOR_MODULE_ENABLED
// #define HAL_SRAM_MODULE_ENABLED
// #define HAL_SDRAM_MODULE_ENABLED
// #define HAL_HASH_MODULE_ENABLED
// #define HAL_GPIO_MODULE_ENABLED
// #define HAL_I2C_MODULE_ENABLED
// #define HAL_I2S_MODULE_ENABLED
// #define HAL_IWDG_MODULE_ENABLED
// #define HAL_LPTIM_MODULE_ENABLED
// #define HAL_LTDC_MODULE_ENABLED
// #define HAL_PWR_MODULE_ENABLED
#define HAL_QSPI_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
    // #define HAL_RNG_MODULE_ENABLED
    // #define HAL_RTC_MODULE_ENABLED
    // #define HAL_SAI_MODULE_ENABLED
    // #define HAL_SD_MODULE_ENABLED
    // #define HAL_SPDIFRX_MODULE_ENABLED
    // #define HAL_SPI_MODULE_ENABLED
    // #define HAL_TIM_MODULE_ENABLED
    // #define HAL_UART_MODULE_ENABLED
    // #define HAL_USART_MODULE_ENABLED
    // #define HAL_IRDA_MODULE_ENABLED
    // #define HAL_SMARTCARD_MODULE_ENABLED
    // #define HAL_WWDG_MODULE_ENABLED
    // #define HAL_CORTEX_MODULE_ENABLED
    // #define HAL_PCD_MODULE_ENABLED
    // #define HAL_HCD_MODULE_ENABLED
    // #define HAL_DFSDM_MODULE_ENABLED
    // #define HAL_DSI_MODULE_ENABLED
    // #define HAL_JPEG_MODULE_ENABLED
    // #define HAL_MDIOS_MODULE_ENABLED

    // ########################## HSE/HSI Values adaptation #####################

#if !defined(HSE_VALUE)
#define HSE_VALUE ((uint32_t)25000000U) /*!< Value of the External oscillator in Hz */
#endif                                  /* HSE_VALUE */

#if !defined(HSE_STARTUP_TIMEOUT)
#define HSE_STARTUP_TIMEOUT (100U) /*!< Time out for HSE start up, in ms */
#endif

#if !defined(HSI_VALUE)
#define HSI_VALUE (16000000U) /*!< Value of the Internal oscillator in Hz*/
#endif

// Internal Low Speed oscillator (LSI) value
#if !defined(LSI_VALUE)
// LSI Typical Value in Hz
#define LSI_VALUE ((uint32_t)32000U)
#endif

// External Low Speed oscillator (LSE) value.
#if !defined(LSE_VALUE)
// Value of the External Low Speed oscillator in Hz
#define LSE_VALUE ((uint32_t)32768U)
#endif

#if !defined(LSE_STARTUP_TIMEOUT)
// Time out for LSE start up, in ms
#define LSE_STARTUP_TIMEOUT ((uint32_t)5000U)
#endif

// ########################### System Configuration #########################
// This is the HAL system configuration section

// Value of VDD in mv
#define VDD_VALUE (3300U)
// tick interrupt priority
#define TICK_INT_PRIORITY (0x0FU)
#define USE_RTOS          0U
#define PREFETCH_ENABLE   1U
// To enable instruction cache and prefetch
#define ART_ACCLERATOR_ENABLE 1U

#define USE_HAL_ADC_REGISTER_CALLBACKS       0U /* ADC register callback disabled       */
#define USE_HAL_CAN_REGISTER_CALLBACKS       0U /* CAN register callback disabled       */
#define USE_HAL_CEC_REGISTER_CALLBACKS       0U /* CEC register callback disabled       */
#define USE_HAL_CRYP_REGISTER_CALLBACKS      0U /* CRYP register callback disabled      */
#define USE_HAL_DAC_REGISTER_CALLBACKS       0U /* DAC register callback disabled       */
#define USE_HAL_DCMI_REGISTER_CALLBACKS      0U /* DCMI register callback disabled      */
#define USE_HAL_DFSDM_REGISTER_CALLBACKS     0U /* DFSDM register callback disabled     */
#define USE_HAL_DMA2D_REGISTER_CALLBACKS     0U /* DMA2D register callback disabled     */
#define USE_HAL_DSI_REGISTER_CALLBACKS       0U /* DSI register callback disabled       */
#define USE_HAL_ETH_REGISTER_CALLBACKS       0U /* ETH register callback disabled       */
#define USE_HAL_HASH_REGISTER_CALLBACKS      0U /* HASH register callback disabled      */
#define USE_HAL_HCD_REGISTER_CALLBACKS       0U /* HCD register callback disabled       */
#define USE_HAL_I2C_REGISTER_CALLBACKS       0U /* I2C register callback disabled       */
#define USE_HAL_I2S_REGISTER_CALLBACKS       0U /* I2S register callback disabled       */
#define USE_HAL_IRDA_REGISTER_CALLBACKS      0U /* IRDA register callback disabled      */
#define USE_HAL_JPEG_REGISTER_CALLBACKS      0U /* JPEG register callback disabled      */
#define USE_HAL_LPTIM_REGISTER_CALLBACKS     0U /* LPTIM register callback disabled     */
#define USE_HAL_LTDC_REGISTER_CALLBACKS      0U /* LTDC register callback disabled      */
#define USE_HAL_MDIOS_REGISTER_CALLBACKS     0U /* MDIOS register callback disabled     */
#define USE_HAL_MMC_REGISTER_CALLBACKS       0U /* MMC register callback disabled       */
#define USE_HAL_NAND_REGISTER_CALLBACKS      0U /* NAND register callback disabled      */
#define USE_HAL_NOR_REGISTER_CALLBACKS       0U /* NOR register callback disabled       */
#define USE_HAL_PCD_REGISTER_CALLBACKS       0U /* PCD register callback disabled       */
#define USE_HAL_QSPI_REGISTER_CALLBACKS      0U /* QSPI register callback disabled      */
#define USE_HAL_RNG_REGISTER_CALLBACKS       0U /* RNG register callback disabled       */
#define USE_HAL_RTC_REGISTER_CALLBACKS       0U /* RTC register callback disabled       */
#define USE_HAL_SAI_REGISTER_CALLBACKS       0U /* SAI register callback disabled       */
#define USE_HAL_SD_REGISTER_CALLBACKS        0U /* SD register callback disabled        */
#define USE_HAL_SMARTCARD_REGISTER_CALLBACKS 0U /* SMARTCARD register callback disabled */
#define USE_HAL_SDRAM_REGISTER_CALLBACKS     0U /* SDRAM register callback disabled     */
#define USE_HAL_SRAM_REGISTER_CALLBACKS      0U /* SRAM register callback disabled      */
#define USE_HAL_SPDIFRX_REGISTER_CALLBACKS   0U /* SPDIFRX register callback disabled   */
#define USE_HAL_SMBUS_REGISTER_CALLBACKS     0U /* SMBUS register callback disabled     */
#define USE_HAL_SPI_REGISTER_CALLBACKS       0U /* SPI register callback disabled       */
#define USE_HAL_TIM_REGISTER_CALLBACKS       0U /* TIM register callback disabled       */
#define USE_HAL_UART_REGISTER_CALLBACKS      0U /* UART register callback disabled      */
#define USE_HAL_USART_REGISTER_CALLBACKS     0U /* USART register callback disabled     */
#define USE_HAL_WWDG_REGISTER_CALLBACKS      0U /* WWDG register callback disabled      */

    // Includes ------------------------------------------------------------------

#ifdef HAL_RCC_MODULE_ENABLED
#include "stm32f7xx_hal_rcc.h"
#endif /* HAL_RCC_MODULE_ENABLED */

#ifdef HAL_GPIO_MODULE_ENABLED
#include "stm32f7xx_hal_gpio.h"
#endif /* HAL_GPIO_MODULE_ENABLED */

#ifdef HAL_DMA_MODULE_ENABLED
#include "stm32f7xx_hal_dma.h"
#endif /* HAL_DMA_MODULE_ENABLED */

#ifdef HAL_CORTEX_MODULE_ENABLED
#include "stm32f7xx_hal_cortex.h"
#endif /* HAL_CORTEX_MODULE_ENABLED */

#ifdef HAL_ADC_MODULE_ENABLED
#include "stm32f7xx_hal_adc.h"
#endif /* HAL_ADC_MODULE_ENABLED */

#ifdef HAL_CAN_MODULE_ENABLED
#include "stm32f7xx_hal_can.h"
#endif /* HAL_CAN_MODULE_ENABLED */

#ifdef HAL_CAN_LEGACY_MODULE_ENABLED
#include "stm32f7xx_hal_can_legacy.h"
#endif /* HAL_CAN_LEGACY_MODULE_ENABLED */

#ifdef HAL_CEC_MODULE_ENABLED
#include "stm32f7xx_hal_cec.h"
#endif /* HAL_CEC_MODULE_ENABLED */

#ifdef HAL_CRC_MODULE_ENABLED
#include "stm32f7xx_hal_crc.h"
#endif /* HAL_CRC_MODULE_ENABLED */

#ifdef HAL_CRYP_MODULE_ENABLED
#include "stm32f7xx_hal_cryp.h"
#endif /* HAL_CRYP_MODULE_ENABLED */

#ifdef HAL_DMA2D_MODULE_ENABLED
#include "stm32f7xx_hal_dma2d.h"
#endif /* HAL_DMA2D_MODULE_ENABLED */

#ifdef HAL_DAC_MODULE_ENABLED
#include "stm32f7xx_hal_dac.h"
#endif /* HAL_DAC_MODULE_ENABLED */

#ifdef HAL_DCMI_MODULE_ENABLED
#include "stm32f7xx_hal_dcmi.h"
#endif /* HAL_DCMI_MODULE_ENABLED */

#ifdef HAL_ETH_MODULE_ENABLED
#include "stm32f7xx_hal_eth.h"
#endif /* HAL_ETH_MODULE_ENABLED */

#ifdef HAL_FLASH_MODULE_ENABLED
#include "stm32f7xx_hal_flash.h"
#endif /* HAL_FLASH_MODULE_ENABLED */

#ifdef HAL_SRAM_MODULE_ENABLED
#include "stm32f7xx_hal_sram.h"
#endif /* HAL_SRAM_MODULE_ENABLED */

#ifdef HAL_NOR_MODULE_ENABLED
#include "stm32f7xx_hal_nor.h"
#endif /* HAL_NOR_MODULE_ENABLED */

#ifdef HAL_NAND_MODULE_ENABLED
#include "stm32f7xx_hal_nand.h"
#endif /* HAL_NAND_MODULE_ENABLED */

#ifdef HAL_SDRAM_MODULE_ENABLED
#include "stm32f7xx_hal_sdram.h"
#endif /* HAL_SDRAM_MODULE_ENABLED */

#ifdef HAL_HASH_MODULE_ENABLED
#include "stm32f7xx_hal_hash.h"
#endif /* HAL_HASH_MODULE_ENABLED */

#ifdef HAL_I2C_MODULE_ENABLED
#include "stm32f7xx_hal_i2c.h"
#endif /* HAL_I2C_MODULE_ENABLED */

#ifdef HAL_I2S_MODULE_ENABLED
#include "stm32f7xx_hal_i2s.h"
#endif /* HAL_I2S_MODULE_ENABLED */

#ifdef HAL_IWDG_MODULE_ENABLED
#include "stm32f7xx_hal_iwdg.h"
#endif /* HAL_IWDG_MODULE_ENABLED */

#ifdef HAL_LPTIM_MODULE_ENABLED
#include "stm32f7xx_hal_lptim.h"
#endif /* HAL_LPTIM_MODULE_ENABLED */

#ifdef HAL_LTDC_MODULE_ENABLED
#include "stm32f7xx_hal_ltdc.h"
#endif /* HAL_LTDC_MODULE_ENABLED */

#ifdef HAL_PWR_MODULE_ENABLED
#include "stm32f7xx_hal_pwr.h"
#endif /* HAL_PWR_MODULE_ENABLED */

#ifdef HAL_QSPI_MODULE_ENABLED
#include "stm32f7xx_hal_qspi.h"
#endif /* HAL_QSPI_MODULE_ENABLED */

#ifdef HAL_RNG_MODULE_ENABLED
#include "stm32f7xx_hal_rng.h"
#endif /* HAL_RNG_MODULE_ENABLED */

#ifdef HAL_RTC_MODULE_ENABLED
#include "stm32f7xx_hal_rtc.h"
#endif /* HAL_RTC_MODULE_ENABLED */

#ifdef HAL_SAI_MODULE_ENABLED
#include "stm32f7xx_hal_sai.h"
#endif /* HAL_SAI_MODULE_ENABLED */

#ifdef HAL_SD_MODULE_ENABLED
#include "stm32f7xx_hal_sd.h"
#endif /* HAL_SD_MODULE_ENABLED */

#ifdef HAL_SPDIFRX_MODULE_ENABLED
#include "stm32f7xx_hal_spdifrx.h"
#endif /* HAL_SPDIFRX_MODULE_ENABLED */

#ifdef HAL_SPI_MODULE_ENABLED
#include "stm32f7xx_hal_spi.h"
#endif /* HAL_SPI_MODULE_ENABLED */

#ifdef HAL_TIM_MODULE_ENABLED
#include "stm32f7xx_hal_tim.h"
#endif /* HAL_TIM_MODULE_ENABLED */

#ifdef HAL_UART_MODULE_ENABLED
#include "stm32f7xx_hal_uart.h"
#endif /* HAL_UART_MODULE_ENABLED */

#ifdef HAL_USART_MODULE_ENABLED
#include "stm32f7xx_hal_usart.h"
#endif /* HAL_USART_MODULE_ENABLED */

#ifdef HAL_IRDA_MODULE_ENABLED
#include "stm32f7xx_hal_irda.h"
#endif /* HAL_IRDA_MODULE_ENABLED */

#ifdef HAL_SMARTCARD_MODULE_ENABLED
#include "stm32f7xx_hal_smartcard.h"
#endif /* HAL_SMARTCARD_MODULE_ENABLED */

#ifdef HAL_WWDG_MODULE_ENABLED
#include "stm32f7xx_hal_wwdg.h"
#endif /* HAL_WWDG_MODULE_ENABLED */

#ifdef HAL_PCD_MODULE_ENABLED
#include "stm32f7xx_hal_pcd.h"
#endif /* HAL_PCD_MODULE_ENABLED */

#ifdef HAL_HCD_MODULE_ENABLED
#include "stm32f7xx_hal_hcd.h"
#endif /* HAL_HCD_MODULE_ENABLED */

#ifdef HAL_DFSDM_MODULE_ENABLED
#include "stm32f7xx_hal_dfsdm.h"
#endif /* HAL_DFSDM_MODULE_ENABLED */

#ifdef HAL_DSI_MODULE_ENABLED
#include "stm32f7xx_hal_dsi.h"
#endif /* HAL_DSI_MODULE_ENABLED */

#ifdef HAL_JPEG_MODULE_ENABLED
#include "stm32f7xx_hal_jpeg.h"
#endif /* HAL_JPEG_MODULE_ENABLED */

#ifdef HAL_MDIOS_MODULE_ENABLED
#include "stm32f7xx_hal_mdios.h"
#endif /* HAL_MDIOS_MODULE_ENABLED */

    // Exported macro ------------------------------------------------------------

#ifdef USE_FULL_ASSERT
    /**
     * @brief  The assert_param macro is used for function's parameters check.
     * @param  expr: If expr is false, it calls assert_failed function
     *         which reports the name of the source file and the source
     *         line number of the call that failed.
     *         If expr is true, it returns no value.
     * @retval None
     */
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
    void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

#ifdef __cplusplus
}
#endif

#endif // STM32F7xx_HAL_CONF_H
