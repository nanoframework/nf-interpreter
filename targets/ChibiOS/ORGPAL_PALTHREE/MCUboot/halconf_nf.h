//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// clang-format off

/**
 * @file    halconf_nf.h
 * @brief   nanoFramework community HAL configuration for the MCUboot bootloader
 *          on ORGPAL_PALTHREE.
 *
 * Enables the nf-overlay STM32 internal flash driver (FLASHv2 LLD) which is
 * used by mcuboot_flash_map_boot.c via stm32FlashWrite() / stm32FlashErase().
 */

#ifndef HALCONF_NF_H
#define HALCONF_NF_H

#define HAL_USE_COMMUNITY                   TRUE
#define HAL_NF_USE_STM32_FLASH              TRUE

#endif /* HALCONF_NF_H */
