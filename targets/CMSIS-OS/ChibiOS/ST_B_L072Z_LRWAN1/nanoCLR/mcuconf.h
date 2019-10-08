//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) 2006..2015 Giovanni Di Sirio.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef MCUCONF_H
#define MCUCONF_H

/*
 * STM32L0xx drivers configuration.
 * The following settings override the default settings present in
 * the various device driver implementation headers.
 * Note that the settings for each driver only have effect if the whole
 * driver is enabled in halconf.h.
 *
 * IRQ priorities:
 * 3...0       Lowest...Highest.
 *
 * DMA priorities:
 * 0...3        Lowest...Highest.
 */

#define STM32L0xx_MCUCONF

/*
 * HAL driver system settings.
 */
#define STM32_NO_INIT                       FALSE
#define STM32_VOS                           STM32_VOS_1P8
#define STM32_PVD_ENABLE                    FALSE
#define STM32_PLS                           STM32_PLS_LEV0
#define STM32_HSI16_ENABLED                 TRUE
#define STM32_HSI16_DIVIDER_ENABLED         FALSE
#define STM32_LSI_ENABLED                   FALSE
#define STM32_HSE_ENABLED                   FALSE
#define STM32_LSE_ENABLED                   TRUE
#define STM32_ADC_CLOCK_ENABLED             TRUE
#define STM32_USB_CLOCK_ENABLED             TRUE
#define STM32_MSIRANGE                      STM32_MSIRANGE_2M
#define STM32_SW                            STM32_SW_PLL
#define STM32_PLLSRC                        STM32_PLLSRC_HSI16
#define STM32_PLLMUL_VALUE                  4
#define STM32_PLLDIV_VALUE                  2
#define STM32_HPRE                          STM32_HPRE_DIV1
#define STM32_PPRE1                         STM32_PPRE1_DIV1
#define STM32_PPRE2                         STM32_PPRE2_DIV1
#define STM32_MCOSEL                        STM32_MCOSEL_NOCLOCK
#define STM32_MCOPRE                        STM32_MCOPRE_DIV1
#define STM32_RTCSEL                        STM32_RTCSEL_LSE
#define STM32_RTCPRE                        STM32_RTCPRE_DIV2
#define STM32_USART1SEL                     STM32_USART1SEL_APB
#define STM32_USART2SEL                     STM32_USART2SEL_APB
#define STM32_LPUART1SEL                    STM32_LPUART1SEL_APB
#define STM32_I2C1SEL                       STM32_I2C1SEL_APB
#define STM32_I2C3SEL                       STM32_I2C3SEL_APB
#define STM32_LPTIM1SEL                     STM32_LPTIM1SEL_APB
#define STM32_HSI48SEL                      STM32_HSI48SEL_HSI48

/*
 * IRQ system settings.
 */
#define STM32_IRQ_EXTI0_1_PRIORITY          3
#define STM32_IRQ_EXTI2_3_PRIORITY          3
#define STM32_IRQ_EXTI4_15_PRIORITY         3
#define STM32_IRQ_EXTI16_PRIORITY           3
#define STM32_IRQ_EXTI17_20_PRIORITY        3
#define STM32_IRQ_EXTI21_22_PRIORITY        3

/*
 * ADC driver system settings.
 * Note, IRQ is shared with EXT channels 21 and 22.
 */
#define STM32_ADC_USE_ADC1                  FALSE
#define STM32_ADC_ADC1_CKMODE               STM32_ADC_CKMODE_ADCCLK
#define STM32_ADC_ADC1_DMA_PRIORITY         2
#define STM32_ADC_ADC1_DMA_IRQ_PRIORITY     2
#define STM32_ADC_ADC1_DMA_STREAM           STM32_DMA_STREAM_ID(1, 1)
#define STM32_ADC_PRESCALER_VALUE           1

/*
 * DAC driver system settings.
 */
#define STM32_DAC_DUAL_MODE                 FALSE
#define STM32_DAC_USE_DAC1_CH1              FALSE
#define STM32_DAC_USE_DAC1_CH2              FALSE
#define STM32_DAC_DAC1_CH1_IRQ_PRIORITY     10
#define STM32_DAC_DAC1_CH2_IRQ_PRIORITY     10
#define STM32_DAC_DAC1_CH1_DMA_PRIORITY     2
#define STM32_DAC_DAC1_CH2_DMA_PRIORITY     2
#define STM32_DAC_DAC1_CH1_DMA_STREAM       STM32_DMA_STREAM_ID(1, 2)
#define STM32_DAC_DAC1_CH2_DMA_STREAM       STM32_DMA_STREAM_ID(1, 4)

/*
 * GPT driver system settings.
 */
#define STM32_GPT_USE_TIM2                  FALSE
#define STM32_GPT_TIM2_IRQ_PRIORITY         2
#define STM32_GPT_USE_TIM3                  FALSE
#define STM32_GPT_TIM3_IRQ_PRIORITY         2
#define STM32_GPT_USE_TIM21                 FALSE
#define STM32_GPT_TIM21_IRQ_PRIORITY        2
#define STM32_GPT_USE_TIM22                 FALSE
#define STM32_GPT_TIM22_IRQ_PRIORITY        2

/*
 * I2C driver system settings.
 */
