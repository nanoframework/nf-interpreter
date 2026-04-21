//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio. All rights reserved.
// See LICENSE file in the project root for full license information.
//

// clang-format off

/**
 * @file    mcuconf.h
 * @brief   MCU hardware configuration for the MCUboot bootloader on ST_STM32F769I_DISCOVERY.
 *
 * Derived from targets/ChibiOS/ST_STM32F769I_DISCOVERY/nanoBooter/mcuconf.h.
 * Only the settings needed by the MCUboot bootloader are kept:
 *   - Full PLL from HSE (25 MHz → 216 MHz SYSCLK), identical to nanoBooter.
 *   - SERIAL USART1 enabled (PA9/PA10 AF7, 115200 baud) for recovery output.
 *   - All other peripheral drivers disabled.
 */

#ifndef MCUCONF_H
#define MCUCONF_H

#define STM32F7xx_MCUCONF
#define STM32F769_MCUCONF

/*===========================================================================*/
/* HAL driver system settings (clock tree — identical to nanoBooter).        */
/* HSE = 25 MHz, PLLM=25, PLLN=432, PLLP=2 → SYSCLK = 216 MHz.            */
/* PPRE1=DIV4 → PCLK1=54 MHz. PPRE2=DIV2 → PCLK2=108 MHz.                */
/*===========================================================================*/
#define STM32_NO_INIT                       FALSE
#define STM32_PVD_ENABLE                    FALSE
#define STM32_PLS                           STM32_PLS_LEV0
#define STM32_BKPRAM_ENABLE                 FALSE
#define STM32_HSI_ENABLED                   TRUE
#define STM32_LSI_ENABLED                   FALSE
#define STM32_HSE_ENABLED                   TRUE
#define STM32_LSE_ENABLED                   FALSE
#define STM32_CLOCK48_REQUIRED              FALSE
#define STM32_SW                            STM32_SW_PLL
#define STM32_PLLSRC                        STM32_PLLSRC_HSE
#define STM32_PLLM_VALUE                    25
#define STM32_PLLN_VALUE                    432
#define STM32_PLLP_VALUE                    2
#define STM32_PLLQ_VALUE                    9
#define STM32_PLLR_VALUE                    4
#define STM32_HPRE                          STM32_HPRE_DIV1
#define STM32_PPRE1                         STM32_PPRE1_DIV4
#define STM32_PPRE2                         STM32_PPRE2_DIV2
#define STM32_RTCSEL                        STM32_RTCSEL_LSI
#define STM32_RTCPRE_VALUE                  25
#define STM32_MCO1SEL                       STM32_MCO1SEL_HSI
#define STM32_MCO1PRE                       STM32_MCO1PRE_DIV1
#define STM32_MCO2SEL                       STM32_MCO2SEL_SYSCLK
#define STM32_MCO2PRE                       STM32_MCO2PRE_DIV4
#define STM32_TIMPRE_ENABLE                 FALSE
#define STM32_I2SSRC                        STM32_I2SSRC_OFF
#define STM32_PLLI2SN_VALUE                 192
#define STM32_PLLI2SP_VALUE                 4
#define STM32_PLLI2SQ_VALUE                 4
#define STM32_PLLI2SR_VALUE                 4
#define STM32_PLLI2SDIVQ_VALUE              2
#define STM32_PLLSAIN_VALUE                 192
#define STM32_PLLSAIP_VALUE                 4
#define STM32_PLLSAIQ_VALUE                 4
#define STM32_PLLSAIR_VALUE                 4
#define STM32_PLLSAIDIVQ_VALUE              2
#define STM32_PLLSAIDIVR_VALUE              2
#define STM32_SAI1SEL                       STM32_SAI1SEL_OFF
#define STM32_SAI2SEL                       STM32_SAI2SEL_OFF
#define STM32_LCDTFT_REQUIRED               FALSE
#define STM32_USART1SEL                     STM32_USART1SEL_PCLK2
#define STM32_USART2SEL                     STM32_USART2SEL_PCLK1
#define STM32_USART3SEL                     STM32_USART3SEL_PCLK1
#define STM32_UART4SEL                      STM32_UART4SEL_PCLK1
#define STM32_UART5SEL                      STM32_UART5SEL_PCLK1
#define STM32_USART6SEL                     STM32_USART6SEL_PCLK2
#define STM32_UART7SEL                      STM32_UART7SEL_PCLK1
#define STM32_UART8SEL                      STM32_UART8SEL_PCLK1
#define STM32_I2C1SEL                       STM32_I2C1SEL_PCLK1
#define STM32_I2C2SEL                       STM32_I2C2SEL_PCLK1
#define STM32_I2C3SEL                       STM32_I2C3SEL_PCLK1
#define STM32_I2C4SEL                       STM32_I2C4SEL_PCLK1
#define STM32_LPTIM1SEL                     STM32_LPTIM1SEL_PCLK1
#define STM32_CECSEL                        STM32_CECSEL_HSIDIV488
#define STM32_CK48MSEL                      STM32_CK48MSEL_PLL
#define STM32_SDMMC1SEL                     STM32_SDMMC1SEL_PLL48CLK
#define STM32_SDMMC2SEL                     STM32_SDMMC2SEL_PLL48CLK
#define STM32_SRAM2_NOCACHE                 FALSE

