//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _MCUCONF_NF_H_
#define _MCUCONF_NF_H_


/*
 * FSMC driver system settings.
 */
#define STM32_FSMC_USE_FSMC1                TRUE
#define STM32_FSMC_FSMC1_IRQ_PRIORITY       10
#define STM32_FSMC_DMA_CHN                  0x03010201

/*
 * FSMC SDRAM driver system settings.
 */
#define STM32_USE_FSMC_SDRAM                TRUE
#define STM32_SDRAM_USE_FSMC_SDRAM1         FALSE
#define STM32_SDRAM_USE_FSMC_SDRAM2         TRUE

#endif // _MCUCONF_NF_H_

