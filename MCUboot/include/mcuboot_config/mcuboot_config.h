//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot compile-time configuration for nanoFramework.
//
// This header is the single authoritative compile-time configuration for the MCUboot
// bootloader integration in nanoFramework. It is included by all MCUboot source files
// via the include path MCUboot/include/mcuboot_config/.
//
// Per-platform overrides (alignment, sector size, etc.) are applied by including the
// platform-specific header AFTER this file:
//   - MCUboot/port/stm32/mcuboot_stm32_config.h
//   - MCUboot/port/esp32/mcuboot_esp32_config.h
//
// Upgrade strategy and other options are selected via Kconfig (nf_config.h).
// The relevant symbols are:
//   CONFIG_NF_MCUBOOT_SWAP_USING_OFFSET   (STM32: primary internal, secondary external)
//   CONFIG_NF_MCUBOOT_SWAP_USING_MOVE     (ESP32: both slots in internal flash)
//   CONFIG_NF_MCUBOOT_OVERWRITE_ONLY      (fallback: no rollback)
//   CONFIG_NF_MCUBOOT_IMAGE_NUMBER        (defaults to 2)
//   CONFIG_NF_MCUBOOT_SERIAL_RECOVERY     (optional UART recovery)
//   CONFIG_NF_MCUBOOT_HEADER_SIZE         (image header size in bytes, defaults to 0x200)

#ifndef __MCUBOOT_CONFIG_H__
#define __MCUBOOT_CONFIG_H__

#include "nf_config.h"

//
// Signature algorithm — ECDSA P-256 (secp256r1).
// Chosen for small key/signature size and fast Cortex-M verification.
// The public key is compiled into the MCUboot binary; signing uses imgtool.
//
#define MCUBOOT_SIGN_EC256

//
// Upgrade strategy — exactly one of the following is set via Kconfig.
//
// CONFIG_NF_MCUBOOT_SWAP_USING_OFFSET: STM32 primary platform.
//   Primary slot: internal STM32 flash (nanoCLR code area).
//   Secondary slot: external (Q)SPI flash, SD card, or USB MSD.
//   MCUboot copies the new image from secondary to primary in-place, sector by sector,
//   using the "offset" variant that does not require a separate scratch area.
//
// CONFIG_NF_MCUBOOT_SWAP_USING_MOVE: ESP32.
//   Both slots reside in internal SPI flash.
//   MCUboot moves sectors without a dedicated scratch area (requires aligned slot sizes).
//
// CONFIG_NF_MCUBOOT_OVERWRITE_ONLY: fallback for targets with no secondary NVM.
//   No rollback support — new image overwrites primary slot directly.
//
#if defined(CONFIG_NF_MCUBOOT_SWAP_USING_OFFSET)
#define MCUBOOT_SWAP_USING_OFFSET 1
#elif defined(CONFIG_NF_MCUBOOT_SWAP_USING_MOVE)
#define MCUBOOT_SWAP_USING_MOVE 1
#elif defined(CONFIG_NF_MCUBOOT_OVERWRITE_ONLY)
#define MCUBOOT_OVERWRITE_ONLY
#endif

//
// Primary slot validation policy — build-type gated.
//
// Debug builds (CONFIG_NF_BUILD_RTM not set):
//   MCUBOOT_VALIDATE_PRIMARY_SLOT is NOT defined.
//   Wire Protocol writes raw .NET assemblies directly to Image 1 primary slot
//   (deploy_0) without MCUboot image headers.  MCUboot must not validate Image 1
//   primary on every boot or it would reject Wire-Protocol-written content.
//   MCUboot only validates Image 1 at OTA upgrade time (when deploy_1 contains
//   a staged, signed OTA package).
//
// RTM builds (CONFIG_NF_BUILD_RTM=y):
//   MCUBOOT_VALIDATE_PRIMARY_SLOT is defined — every boot validates the signature
//   of the primary slot image.  Wire Protocol is disabled in RTM firmware, so
//   direct writes to deploy_0 never occur.
//
#if defined(CONFIG_NF_BUILD_RTM)
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
#ifndef MCUBOOT_MAX_IMG_SECTORS
#define MCUBOOT_MAX_IMG_SECTORS 512
#endif

