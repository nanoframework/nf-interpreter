//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Uladzimir Pylinsky aka barthess.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// ChibiOS-Contrib MCU settings for the MCUboot bootloader on ORGPAL_PALTHREE.
// USB host on OTG_HS (USBHD2), same as the nanoCLR configuration; OTG_FS stays the
// USB CDC device used for serial recovery.

#ifndef MCUCONF_COMMUNITY_H
#define MCUCONF_COMMUNITY_H

// USBH driver system settings
#define STM32_OTG_FS_CHANNELS_NUMBER 8
#define STM32_OTG_HS_CHANNELS_NUMBER 12

#define STM32_USBH_USE_OTG1      FALSE
#define STM32_OTG1_RXFIFO_SIZE   1024
#define STM32_OTG1_PTXFIFO_SIZE  128
#define STM32_OTG1_NPTXFIFO_SIZE 128

#if defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD
#define STM32_USBH_USE_OTG2 TRUE
#else
#define STM32_USBH_USE_OTG2 FALSE
#endif
#define STM32_OTG2_RXFIFO_SIZE   2048
#define STM32_OTG2_PTXFIFO_SIZE  1024
#define STM32_OTG2_NPTXFIFO_SIZE 1024

#define STM32_USBH_MIN_QSPACE  4
#define STM32_USBH_CHANNELS_NP 4

#endif // MCUCONF_COMMUNITY_H
