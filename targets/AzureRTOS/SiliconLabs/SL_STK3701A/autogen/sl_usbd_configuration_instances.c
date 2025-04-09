//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

//****************************************************************************
// Includes.

#include <sl_status.h>
#include <sl_usbd_core.h>

/* template headers */
#include "sl_usbd_configuration_instances.h"

/* include config file for the instances */

#include <sl_usbd_config0_config.h>

//****************************************************************************
// Global variables.

/* configuration numbers assigned by the USB stack after init */

uint8_t sl_usbd_configuration_config0_number = 0;

//****************************************************************************
// Global functions.

/* initialize config0 instance */
void sli_usbd_configuration_config0_init()
{
    uint8_t attrib = 0;
    uint16_t power = 0;
    sl_usbd_device_speed_t speed = SL_USBD_DEVICE_SPEED_FULL;
    const char *name = NULL;
    uint8_t number = 0;

    /* configuration attributes */
#if SL_USB_CONFIGURATION_CONFIG0_POWER_SOURCE == 1
    attrib |= SL_USBD_DEV_ATTRIB_SELF_POWERED;
#endif
#if SL_USB_CONFIGURATION_CONFIG0_REMOTE_WAKEUP == 1
    attrib |= SL_USBD_DEV_ATTRIB_REMOTE_WAKEUP;
#endif

    /* configuration maximum power (mA) */
    power = SL_USB_CONFIGURATION_CONFIG0_MAXIMUM_POWER;

    /* configuration speed */
    speed = SL_USBD_DEVICE_SPEED_FULL;

    /* configuration name */
    name = SL_USB_CONFIGURATION_CONFIG0_NAME;

    /* create the configuration descriptor */
    sl_usbd_core_add_configuration(attrib, power, speed, name, &number);

    /* store the configuration number globally */
    sl_usbd_configuration_config0_number = number;
}