//
// Multi-image configuration.
// IMAGE_NUMBER = 2 for ALL nanoFramework targets (STM32 and ESP32).
//   Image 0 — nanoCLR firmware  (primary = clr_0, secondary = clr_1)
//   Image 1 — deployment area   (primary = deploy_0, secondary = deploy_1)
// Both images are independently managed and can be upgraded/rolled back independently.
//
#if defined(CONFIG_NF_MCUBOOT_IMAGE_NUMBER)
#define MCUBOOT_IMAGE_NUMBER CONFIG_NF_MCUBOOT_IMAGE_NUMBER
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
// Enabled per target via CONFIG_NF_MCUBOOT_SERIAL_RECOVERY=y in Kconfig.
//
#if defined(CONFIG_NF_MCUBOOT_SERIAL_RECOVERY)
#define MCUBOOT_SERIAL 1
// Use snprintf() for version string formatting (smaller code with newlib-nano).
#define MCUBOOT_USE_SNPRINTF 1
// Boot button detect delay (ms); board debounce before reading the GPIO.
#if defined(CONFIG_NF_MCUBOOT_SERIAL_DETECT_DELAY_MS)
#define MCUBOOT_SERIAL_DETECT_DELAY CONFIG_NF_MCUBOOT_SERIAL_DETECT_DELAY_MS
#else
#define MCUBOOT_SERIAL_DETECT_DELAY 100
#endif
// Disable the LED status pin — avoids os_cputime dependency in boot_serial.c.
#define BOOT_SERIAL_REPORT_PIN -1
// Disable optional image info groups to minimise flash usage.
// Only the image upload group (group 1, cmd 1) is required for OTA recovery.
// MCUBOOT_SERIAL_IMG_GRP_IMAGE_STATE not defined → disabled
// MCUBOOT_SERIAL_IMG_GRP_HASH         not defined → disabled
// MCUBOOT_SERIAL_IMG_GRP_SLOT_INFO    not defined → disabled

// Disable per-user management group extension.
// Set to 0 (not 1) to skip bs_peruser_system_specific() call in boot_serial_input().
#define MCUBOOT_PERUSER_MGMT_GROUP_ENABLED 0

// CPU idle hook — called in boot_serial_read_console() when the read returns
// no data. Must relinquish the CPU so ChibiOS can service the USB ISR and
// allow the device to enumerate before any data arrives.
// Implemented in targets/ChibiOS/_mcuboot/mcuboot_serial_port.c.
extern void nf_mcuboot_cpu_idle(void);
#define MCUBOOT_CPU_IDLE() nf_mcuboot_cpu_idle()
#endif

//
// Image header size — must match the flash layout reserved header area and
// the imgtool sign --header-size argument.
// Configured via CONFIG_NF_MCUBOOT_HEADER_SIZE in Kconfig (default 0x200 = 512 bytes).
//
#if defined(CONFIG_NF_MCUBOOT_HEADER_SIZE)
#define MCUBOOT_HEADER_SIZE CONFIG_NF_MCUBOOT_HEADER_SIZE
#else
#define MCUBOOT_HEADER_SIZE 0x200
#endif

//
// Logging — disabled for the bare-metal STM32 bootloader.
// There is no UART or semihosting infrastructure in the bare-metal build.
// Define MCUBOOT_HAVE_LOGGING and provide mcuboot_logging.h when adding a log backend.
//
// #define MCUBOOT_HAVE_LOGGING 1

//
// Watchdog — no watchdog in the bare-metal STM32 bootloader.
// Define as a no-op; platform ports that use a watchdog should replace this.
//
#define MCUBOOT_WATCHDOG_FEED()                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)

#endif // __MCUBOOT_CONFIG_H__
