//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef __STM32L4xx_HAL_CONF_H
#define __STM32L4xx_HAL_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

#define HAL_MODULE_ENABLED
// #define HAL_ADC_MODULE_ENABLED
// #define HAL_CAN_MODULE_ENABLED
// #define HAL_CAN_LEGACY_MODULE_ENABLED
// #define HAL_COMP_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_CRC_MODULE_ENABLED
// #define HAL_CRYP_MODULE_ENABLED
// #define HAL_DAC_MODULE_ENABLED
// #define HAL_DFSDM_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
// #define HAL_FIREWALL_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_HCD_MODULE_ENABLED
// #define HAL_NAND_MODULE_ENABLED
// #define HAL_NOR_MODULE_ENABLED
// #define HAL_SRAM_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
// #define HAL_IRDA_MODULE_ENABLED
// #define HAL_IWDG_MODULE_ENABLED
// #define HAL_LCD_MODULE_ENABLED
// #define HAL_LPTIM_MODULE_ENABLED
// #define HAL_OPAMP_MODULE_ENABLED
// #define HAL_PCD_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_QSPI_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
// #define HAL_RNG_MODULE_ENABLED
#define HAL_RTC_MODULE_ENABLED
// #define HAL_SAI_MODULE_ENABLED
// #define HAL_SD_MODULE_ENABLED
// #define HAL_SMARTCARD_MODULE_ENABLED
// #define HAL_SMBUS_MODULE_ENABLED
// #define HAL_SPI_MODULE_ENABLED
// #define HAL_SWPMI_MODULE_ENABLED
// #define HAL_TIM_MODULE_ENABLED
// #define HAL_TSC_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
    // #define HAL_USART_MODULE_ENABLED
    // #define HAL_WWDG_MODULE_ENABLED

#define HSE_VALUE 8000000U

#define HSE_STARTUP_TIMEOUT 100U

#define MSI_VALUE 4000000U

#define HSI_VALUE 16000000U

#define HSI48_VALUE 48000000U

#define LSI_VALUE 32000U

#define LSE_VALUE 32768U

#define LSE_STARTUP_TIMEOUT 5000U

#define EXTERNAL_SAI1_CLOCK_VALUE 48000U

#define EXTERNAL_SAI2_CLOCK_VALUE 48000U

//########################### System Configuration #########################

#define VDD_VALUE                3300U
#define TICK_INT_PRIORITY        0x0FU
#define USE_RTOS                 0U
#define PREFETCH_ENABLE          0U
#define INSTRUCTION_CACHE_ENABLE 1U
#define DATA_CACHE_ENABLE        1U

// ########################## Assert Selection ##############################
// #define USE_FULL_ASSERT               1U

// ################## SPI peripheral configuration ##########################
// #define USE_SPI_CRC                   1U

#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32l4xx_hal_rcc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32l4xx_hal_gpio.h"
#endif // HAL_GPIO_MODULE_ENABLED

#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32l4xx_hal_dma.h"
#endif // HAL_DMA_MODULE_ENABLED

#ifdef HAL_DFSDM_MODULE_ENABLED
  #include "stm32l4xx_hal_dfsdm.h"
#endif // HAL_DFSDM_MODULE_ENABLED

#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32l4xx_hal_cortex.h"
#endif // HAL_CORTEX_MODULE_ENABLED

#ifdef HAL_ADC_MODULE_ENABLED
  #include "stm32l4xx_hal_adc.h"
#endif // HAL_ADC_MODULE_ENABLED

#ifdef HAL_CAN_MODULE_ENABLED
  #include "stm32l4xx_hal_can.h"
#endif // HAL_CAN_MODULE_ENABLED

#ifdef HAL_CAN_LEGACY_MODULE_ENABLED
  #include "Legacy/stm32l4xx_hal_can_legacy.h"
#endif // HAL_CAN_LEGACY_MODULE_ENABLED

#ifdef HAL_COMP_MODULE_ENABLED
  #include "stm32l4xx_hal_comp.h"
#endif // HAL_COMP_MODULE_ENABLED

#ifdef HAL_CRC_MODULE_ENABLED
  #include "stm32l4xx_hal_crc.h"
#endif // HAL_CRC_MODULE_ENABLED

#ifdef HAL_CRYP_MODULE_ENABLED
  #include "stm32l4xx_hal_cryp.h"
#endif // HAL_CRYP_MODULE_ENABLED

#ifdef HAL_DAC_MODULE_ENABLED
  #include "stm32l4xx_hal_dac.h"
#endif // HAL_DAC_MODULE_ENABLED

#ifdef HAL_FIREWALL_MODULE_ENABLED
  #include "stm32l4xx_hal_firewall.h"
#endif // HAL_FIREWALL_MODULE_ENABLED

