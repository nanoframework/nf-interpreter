//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Uladzimir Pylinsky aka barthess.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

/*
 * USBH driver system settings.
 */
#define STM32_OTG1_CHANNELS_NUMBER          8
#define STM32_OTG2_CHANNELS_NUMBER          12

#define STM32_USBH_USE_OTG1                 FALSE
#define STM32_OTG1_RXFIFO_SIZE              1024
#define STM32_OTG1_PTXFIFO_SIZE             128
#define STM32_OTG1_NPTXFIFO_SIZE            128

#define STM32_USBH_USE_OTG2                 TRUE
#define STM32_OTG2_RXFIFO_SIZE              2048
#define STM32_OTG2_PTXFIFO_SIZE             1024
#define STM32_OTG2_NPTXFIFO_SIZE            1024

#define STM32_USBH_MIN_QSPACE               4
#define STM32_USBH_CHANNELS_NP              4
