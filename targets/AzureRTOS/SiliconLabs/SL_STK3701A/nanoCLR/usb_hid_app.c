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
#include "sl_usbd_class_hid.h"
#include <sl_usbd_class_hid_instances.h>

// Task configuration
#define TASK_STACK_SIZE 512u
#define TASK_PRIO       5u
#define TASK_DELAY_MS   100u

#define USB_HID_REPORT_LEN 4u

// FreeRTOS Task handle
static TX_THREAD task_handle;
uint32_t hidThreadStack[TASK_STACK_SIZE / sizeof(uint32_t)];

// Mouse report buffer.
__ALIGNED(4) static uint8_t usb_hid_report_buffer[USB_HID_REPORT_LEN];

static void hid_task(uint32_t p_arg);

// Initialize application.
void usb_device_hid_app_init(void)
{
    uint16_t status;

    // Create application task
    status = tx_thread_create(
        &task_handle,
        "USB HID task",
        hid_task,
        (uint32_t)&sl_usbd_hid_hid0_number,
        hidThreadStack,
        TASK_STACK_SIZE,
        TASK_PRIO,
        TASK_PRIO,
        TX_NO_TIME_SLICE,
        TX_AUTO_START);

    _ASSERTE(status == TX_SUCCESS);
}

// hid_task()
// Perform HID writes to host.
// @param  p_arg  Task argument pointer. Class number in this case.
static void hid_task(uint32_t p_arg)
{
    uint8_t class_nbr = *(uint8_t *)p_arg;
    bool x_is_pos = true;
    bool y_is_pos = true;
    bool conn;
    sl_status_t status;
    uint32_t xfer_len = 0;
    const uint32_t xDelay = TX_TICKS_PER_MILLISEC(TASK_DELAY_MS);

    usb_hid_report_buffer[0u] = 0u;
    usb_hid_report_buffer[1u] = 0u;

    while (true)
    {

        // Wait for device connection.
        status = sl_usbd_hid_is_enabled(class_nbr, &conn);
        _ASSERTE(status == SL_STATUS_OK);

        while (conn != true)
        {
            tx_thread_sleep(xDelay);

            status = sl_usbd_hid_is_enabled(class_nbr, &conn);

            _ASSERTE(status == SL_STATUS_OK);
        }

        // Emulates back and fourth movement.
        ((int8_t *)usb_hid_report_buffer)[2u] = (x_is_pos) ? 50 : -50;
        ((int8_t *)usb_hid_report_buffer)[3u] = (y_is_pos) ? 50 : -50;

        x_is_pos = !x_is_pos;
        y_is_pos = !y_is_pos;

        // Send report.
        status = sl_usbd_hid_write_sync(class_nbr, usb_hid_report_buffer, USB_HID_REPORT_LEN, 0u, &xfer_len);

        // Delay Task
        tx_thread_sleep(xDelay);
    }
}

// USB bus events.
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

// USB configuration events.
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

// HID mouse0 instance Enable event.
void sl_usbd_hid_hid0_on_enable_event(void)
{
    // Called when the HID device is connected to the USB host and a
    // RESET transfer succeeded.
}

// HID mouse0 instance Disable event.
void sl_usbd_hid_hid0_on_disable_event(void)
{
    // Called when the HID device is disconnected to the USB host (cable removed).
}

// Hook function to pass the HID descriptor of the mouse0 instance.
void sl_usbd_hid_hid0_on_get_report_desc_event(const uint8_t **p_report_ptr, uint16_t *p_report_len)
{
    // Called during the HID mouse0 instance initialization so the USB stack
    // can retrieve its HID descriptor.
    (void)p_report_ptr;
    (void)p_report_len;
}

// Hook function to pass the HID PHY descriptor.
void sl_usbd_hid_hid0_on_get_phy_desc_event(const uint8_t **p_report_ptr, uint16_t *p_report_len)
{
    // Called during the HID mouse0 instance initialization so the USB stack
    // can retrieve the its HID physical descriptor.
    (void)p_report_ptr;
    (void)p_report_len;
}

// Notification of a new set report received on control endpoint.
// @param  report_id     Report ID.
// @param  p_report_buf  Pointer to report buffer.
// @param  report_len    Length of report, in octets.
void sl_usbd_hid_hid0_on_set_output_report_event(uint8_t report_id, uint8_t *p_report_buf, uint16_t report_len)
{
    // This function is called when host issues a SetReport request.
    // The application can take action in function of the report content.

    (void)report_id;
    (void)p_report_buf;
    (void)report_len;
}

// Get HID feature report corresponding to report ID.
// @param  report_id     Report ID.
// @param  p_report_buf  Pointer to feature report buffer.
// @param  report_len    Length of report, in octets.
// @note   (1) Report ID must not be written into the feature report buffer.
void sl_usbd_hid_hid0_on_get_feature_report_event(uint8_t report_id, uint8_t *p_report_buf, uint16_t report_len)
{
    // This function is called when host issues a GetReport(feature) request.
    // The application can provide the report to send by copying it in p_report_buf.

    (void)report_id;
    (void)p_report_buf;
    (void)report_len;
}

// Set HID feature report corresponding to report ID.
// @param  report_id     Report ID.
// @param  p_report_buf  Pointer to feature report buffer.
// @param  report_len    Length of report, in octets.
// @note   (1) Report ID is not present in the feature report buffer.
void sl_usbd_hid_hid0_on_set_feature_report_event(uint8_t report_id, uint8_t *p_report_buf, uint16_t report_len)
{
    // This function is called when host issues a SetReport(Feature) request.
    // The application can take action in function of the provided report in p_report_buf.

    (void)report_id;
    (void)p_report_buf;
    (void)report_len;
}

// Retrieve active protocol: BOOT or REPORT protocol.
// @param  p_protocol Pointer to variable that will receive the protocol type.
void sl_usbd_hid_hid0_on_get_protocol_event(uint8_t *p_protocol)
{
    // This function is called when host issues a GetProtocol request.
    // The application should return the current protocol.
    (void)p_protocol;
}

// Store active protocol: BOOT or REPORT protocol.
// @param  protocol   Protocol.
void sl_usbd_hid_hid0_on_set_protocol_event(uint8_t protocol)
{
    // This function is called when host issues a SetProtocol request.
    // The application should apply the new protocol.
    (void)protocol;
}