#define STM32_I2C_USE_I2C1                  FALSE
#define STM32_I2C_USE_I2C2                  FALSE
#define STM32_I2C_USE_I2C3                  FALSE
#define STM32_I2C_BUSY_TIMEOUT              50
#define STM32_I2C_I2C1_IRQ_PRIORITY         3
#define STM32_I2C_I2C2_IRQ_PRIORITY         3
#define STM32_I2C_I2C3_IRQ_PRIORITY         3
#define STM32_I2C_USE_DMA                   TRUE
#define STM32_I2C_I2C1_DMA_PRIORITY         1
#define STM32_I2C_I2C2_DMA_PRIORITY         1
#define STM32_I2C_I2C3_DMA_PRIORITY         1
#define STM32_I2C_I2C1_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 7)
#define STM32_I2C_I2C1_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 2)
#define STM32_I2C_I2C2_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 5)
#define STM32_I2C_I2C2_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 4)
#define STM32_I2C_I2C3_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 3)
#define STM32_I2C_I2C3_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 6)
#define STM32_I2C_DMA_ERROR_HOOK(i2cp)      osalSysHalt("DMA failure")

/*
 * ICU driver system settings.
 */
#define STM32_ICU_USE_TIM2                  FALSE
#define STM32_ICU_TIM2_IRQ_PRIORITY         3
#define STM32_ICU_USE_TIM3                  FALSE
#define STM32_ICU_TIM3_IRQ_PRIORITY         3
#define STM32_ICU_USE_TIM21                 FALSE
#define STM32_ICU_TIM21_IRQ_PRIORITY        3
#define STM32_ICU_USE_TIM22                 FALSE
#define STM32_ICU_TIM22_IRQ_PRIORITY        3

/*
 * PWM driver system settings.
 */
#define STM32_PWM_USE_ADVANCED              FALSE
#define STM32_PWM_USE_TIM2                  FALSE
#define STM32_PWM_TIM2_IRQ_PRIORITY         3
#define STM32_PWM_USE_TIM3                  FALSE
#define STM32_PWM_TIM3_IRQ_PRIORITY         3
#define STM32_PWM_USE_TIM21                 FALSE
#define STM32_PWM_TIM21_IRQ_PRIORITY        3
#define STM32_PWM_USE_TIM22                 FALSE
#define STM32_PWM_TIM22_IRQ_PRIORITY        3
/*
 * SERIAL driver system settings.
 */
#define STM32_SERIAL_USE_USART1             FALSE
#define STM32_SERIAL_USE_USART2             TRUE
#define STM32_SERIAL_USE_UART4              FALSE
#define STM32_SERIAL_USE_UART5              FALSE
#define STM32_SERIAL_USE_LPUART1            FALSE
#define STM32_SERIAL_USART1_PRIORITY        3
#define STM32_SERIAL_USART2_PRIORITY        3
#define STM32_SERIAL_USART3_8_PRIORITY      3
#define STM32_SERIAL_LPUART1_PRIORITY       3

/*
 * SPI driver system settings.
 */
#define STM32_SPI_USE_SPI1                  FALSE
#define STM32_SPI_USE_SPI2                  FALSE
#define STM32_SPI_SPI1_DMA_PRIORITY         1
#define STM32_SPI_SPI2_DMA_PRIORITY         1
#define STM32_SPI_SPI1_IRQ_PRIORITY         1
#define STM32_SPI_SPI2_IRQ_PRIORITY         1
#define STM32_SPI_SPI1_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 2)
#define STM32_SPI_SPI1_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 3)
#define STM32_SPI_SPI2_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 4)
#define STM32_SPI_SPI2_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1, 5)
#define STM32_SPI_DMA_ERROR_HOOK(spip)      osalSysHalt("DMA failure")

/*
 * ST driver system settings.
 */
#define STM32_ST_IRQ_PRIORITY               2
#define STM32_ST_USE_TIMER                  21

/*
 * UART driver system settings.
 */
#define STM32_UART_USE_USART1               FALSE
#define STM32_UART_USE_USART2               FALSE
#define STM32_UART_USE_UART4                FALSE
#define STM32_UART_USART1_IRQ_PRIORITY      3
#define STM32_UART_USART2_IRQ_PRIORITY      3
#define STM32_UART_USART3_8_IRQ_PRIORITY    3
#define STM32_UART_USART1_DMA_PRIORITY      0
#define STM32_UART_USART2_DMA_PRIORITY      0
#define STM32_UART_USART1_RX_DMA_STREAM     STM32_DMA_STREAM_ID(1, 5)
#define STM32_UART_USART1_TX_DMA_STREAM     STM32_DMA_STREAM_ID(1, 4)
#define STM32_UART_USART2_RX_DMA_STREAM     STM32_DMA_STREAM_ID(1, 6)
#define STM32_UART_USART2_TX_DMA_STREAM     STM32_DMA_STREAM_ID(1, 7)
#define STM32_UART_UART4_RX_DMA_STREAM      STM32_DMA_STREAM_ID(1, 2)
#define STM32_UART_UART4_TX_DMA_STREAM      STM32_DMA_STREAM_ID(1, 3)
#define STM32_UART_DMA_ERROR_HOOK(uartp)    osalSysHalt("DMA failure")

/*
 * WDG driver system settings.
 */
#define STM32_WDG_USE_IWDG                  FALSE

// header for nanoFramework overlay drivers
#include "mcuconf_nf.h"

#endif /* MCUCONF_H */
