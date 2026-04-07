//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nano_sl_usbd_class_vendor.h>

// need these declared here as they are scattered throughout Gecko SDK and config files
extern char UsbClassVendorDescription[GECKO_DEVICE_CLASS_VENDOR_DESCRIPTION_PROPERTY_LEN + 1];
extern char UsbClassVendorDeviceInterfaceGuid[38 * 2 + 2];
extern uint8_t sl_usbd_vendor_winusb_number;
extern "C" sl_status_t sli_usbd_vendor_winusb_init(void);

// struct representing the UART
typedef struct
{
    uint16_t RxBytesReceived;

    uint16_t TxBytesSent;

    bool WaitingForRxEvent;
    bool WaitingForTxEvent;

} NF_PAL_USB;

extern NF_PAL_USB UsbStream_PAL;
