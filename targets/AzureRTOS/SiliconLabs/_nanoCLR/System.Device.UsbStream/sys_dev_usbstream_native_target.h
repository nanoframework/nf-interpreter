//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Need to add these here, until Silabs fixes the include files.
// Also can't use the #pragma GCC diagnostic ignored "-Wundef" in C++ because there's a bug with it //
// It's reportedly fixed and will be available in GCC 13.
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define USBD_CFG_HS_EN      0
#define USBD_CFG_EP_ISOC_EN 0
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include <nano_sl_usbd_class_vendor.h>

// need these declared here as they are scattered throughout Gecko SDK and config files
extern char UsbClassVendorDescription[32 + 1];
extern char UsbClassVendorDeviceInterfaceGuid[38 * 2 + 2];
extern uint8_t sl_usbd_vendor_winusb_number;
extern "C" sl_status_t sli_usbd_vendor_winusb_init(void);

// struct representing the UART
typedef struct
{
    uint16_t RxBytesReceived;

    uint16_t TxBytesSent;

} NF_PAL_USB;

extern NF_PAL_USB UsbStream_PAL;
