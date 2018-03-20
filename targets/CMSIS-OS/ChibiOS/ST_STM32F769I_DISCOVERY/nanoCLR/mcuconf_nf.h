//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _MCUCONF_NF_H_
#define _MCUCONF_NF_H_

// FSMC driver system settings
#define STM32_FSMC_USE_FSMC1              TRUE
#define STM32_FSMC_FSMC1_IRQ_PRIORITY     10

// FSMC NAND driver system settings
#define STM32_NAND_USE_FSMC_NAND1        FALSE
#define STM32_NAND_USE_FSMC_NAND2        FALSE

// FCM SDRAM driver system settings
#define STM32_USE_FSMC_SDRAM             TRUE
#define STM32_SDRAM_USE_FSMC_SDRAM1      TRUE
#define STM32_SDRAM_USE_FSMC_SDRAM2      FALSE

// FCM SRAM driver system settings
#define STM32_USE_FSMC_SRAM              FALSE
#define STM32_SRAM_USE_FSMC_SRAM1        FALSE
#define STM32_SRAM_USE_FSMC_SRAM2        FALSE
#define STM32_SRAM_USE_FSMC_SRAM3        FALSE
#define STM32_SRAM_USE_FSMC_SRAM4        FALSE

// FCM PC card driver system settings
#define STM32_USE_FSMC_PCCARD            FALSE

#endif // _MCUCONF_NF_H_
