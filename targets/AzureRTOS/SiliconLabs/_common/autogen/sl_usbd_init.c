//****************************************************************************
// Includes.

#include <target_platform.h>

#include "sl_status.h"
#include <sl_usbd_core.h>

#if GECKO_FEATURE_USBD_HID == TRUE
#include "sl_usbd_class_hid.h"
#endif

#if GECKO_FEATURE_USBD_WINUSB == TRUE
#include "sl_usbd_class_vendor.h"
#endif

#if HAL_WP_USE_USB_CDC == TRUE
#include "sl_usbd_class_cdc.h"
#include "sl_usbd_class_cdc_acm.h"
#endif

// need to fit 64bits as hex string
// Can be referenced from sl_usbd_device_config.h in the define SL_USBD_DEVICE_SERIAL_NUMBER_STRING
#define USB_SERIAL_NUMBER_LENGTH (8 * 2 + 1)
char UsbSerialNumber[USB_SERIAL_NUMBER_LENGTH];

//****************************************************************************
// Global functions.

// this function composes a string with the device Unique ID
void ComposeDeviceUID()
{
    // device Unique ID is 64 bits long
    uint8_t deviceUID[8];

    deviceUID[0] = DEVINFO->UNIQUEH >> 24;
    deviceUID[1] = DEVINFO->UNIQUEH >> 16;
    deviceUID[2] = DEVINFO->UNIQUEH >> 8;
    deviceUID[3] = DEVINFO->UNIQUEH;
    deviceUID[4] = DEVINFO->UNIQUEL >> 24;
    deviceUID[5] = DEVINFO->UNIQUEL >> 16;
    deviceUID[6] = DEVINFO->UNIQUEL >> 8;
    deviceUID[7] = DEVINFO->UNIQUEL;

    snprintf(
        UsbSerialNumber,
        sizeof(UsbSerialNumber),
        "%02X%02X%02X%02X%02X%02X%02X%02X",
        deviceUID[0],
        deviceUID[1],
        deviceUID[2],
        deviceUID[3],
        deviceUID[4],
        deviceUID[5],
        deviceUID[6],
        deviceUID[7]);
}

/* USB initialization function */
void sli_usbd_init(void)
{
    ComposeDeviceUID();

    sl_usbd_core_init();

#if GECKO_FEATURE_USBD_HID == TRUE
    sl_usbd_hid_init();
#endif

#if HAL_WP_USE_USB_CDC == TRUE
    sl_usbd_cdc_init();
    sl_usbd_cdc_acm_init();
#endif

#if GECKO_FEATURE_USBD_WINUSB == TRUE
    sl_usbd_vendor_init();
#endif
}
