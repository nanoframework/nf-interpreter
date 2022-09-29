//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SL_USBD_CLASS_VENDOR_INSTANCES_INIT
#define SL_USBD_CLASS_VENDOR_INSTANCES_INIT

#include <sl_usbd_class_vendor.h>

/* class numbers assigned by the USB stack after init */

extern uint8_t sl_usbd_vendor_winusb_number;

/* event handlers for all vendor instances */

__WEAK void sl_usbd_vendor_winusb_on_enable_event(void);
__WEAK void sl_usbd_vendor_winusb_on_disable_event(void);
__WEAK void sl_usbd_vendor_winusb_on_setup_request_event(const sl_usbd_setup_req_t *p_setup_req);

/* init functions for all vendor instances */

void sli_usbd_vendor_winusb_init(void);

#endif
