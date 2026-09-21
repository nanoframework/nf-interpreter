//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Uladzimir Pylinsky aka barthess.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// ChibiOS-Contrib HAL configuration for the MCUboot bootloader on ORGPAL_PALTHREE.
//
// Only the USB host stack with the mass storage class driver is enabled, and only when
// a USB MSD is an update medium (NF_FEATURE_MCUBOOT_HAS_USB_MSD). No hub support and no
// USBH debug output: both cost flash the bootloader partition does not have.

#ifndef HALCONF_COMMUNITY_H
#define HALCONF_COMMUNITY_H

#if defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD

// Enables the USBH subsystem
#define HAL_USE_USBH TRUE

/*===========================================================================*/
/* USBH driver related settings.                                             */
/*===========================================================================*/

/* main driver */
#define HAL_USBH_PORT_DEBOUNCE_TIME              200
#define HAL_USBH_PORT_RESET_TIMEOUT              500
#define HAL_USBH_DEVICE_ADDRESS_STABILIZATION    20
#define HAL_USBH_CONTROL_REQUEST_DEFAULT_TIMEOUT OSAL_MS2I(1000)

/* MSD */
#define HAL_USBH_USE_MSD TRUE

#define HAL_USBHMSD_MAX_LUNS      1
#define HAL_USBHMSD_MAX_INSTANCES 1

/* no hub, no other class drivers */
#define HAL_USBH_USE_HUB  FALSE
#define HAL_USBH_USE_FTDI FALSE
#define HAL_USBH_USE_UVC  FALSE
#define HAL_USBH_USE_AOA  FALSE
#define HAL_USBH_USE_HID  FALSE

/* debug */
#define USBH_DEBUG_ENABLE FALSE

#define USBH_DEBUG_ENABLE_TRACE    FALSE
#define USBH_DEBUG_ENABLE_INFO     FALSE
#define USBH_DEBUG_ENABLE_WARNINGS FALSE
#define USBH_DEBUG_ENABLE_ERRORS   FALSE

#define USBH_LLD_DEBUG_ENABLE_TRACE    FALSE
#define USBH_LLD_DEBUG_ENABLE_INFO     FALSE
#define USBH_LLD_DEBUG_ENABLE_WARNINGS FALSE
#define USBH_LLD_DEBUG_ENABLE_ERRORS   FALSE

#define USBHMSD_DEBUG_ENABLE_TRACE    FALSE
#define USBHMSD_DEBUG_ENABLE_INFO     FALSE
#define USBHMSD_DEBUG_ENABLE_WARNINGS FALSE
#define USBHMSD_DEBUG_ENABLE_ERRORS   FALSE

#else

#define HAL_USE_USBH     FALSE
#define HAL_USBH_USE_MSD FALSE

#endif // CONFIG_NF_FEATURE_MCUBOOT_HAS_USB_MSD

#endif /* HALCONF_COMMUNITY_H */
