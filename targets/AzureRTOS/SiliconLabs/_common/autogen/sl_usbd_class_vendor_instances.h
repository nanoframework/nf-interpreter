//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SL_USBD_CLASS_VENDOR_INSTANCES_INIT
#define SL_USBD_CLASS_VENDOR_INSTANCES_INIT

#include <sl_usbd_class_vendor.h>

typedef enum __nfpack UsbEventType
{
    UsbEventType_Invalid = 0,
    UsbEventType_DeviceConnected = 1,
    UsbEventType_DeviceDisconnected = 2,
} UsbEventType;

#define DEVICEINTERFACE_GUID_PROP_NAME L"DeviceInterfaceGUID"

/// Length of GUID Property Name
#define DEVICEINTERFACE_GUID_PROP_NAME_LEN  sizeof (DEVICEINTERFACE_GUID_PROP_NAME)

// GUID for device class that will be reported to WinUSB (going into DeviceInterfaceGUID extended property)
#define DEVICE_CLASS_GUID_PROPERTY L"{00000000-0000-0000-0000-000000000000}"
#define DEVICE_CLASS_GUID_PROPERTY_LEN sizeof(DEVICE_CLASS_GUID_PROPERTY)

/* class numbers assigned by the USB stack after init */

extern uint8_t sl_usbd_vendor_winusb_number;
extern char UsbClassVendorDeviceInterfaceGuid[DEVICE_CLASS_GUID_PROPERTY_LEN];

/* event handlers for all vendor instances */

__WEAK void sl_usbd_vendor_winusb_on_enable_event(void);
__WEAK void sl_usbd_vendor_winusb_on_disable_event(void);
__WEAK void sl_usbd_vendor_winusb_on_setup_request_event(const sl_usbd_setup_req_t *p_setup_req);

/* init functions for all vendor instances */

#ifdef __cplusplus
extern "C" {
#endif

sl_status_t sli_usbd_vendor_winusb_init(void);

#ifdef __cplusplus
}
#endif

#endif