#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32l4xx_hal_flash.h"
#endif // HAL_FLASH_MODULE_ENABLED

#ifdef HAL_SRAM_MODULE_ENABLED
  #include "stm32l4xx_hal_sram.h"
#endif // HAL_SRAM_MODULE_ENABLED

#ifdef HAL_NOR_MODULE_ENABLED
  #include "stm32l4xx_hal_nor.h"
#endif // HAL_NOR_MODULE_ENABLED

#ifdef HAL_NAND_MODULE_ENABLED
  #include "stm32l4xx_hal_nand.h"
#endif // HAL_NAND_MODULE_ENABLED

#ifdef HAL_I2C_MODULE_ENABLED
  #include "stm32l4xx_hal_i2c.h"
#endif // HAL_I2C_MODULE_ENABLED

#ifdef HAL_IWDG_MODULE_ENABLED
  #include "stm32l4xx_hal_iwdg.h"
#endif // HAL_IWDG_MODULE_ENABLED

#ifdef HAL_LCD_MODULE_ENABLED
  #include "stm32l4xx_hal_lcd.h"
#endif // HAL_LCD_MODULE_ENABLED

#ifdef HAL_LPTIM_MODULE_ENABLED
  #include "stm32l4xx_hal_lptim.h"
#endif // HAL_LPTIM_MODULE_ENABLED

#ifdef HAL_OPAMP_MODULE_ENABLED
  #include "stm32l4xx_hal_opamp.h"
#endif // HAL_OPAMP_MODULE_ENABLED

#ifdef HAL_PWR_MODULE_ENABLED
  #include "stm32l4xx_hal_pwr.h"
#endif // HAL_PWR_MODULE_ENABLED

#ifdef HAL_QSPI_MODULE_ENABLED
  #include "stm32l4xx_hal_qspi.h"
#endif // HAL_QSPI_MODULE_ENABLED

#ifdef HAL_RNG_MODULE_ENABLED
  #include "stm32l4xx_hal_rng.h"
#endif // HAL_RNG_MODULE_ENABLED

#ifdef HAL_RTC_MODULE_ENABLED
  #include "stm32l4xx_hal_rtc.h"
#endif // HAL_RTC_MODULE_ENABLED

#ifdef HAL_SAI_MODULE_ENABLED
  #include "stm32l4xx_hal_sai.h"
#endif // HAL_SAI_MODULE_ENABLED

#ifdef HAL_SD_MODULE_ENABLED
  #include "stm32l4xx_hal_sd.h"
#endif // HAL_SD_MODULE_ENABLED

#ifdef HAL_SMBUS_MODULE_ENABLED
  #include "stm32l4xx_hal_smbus.h"
#endif // HAL_SMBUS_MODULE_ENABLED

#ifdef HAL_SPI_MODULE_ENABLED
  #include "stm32l4xx_hal_spi.h"
#endif // HAL_SPI_MODULE_ENABLED

#ifdef HAL_SWPMI_MODULE_ENABLED
  #include "stm32l4xx_hal_swpmi.h"
#endif // HAL_SWPMI_MODULE_ENABLED

#ifdef HAL_TIM_MODULE_ENABLED
  #include "stm32l4xx_hal_tim.h"
#endif // HAL_TIM_MODULE_ENABLED

#ifdef HAL_TSC_MODULE_ENABLED
  #include "stm32l4xx_hal_tsc.h"
#endif // HAL_TSC_MODULE_ENABLED

#ifdef HAL_UART_MODULE_ENABLED
  #include "stm32l4xx_hal_uart.h"
#endif // HAL_UART_MODULE_ENABLED

#ifdef HAL_USART_MODULE_ENABLED
  #include "stm32l4xx_hal_usart.h"
#endif // HAL_USART_MODULE_ENABLED

#ifdef HAL_IRDA_MODULE_ENABLED
  #include "stm32l4xx_hal_irda.h"
#endif // HAL_IRDA_MODULE_ENABLED

#ifdef HAL_SMARTCARD_MODULE_ENABLED
  #include "stm32l4xx_hal_smartcard.h"
#endif // HAL_SMARTCARD_MODULE_ENABLED

#ifdef HAL_WWDG_MODULE_ENABLED
  #include "stm32l4xx_hal_wwdg.h"
#endif // HAL_WWDG_MODULE_ENABLED

#ifdef HAL_PCD_MODULE_ENABLED
  #include "stm32l4xx_hal_pcd.h"
#endif // HAL_PCD_MODULE_ENABLED

#ifdef HAL_HCD_MODULE_ENABLED
  #include "stm32l4xx_hal_hcd.h"
#endif // HAL_HCD_MODULE_ENABLED

#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif // __STM32L4xx_HAL_CONF_H
