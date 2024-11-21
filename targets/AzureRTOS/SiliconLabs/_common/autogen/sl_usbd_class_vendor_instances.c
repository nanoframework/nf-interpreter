//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

//****************************************************************************
// Includes.

#include <sl_status.h>
#include <nanoHAL_v2.h>
#include <target_platform.h>
#include <sl_usbd_core.h>
#include <sl_usbd_class_hid.h>
#include <sl_usbd_class_vendor.h>

// template headers
#include <sl_usbd_configuration_instances.h>
#include <sl_usbd_class_vendor_instances.h>
#include <nano_sl_usbd_class_vendor.h>

// include config file for the instances

#include <sl_usbd_class_winusb_config.h>

// need to declare this here as extern
extern void PostManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2);
extern sl_status_t sl_usbd_vendor_update_device_product_string(const char *product_string);

// storage for USB class vendor description
char UsbClassVendorDescription[GECKO_DEVICE_CLASS_VENDOR_DESCRIPTION_PROPERTY_LEN + 1];
char UsbClassVendorDeviceInterfaceGuid[DEVICE_CLASS_GUID_PROPERTY_LEN];

// flag for USB WinUSB intialized
static bool usbdVendorWinusbInited = false;

//****************************************************************************
// Function declarations.

/* callback prototypes for winusb instance */

void sli_usbd_vendor_winusb_enable(uint8_t class_nbr);

void sli_usbd_vendor_winusb_disable(uint8_t class_nbr);

void sli_usbd_vendor_winusb_setup_req(uint8_t class_nbr, const sl_usbd_setup_req_t *p_setup_req);

//****************************************************************************
// Global variables.

/* variables for winusb instance */

uint8_t sl_usbd_vendor_winusb_number = 0;

sl_usbd_vendor_callbacks_t sli_usbd_vendor_winusb_callbacks = {
    sli_usbd_vendor_winusb_enable,
    sli_usbd_vendor_winusb_disable,
    sli_usbd_vendor_winusb_setup_req,
};

//****************************************************************************
// Callback functions.

void sli_usbd_vendor_winusb_enable(uint8_t class_nbr)
{
    (void)&class_nbr;

    sl_usbd_vendor_winusb_on_enable_event();

    return;
}

void sli_usbd_vendor_winusb_disable(uint8_t class_nbr)
{
    (void)&class_nbr;

    sl_usbd_vendor_winusb_on_disable_event();

    return;
}

void sli_usbd_vendor_winusb_setup_req(uint8_t class_nbr, const sl_usbd_setup_req_t *p_setup_req)
{
    (void)&class_nbr;

    sl_usbd_vendor_winusb_on_setup_request_event(p_setup_req);

    return;
}

//****************************************************************************
// Global functions.

/* initialize winusb instance */
sl_status_t sli_usbd_vendor_winusb_init()
{
    bool intr_en = true;
    uint16_t interval = 0;

    uint8_t class_number = 0;
    uint8_t config_number = 0;
    sl_usbd_device_state_t deviceState;
    bool needToReinit = false;
    sl_status_t returnStatus = SL_STATUS_OK;

    char *configs = NULL;
    char *token = NULL;

    // check if already initialized
    if (usbdVendorWinusbInited)
    {
        return SL_STATUS_OK;
    }

    // get current device state
    sl_usbd_core_get_device_state(&deviceState);

    if (deviceState > SL_USBD_DEVICE_STATE_INIT)
    {
        // device is already initialized, stop USB core, **ONLY** if there isn't an active debug session 
        if (!DebugSessionIsActive())
        {
            sl_usbd_core_stop_device();

            // flag to reinit
            needToReinit = true;
        }
    }

    /* configs to attach the class instance to */
    configs = SL_USBD_VENDOR_WINUSB_CONFIGURATIONS;

    /* read interrupt enable flag */
    intr_en = SL_USBD_VENDOR_WINUSB_INTERRUPT_ENDPOINTS;

    /* read interval */
    interval = SL_USBD_VENDOR_WINUSB_INTERVAL;

    /* create vendor instance */
    if (sl_usbd_vendor_create_instance(intr_en, interval, &sli_usbd_vendor_winusb_callbacks, &class_number) !=
        SL_STATUS_OK)
    {
        // error creating instance
        returnStatus = SL_STATUS_FAIL;

        // done here
        goto exit;
    }

    /* store class number globally */
    sl_usbd_vendor_winusb_number = class_number;

    // even if not initialized, check if it's active
    bool classIsEnabled;

    if (sl_usbd_vendor_is_enabled(class_number, &classIsEnabled) != SL_STATUS_OK)
    {
        // error getting class enabled status
        returnStatus = SL_STATUS_FAIL;

        // done here
        goto exit;
    }

    if (classIsEnabled)
    {
        // already active, disable it to add new configuration
        // sl_usbd_vendor_disable
        // usbd_vendor_disable(sl_usbd_configuration_config0_number, void    *p_if_class_arg)
    }

    // tokenize configs by "," and spaces
    token = strtok(configs, ", ");

    // loop over tokens
    while (token != NULL)
    {

        /* add to config0? */
        if (!strcmp(token, "config0") || !strcmp(token, "all"))
        {
            config_number = sl_usbd_configuration_config0_number;
            if (sl_usbd_vendor_add_to_configuration(class_number, config_number) != SL_STATUS_OK)
            {
                // error adding class to configuration
                returnStatus = SL_STATUS_FAIL;

                // done here
                goto exit;
            }
        }

        /* next token */
        token = strtok(NULL, ", ");
    }

#if HAL_WP_USE_USB_CDC == FALSE
    // no USB CDC so this won't be a composite device
    // need to set the description here from USB Class vendor description
    sl_usbd_vendor_update_device_product_string((const char *)UsbClassVendorDescription);

    // also need to adjust the class number for the Microsoft extend property
    class_number--;
#endif

    // add device class GUID to WinUSB properties
    if (sl_usbd_vendor_add_microsoft_ext_property(
            class_number,
            SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_SZ,
            (const uint8_t *)DEVICEINTERFACE_GUID_PROP_NAME,
            DEVICEINTERFACE_GUID_PROP_NAME_LEN,
            (const uint8_t *)UsbClassVendorDeviceInterfaceGuid,
            sizeof(UsbClassVendorDeviceInterfaceGuid)) != SL_STATUS_OK)
    {
        // error adding GUID property
        returnStatus = SL_STATUS_FAIL;

        // done here
        goto exit;
    }
    else
    {
        // all good here, update flag
        usbdVendorWinusbInited = true;
    }

exit:

    // reinit USB core if needed
    if (needToReinit)
    {
        sl_usbd_core_start_device();
    }

    return returnStatus;
}

void sl_usbd_vendor_winusb_on_enable_event(void)
{
}

void sl_usbd_vendor_winusb_on_disable_event(void)
{
}

void sl_usbd_vendor_winusb_on_setup_request_event(const sl_usbd_setup_req_t *p_setup_req)
{
    (void)p_setup_req;
}

// USB bus events.
void sl_usbd_on_bus_event(sl_usbd_bus_event_t event)
{
    switch (event)
    {
        case SL_USBD_EVENT_BUS_CONNECT:
            // called when usb cable is inserted in a host controller
            PostManagedEvent(EVENT_USB, 0, UsbEventType_DeviceConnected, 0);
            break;

        case SL_USBD_EVENT_BUS_DISCONNECT:
            // called when usb cable is removed from a host controller
            PostManagedEvent(EVENT_USB, 0, UsbEventType_DeviceDisconnected, 0);
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
