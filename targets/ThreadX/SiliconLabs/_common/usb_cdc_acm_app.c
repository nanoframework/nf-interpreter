//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <tx_api.h>
#include <targetHAL.h>
#include <sl_usbd_core.h>
#include "sl_usbd_class_cdc.h"
#include "sl_usbd_class_cdc_acm.h"

#include "sl_usbd_class_cdc_acm_instances.h"

// Task configuration
#define TASK_STACK_SIZE 512u
#define TASK_PRIO       24u

//  sl_usbd_on_bus_event()
// @brief  USB bus events.
void sl_usbd_on_bus_event(sl_usbd_bus_event_t event)
{
    switch (event)
    {
        case SL_USBD_EVENT_BUS_CONNECT:
            // called when usb cable is inserted in a host controller
            break;

        case SL_USBD_EVENT_BUS_DISCONNECT:
            // called when usb cable is removed from a host controller
            break;

        case SL_USBD_EVENT_BUS_RESET:
            // called when the host sends reset command
            break;

        case SL_USBD_EVENT_BUS_SUSPEND:
            // called when the host sends suspend command
            break;

        case SL_USBD_EVENT_BUS_RESUME:
            // called when the host sends wake up command
            break;

        default:
            break;
    }
}

// sl_usbd_on_config_event()
// @brief  USB configuration events.
void sl_usbd_on_config_event(sl_usbd_config_event_t event, uint8_t config_nbr)
{
    (void)config_nbr;

    switch (event)
    {
        case SL_USBD_EVENT_CONFIG_SET:
            // called when the host sets a configuration after reset
            break;

        case SL_USBD_EVENT_CONFIG_UNSET:
            // called when a configuration is unset due to reset command
            break;

        default:
            break;
    }
}

// Initialize application.
void usb_device_cdc_acm_app_init(void)
{
    // need to configure the CDC with the proper WP settings
    sl_usbd_cdc_acm_line_coding_t codingBaudRate;
    codingBaudRate.baudrate = 921600;
    codingBaudRate.data_bits = 8;
    codingBaudRate.parity = SL_USBD_CDC_ACM_PARITY_NONE;
    codingBaudRate.stop_bits = SL_USBD_CDC_ACM_STOP_BIT_1;

    sl_usbd_cdc_acm_set_line_coding(sl_usbd_cdc_acm_acm0_number, &codingBaudRate);
}
