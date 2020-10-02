//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Uladzimir Pylinsky aka barthess.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef HALCONF_COMMUNITY_H
#define HALCONF_COMMUNITY_H

/**
 * @brief   Enables the USBH subsystem.
 */
#if !defined(HAL_USE_USBH) || defined(__DOXYGEN__)
#define HAL_USE_USBH                TRUE
#endif


/*===========================================================================*/
/* USBH driver related settings.                                             */
/*===========================================================================*/

/* main driver */
#define HAL_USBH_PORT_DEBOUNCE_TIME                   200
#define HAL_USBH_PORT_RESET_TIMEOUT                   500
#define HAL_USBH_DEVICE_ADDRESS_STABILIZATION         20
#define HAL_USBH_CONTROL_REQUEST_DEFAULT_TIMEOUT	    OSAL_MS2I(1000)

/* MSD */
// this option is set at target_platform.h (from config file)
// #define HAL_USBH_USE_MSD                              TRUE

#define HAL_USBHMSD_MAX_LUNS                          1
#define HAL_USBHMSD_MAX_INSTANCES                     1

// #define HAL_USBH_USE_ADDITIONAL_CLASS_DRIVERS		  TRUE

/* debug */
#define USBH_DEBUG_ENABLE                             FALSE
#define USBH_DEBUG_USBHD                              USBHD1
// #define USBH_DEBUG_SD                                 SD2
#define USBH_DEBUG_BUFFER                             25000

#define USBH_DEBUG_ENABLE_TRACE                       FALSE
#define USBH_DEBUG_ENABLE_INFO                        TRUE
#define USBH_DEBUG_ENABLE_WARNINGS                    TRUE
#define USBH_DEBUG_ENABLE_ERRORS                      TRUE

#define USBH_LLD_DEBUG_ENABLE_TRACE                   FALSE
#define USBH_LLD_DEBUG_ENABLE_INFO                    TRUE
#define USBH_LLD_DEBUG_ENABLE_WARNINGS                TRUE
#define USBH_LLD_DEBUG_ENABLE_ERRORS                  TRUE

#define USBHHUB_DEBUG_ENABLE_TRACE                    FALSE
#define USBHHUB_DEBUG_ENABLE_INFO                     TRUE
#define USBHHUB_DEBUG_ENABLE_WARNINGS                 TRUE
#define USBHHUB_DEBUG_ENABLE_ERRORS                   TRUE

#define USBHMSD_DEBUG_ENABLE_TRACE                    FALSE
#define USBHMSD_DEBUG_ENABLE_INFO                     TRUE
#define USBHMSD_DEBUG_ENABLE_WARNINGS                 TRUE
#define USBHMSD_DEBUG_ENABLE_ERRORS                   TRUE

#define USBHUVC_DEBUG_ENABLE_TRACE                    FALSE
#define USBHUVC_DEBUG_ENABLE_INFO                     TRUE
#define USBHUVC_DEBUG_ENABLE_WARNINGS                 TRUE
#define USBHUVC_DEBUG_ENABLE_ERRORS                   TRUE

#define USBHFTDI_DEBUG_ENABLE_TRACE                   FALSE
#define USBHFTDI_DEBUG_ENABLE_INFO                    TRUE
#define USBHFTDI_DEBUG_ENABLE_WARNINGS                TRUE
#define USBHFTDI_DEBUG_ENABLE_ERRORS                  TRUE

#define USBHAOA_DEBUG_ENABLE_TRACE                    FALSE
#define USBHAOA_DEBUG_ENABLE_INFO                     TRUE
#define USBHAOA_DEBUG_ENABLE_WARNINGS                 TRUE
#define USBHAOA_DEBUG_ENABLE_ERRORS                   TRUE

#define USBHHID_DEBUG_ENABLE_TRACE                    FALSE
#define USBHHID_DEBUG_ENABLE_INFO                     TRUE
#define USBHHID_DEBUG_ENABLE_WARNINGS                 TRUE
#define USBHHID_DEBUG_ENABLE_ERRORS                   TRUE

#endif /* HALCONF_COMMUNITY_H */

/** @} */
