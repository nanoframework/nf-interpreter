//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// nf_config.h stub for the standalone MCUboot bootloader binary (STM32).
//
// The nanoCLR build uses a Kconfig-generated nf_config.h with the full set of
// CONFIG_* symbols.  The standalone MCUboot bootloader does not run through
// the Kconfig/CMake configure pipeline for nanoCLR, so this stub provides
// the subset of CONFIG_NF_MCUBOOT_* symbols that mcuboot_config.h requires.
//
// This file is placed first in the bootloader target's include path so that
// #include "nf_config.h" in MCUboot/include/mcuboot_config/mcuboot_config.h
// resolves here instead of a build-directory generated file.

#ifndef NF_CONFIG_MCUBOOT_STUB_H
#define NF_CONFIG_MCUBOOT_STUB_H

// STM32 targets use swap-using-offset: primary slot in internal flash, secondary slot on external storage.
#define CONFIG_NF_MCUBOOT_SWAP_USING_OFFSET 1

// Two images: Image 0 = nanoCLR firmware, Image 1 = deployment area.
#define CONFIG_NF_MCUBOOT_IMAGE_NUMBER 2

// Image header size in bytes — must match imgtool sign --header-size.
#define CONFIG_NF_MCUBOOT_HEADER_SIZE 0x200

// CONFIG_NF_BUILD_RTM is intentionally NOT defined here.
// In the bootloader context this allows boot even when primary slot signature
// validation is skipped (debug/development builds).

// Serial recovery mode: detect BOOT button (GPIOK7, active-LOW) and enter SMP
// recovery loop if pressed. Enables MCUBOOT_SERIAL=1 and full boot_serial.c stack.
#define CONFIG_NF_MCUBOOT_SERIAL_RECOVERY        1
#define CONFIG_NF_MCUBOOT_SERIAL_DETECT_DELAY_MS 100

#endif // NF_CONFIG_MCUBOOT_STUB_H