/*===========================================================================*/
/* IRQ priorities.                                                           */
/*===========================================================================*/
#define STM32_IRQ_EXTI0_PRIORITY            6
#define STM32_IRQ_EXTI1_PRIORITY            6
#define STM32_IRQ_EXTI2_PRIORITY            6
#define STM32_IRQ_EXTI3_PRIORITY            6
#define STM32_IRQ_EXTI4_PRIORITY            6
#define STM32_IRQ_EXTI5_9_PRIORITY          6
#define STM32_IRQ_EXTI10_15_PRIORITY        6
#define STM32_IRQ_EXTI16_PRIORITY           6
#define STM32_IRQ_EXTI17_PRIORITY           15
#define STM32_IRQ_EXTI18_PRIORITY           6
#define STM32_IRQ_EXTI19_PRIORITY           6
#define STM32_IRQ_EXTI20_PRIORITY           6
#define STM32_IRQ_EXTI21_PRIORITY           15
#define STM32_IRQ_EXTI22_PRIORITY           15
#define STM32_IRQ_EXTI23_PRIORITY           6

#define STM32_IRQ_USART1_PRIORITY           12
#define STM32_IRQ_USART2_PRIORITY           12
#define STM32_IRQ_USART3_PRIORITY           12
#define STM32_IRQ_UART4_PRIORITY            12
#define STM32_IRQ_UART5_PRIORITY            12
#define STM32_IRQ_USART6_PRIORITY           12
#define STM32_IRQ_UART7_PRIORITY            12
#define STM32_IRQ_UART8_PRIORITY            12

/*===========================================================================*/
/* ADC — disabled.                                                           */
/*===========================================================================*/
#define STM32_ADC_ADCPRE                    ADC_CCR_ADCPRE_DIV4
#define STM32_ADC_USE_ADC1                  FALSE
#define STM32_ADC_USE_ADC2                  FALSE
#define STM32_ADC_USE_ADC3                  FALSE
#define STM32_ADC_ADC1_DMA_STREAM           STM32_DMA_STREAM_ID(2, 4)
#define STM32_ADC_ADC2_DMA_STREAM           STM32_DMA_STREAM_ID(2, 2)
#define STM32_ADC_ADC3_DMA_STREAM           STM32_DMA_STREAM_ID(2, 1)
#define STM32_ADC_ADC1_DMA_PRIORITY         2
#define STM32_ADC_ADC2_DMA_PRIORITY         2
#define STM32_ADC_ADC3_DMA_PRIORITY         2
#define STM32_ADC_IRQ_PRIORITY              6
#define STM32_ADC_ADC1_DMA_IRQ_PRIORITY     6
#define STM32_ADC_ADC2_DMA_IRQ_PRIORITY     6
#define STM32_ADC_ADC3_DMA_IRQ_PRIORITY     6

/*===========================================================================*/
/* CAN — disabled.                                                           */
/*===========================================================================*/
#define STM32_CAN_USE_CAN1                  FALSE
#define STM32_CAN_USE_CAN2                  FALSE
#define STM32_CAN_USE_CAN3                  FALSE
#define STM32_CAN_CAN1_IRQ_PRIORITY         11
#define STM32_CAN_CAN2_IRQ_PRIORITY         11
#define STM32_CAN_CAN3_IRQ_PRIORITY         11

/*===========================================================================*/
/* DAC — disabled.                                                           */
/*===========================================================================*/
#define STM32_DAC_DUAL_MODE                 FALSE
#define STM32_DAC_USE_DAC1_CH1              FALSE
#define STM32_DAC_USE_DAC1_CH2              FALSE
#define STM32_DAC_DAC1_CH1_IRQ_PRIORITY     10
#define STM32_DAC_DAC1_CH2_IRQ_PRIORITY     10
#define STM32_DAC_DAC1_CH1_DMA_PRIORITY     2
#define STM32_DAC_DAC1_CH2_DMA_PRIORITY     2
#define STM32_DAC_DAC1_CH1_DMA_STREAM       STM32_DMA_STREAM_ID(1, 5)
#define STM32_DAC_DAC1_CH2_DMA_STREAM       STM32_DMA_STREAM_ID(1, 6)

