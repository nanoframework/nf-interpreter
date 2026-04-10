//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Flash slot ID definitions for the nanoFramework MCUboot integration.
//
// These constants identify the named flash areas that MCUboot manages.
// The platform port (flash_map_stm32.c / flash_map_esp32.c) maintains a static
// table of struct flash_area descriptors indexed by these IDs.
//
// IMAGE_NUMBER = 2 for ALL nanoFramework targets:
//   Image 0 = nanoCLR firmware  (IDs 1, 2)
//   Image 1 = deployment area   (IDs 4, 5)
//
// FLASH_DEVICE_EXTERNAL_FLASH is used by the STM32 port for the secondary (update)
// slots that reside on (Q)SPI flash, SD card, or USB MSD.

#ifndef __SYSFLASH_H__
#define __SYSFLASH_H__

#include <mcuboot_config/mcuboot_config.h>

//
// Device identifiers — stored in flash_area.fa_device_id.
//
#define FLASH_DEVICE_INTERNAL_FLASH 0 // On-chip flash (primary slot for all targets)
#define FLASH_DEVICE_EXTERNAL_FLASH 1 // Off-chip storage: (Q)SPI flash, SD card, USB MSD (STM32 secondary slots)

//
// Sentinel value returned when FLASH_AREA_IMAGE_PRIMARY/SECONDARY macros receive
// an out-of-range image index.
//
#define FLASH_SLOT_DOES_NOT_EXIST 255

//
// Flash area IDs.
//
#define FLASH_AREA_BOOTLOADER        0 // MCUboot bootloader itself (read-only after flash)
#define FLASH_AREA_IMAGE_0_PRIMARY   1 // nanoCLR primary slot   (internal flash, active CLR image)
#define FLASH_AREA_IMAGE_0_SECONDARY 2 // nanoCLR secondary slot (external storage on STM32; internal on ESP32)
#define FLASH_AREA_IMAGE_SCRATCH     3 // Scratch area (swap-using-scratch strategy only; unused by nanoFramework)

#if (MCUBOOT_IMAGE_NUMBER == 2)
#define FLASH_AREA_IMAGE_1_PRIMARY                                                                                     \
    4 // Deployment primary slot   (deploy_0; Wire Protocol writes here directly in debug)
#define FLASH_AREA_IMAGE_1_SECONDARY 5 // Deployment secondary slot (deploy_1; OTA staging area)
#endif

//
// Convenience macros that map (image_index, slot) to a flash area ID.
// image_index: 0 = nanoCLR, 1 = deployment
// slot:        0 = primary, 1 = secondary
//
#if (MCUBOOT_IMAGE_NUMBER == 1)

#define FLASH_AREA_IMAGE_PRIMARY(x) (((x) == 0) ? FLASH_AREA_IMAGE_0_PRIMARY : FLASH_SLOT_DOES_NOT_EXIST)

#define FLASH_AREA_IMAGE_SECONDARY(x) (((x) == 0) ? FLASH_AREA_IMAGE_0_SECONDARY : FLASH_SLOT_DOES_NOT_EXIST)

#elif (MCUBOOT_IMAGE_NUMBER == 2)

#define FLASH_AREA_IMAGE_PRIMARY(x)                                                                                    \
    (((x) == 0) ? FLASH_AREA_IMAGE_0_PRIMARY : (((x) == 1) ? FLASH_AREA_IMAGE_1_PRIMARY : FLASH_SLOT_DOES_NOT_EXIST))

#define FLASH_AREA_IMAGE_SECONDARY(x)                                                                                  \
    (((x) == 0) ? FLASH_AREA_IMAGE_0_SECONDARY                                                                         \
                : (((x) == 1) ? FLASH_AREA_IMAGE_1_SECONDARY : FLASH_SLOT_DOES_NOT_EXIST))

#endif

#endif // __SYSFLASH_H__
