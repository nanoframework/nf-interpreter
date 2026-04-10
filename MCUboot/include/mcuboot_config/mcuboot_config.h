//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot compile-time configuration for nanoFramework.
//
// This header is the single authoritative compile-time configuration for the MCUboot
// bootloader integration in nanoFramework.  It is included by all MCUboot source files
// via the include path MCUboot/include/mcuboot_config/.
//
// Per-platform overrides (alignment, sector size, etc.) are applied by including the
// platform-specific header AFTER this file:
//   - MCUboot/port/stm32/mcuboot_stm32_config.h
//   - MCUboot/port/esp32/mcuboot_esp32_config.h
//
// Upgrade strategy is selected at CMake configure time via one of:
//   -DNF_MCUBOOT_SWAP_USING_OFFSET=ON   (STM32: primary internal, secondary external)
//   -DNF_MCUBOOT_SWAP_USING_MOVE=ON     (ESP32: both slots in internal flash)
//   -DNF_MCUBOOT_OVERWRITE_ONLY=ON      (fallback: no rollback)
//
// IMAGE_NUMBER and SERIAL_RECOVERY are similarly controlled via CMake.

#ifndef __MCUBOOT_CONFIG_H__
#define __MCUBOOT_CONFIG_H__

//
// Signature algorithm — ECDSA P-256 (secp256r1).
// Chosen for small key/signature size and fast Cortex-M verification.
// The public key is compiled into the MCUboot binary; signing uses imgtool.
//
#define MCUBOOT_SIGN_EC256

//
// Upgrade strategy — exactly one of the following is defined at CMake configure time.
//
// NF_MCUBOOT_SWAP_USING_OFFSET: STM32 primary platform.
//   Primary slot: internal STM32 flash (nanoCLR code area).
//   Secondary slot: external (Q)SPI flash, SD card, or USB MSD.
//   MCUboot copies the new image from secondary to primary in-place, sector by sector,
//   using the "offset" variant that does not require a separate scratch area.
//
// NF_MCUBOOT_SWAP_USING_MOVE: ESP32.
//   Both slots reside in internal SPI flash.
//   MCUboot moves sectors without a dedicated scratch area (requires aligned slot sizes).
//
// NF_MCUBOOT_OVERWRITE_ONLY: fallback for targets with no secondary NVM.
//   No rollback support — new image overwrites primary slot directly.
//
#if defined(NF_MCUBOOT_SWAP_USING_OFFSET)
#define MCUBOOT_SWAP_USING_OFFSET 1
#elif defined(NF_MCUBOOT_SWAP_USING_MOVE)
#define MCUBOOT_SWAP_USING_MOVE 1
#elif defined(NF_MCUBOOT_OVERWRITE_ONLY)
#define MCUBOOT_OVERWRITE_ONLY
#endif

//
// Primary slot validation policy — build-type gated.
//
// Debug builds (NF_BUILD_RTM not defined):
//   MCUBOOT_VALIDATE_PRIMARY_SLOT is NOT defined.
//   Wire Protocol writes raw .NET assemblies directly to Image 1 primary slot
//   (deploy_0) without MCUboot image headers.  MCUboot must not validate Image 1
//   primary on every boot or it would reject Wire-Protocol-written content.
//   MCUboot only validates Image 1 at OTA upgrade time (when deploy_1 contains
//   a staged, signed OTA package).
//
// RTM builds (NF_BUILD_RTM defined):
//   MCUBOOT_VALIDATE_PRIMARY_SLOT is defined — every boot validates the signature
//   of the primary slot image.  Wire Protocol is disabled in RTM firmware, so
//   direct writes to deploy_0 never occur.
//
#if defined(NF_BUILD_RTM)
#define MCUBOOT_VALIDATE_PRIMARY_SLOT
#endif

//
// Flash abstraction — use the flash_area_get_sectors() API for sector enumeration.
// DEV_WITH_ERASE: flash device requires explicit erase before write.
//
#define MCUBOOT_USE_FLASH_AREA_GET_SECTORS
#define MCUBOOT_DEV_WITH_ERASE

//
// Maximum number of erasable sectors that MCUboot will track per image slot.
// Must be >= (slot_size / min_sector_size).
// 512 is sufficient for all current nanoFramework targets:
//   - STM32F76xx 128kB sectors: 960kB / 128kB = 7.5 → 8 sectors
//   - ESP32 4kB sectors: 1664kB / 4kB = 416 sectors  (< 512 ✓)
//
#define MCUBOOT_MAX_IMG_SECTORS 512

//
// Multi-image configuration.
// IMAGE_NUMBER = 2 for ALL nanoFramework targets (STM32 and ESP32).
//   Image 0 — nanoCLR firmware  (primary = clr_0, secondary = clr_1)
//   Image 1 — deployment area   (primary = deploy_0, secondary = deploy_1)
// Both images are independently managed and can be upgraded/rolled back independently.
//
#if defined(NF_MCUBOOT_IMAGE_NUMBER)
#define MCUBOOT_IMAGE_NUMBER NF_MCUBOOT_IMAGE_NUMBER
#else
#define MCUBOOT_IMAGE_NUMBER 2
#endif

//
// Dependency checking — automatically enabled for multi-image configurations.
// Enforces deploy→CLR version compatibility via IMAGE_TLV_DEPENDENCY TLV in signed images.
//
#if (MCUBOOT_IMAGE_NUMBER > 1)
#define MCUBOOT_DEPENDENCY_CHECK 1
#endif

//
// Serial recovery — optional bootloader recovery mode over UART.
// Enabled per target via -DNF_MCUBOOT_SERIAL_RECOVERY=ON at CMake configure time.
//
#if defined(NF_MCUBOOT_SERIAL_RECOVERY)
#define MCUBOOT_SERIAL 1
#endif

//
// Logging — enable MCUboot's internal log output.
// The platform port must provide MCUBOOT_LOG_MODULE_DECLARE / MCUBOOT_LOG_ERR etc.
// wrappers redirecting to the nanoFramework trace infrastructure.
//
#define MCUBOOT_HAVE_LOGGING 1

#endif // __MCUBOOT_CONFIG_H__