/*===========================================================================*/
/* I2C — disabled.                                                           */
/*===========================================================================*/
#define STM32_I2C_USE_I2C1                  FALSE
#define STM32_I2C_USE_I2C2                  FALSE
#define STM32_I2C_USE_I2C3                  FALSE
#define STM32_I2C_USE_I2C4                  FALSE
#define STM32_I2C_BUSY_TIMEOUT              50
#define STM32_I2C_I2C1_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 0)
#define STM32_I2C_I2C1_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 6)
#define STM32_I2C_I2C2_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 2)
#define STM32_I2C_I2C2_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 7)
#define STM32_I2C_I2C3_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 2)
#define STM32_I2C_I2C3_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 4)
#define STM32_I2C_I2C4_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 2)
#define STM32_I2C_I2C4_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 5)
#define STM32_I2C_I2C1_IRQ_PRIORITY         5
#define STM32_I2C_I2C2_IRQ_PRIORITY         5
#define STM32_I2C_I2C3_IRQ_PRIORITY         5
#define STM32_I2C_I2C4_IRQ_PRIORITY         5
#define STM32_I2C_I2C1_DMA_PRIORITY         3
#define STM32_I2C_I2C2_DMA_PRIORITY         3
#define STM32_I2C_I2C3_DMA_PRIORITY         3
#define STM32_I2C_I2C4_DMA_PRIORITY         3
#define STM32_I2C_DMA_ERROR_HOOK(i2cp)      osalSysHalt("DMA failure")

/*===========================================================================*/
/* Serial — USART1 enabled for MCUboot serial recovery.                      */
/*===========================================================================*/
#define STM32_SERIAL_USE_USART1             TRUE
#define STM32_SERIAL_USE_USART2             FALSE
#define STM32_SERIAL_USE_USART3             FALSE
#define STM32_SERIAL_USE_UART4              FALSE
#define STM32_SERIAL_USE_UART5              FALSE
#define STM32_SERIAL_USE_USART6             FALSE
#define STM32_SERIAL_USE_UART7              FALSE
#define STM32_SERIAL_USE_UART8              FALSE

/*===========================================================================*/
/* SPI — disabled.                                                           */
/*===========================================================================*/
#define STM32_SPI_USE_SPI1                  FALSE
#define STM32_SPI_USE_SPI2                  FALSE
#define STM32_SPI_USE_SPI3                  FALSE
#define STM32_SPI_USE_SPI4                  FALSE
#define STM32_SPI_USE_SPI5                  FALSE
#define STM32_SPI_USE_SPI6                  FALSE
#define STM32_SPI_SPI1_RX_DMA_STREAM        STM32_DMA_STREAM_ID(2, 0)
#define STM32_SPI_SPI1_TX_DMA_STREAM        STM32_DMA_STREAM_ID(2, 3)
#define STM32_SPI_SPI2_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 3)
#define STM32_SPI_SPI2_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 4)
#define STM32_SPI_SPI3_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 0)
#define STM32_SPI_SPI3_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 7)
#define STM32_SPI_SPI1_DMA_PRIORITY         1
#define STM32_SPI_SPI2_DMA_PRIORITY         1
#define STM32_SPI_SPI3_DMA_PRIORITY         1
#define STM32_SPI_SPI1_IRQ_PRIORITY         10
#define STM32_SPI_SPI2_IRQ_PRIORITY         10
#define STM32_SPI_SPI3_IRQ_PRIORITY         10
#define STM32_SPI_DMA_ERROR_HOOK(spip)      osalSysHalt("DMA failure")

/*===========================================================================*/
/* ST (SysTick) driver.                                                      */
/*===========================================================================*/
#define STM32_ST_IRQ_PRIORITY               8
#define STM32_ST_USE_TIMER                  2

/*===========================================================================*/
/* USB — disabled.                                                           */
/*===========================================================================*/
#define STM32_USB_USE_OTG1                  FALSE
#define STM32_USB_USE_OTG2                  FALSE
#define STM32_USB_OTG1_IRQ_PRIORITY         14
#define STM32_USB_OTG2_IRQ_PRIORITY         14
#define STM32_USB_OTG1_RX_FIFO_SIZE         512
#define STM32_USB_OTG2_RX_FIFO_SIZE         1024

/*===========================================================================*/
/* WDG — disabled.                                                           */
/*===========================================================================*/
#define STM32_WDG_USE_IWDG                  FALSE

/*===========================================================================*/
/* WSPI — disabled (Discovery uses SD card, not QSPI flash).                 */
/*===========================================================================*/
#define STM32_WSPI_USE_QUADSPI1             FALSE
#define STM32_WSPI_QUADSPI1_DMA_STREAM      STM32_DMA_STREAM_ID(2, 7)
#define STM32_WSPI_QUADSPI1_PRESCALER_VALUE 2

// nanoFramework overlay driver settings
#include "mcuconf_nf.h"

#endif /* MCUCONF_H */
