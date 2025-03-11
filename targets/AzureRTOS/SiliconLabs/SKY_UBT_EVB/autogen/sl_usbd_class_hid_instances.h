//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SL_USBD_CLASS_HID_INSTANCES_INIT
#define SL_USBD_CLASS_HID_INSTANCES_INIT

#include "sl_usbd_class_hid.h"

/* class numbers assigned by the USB stack after init */

extern uint8_t sl_usbd_hid_hid0_number;

/* event handlers for all HID instances */

__WEAK void sl_usbd_hid_hid0_on_enable_event(void);
__WEAK void sl_usbd_hid_hid0_on_disable_event(void);
__WEAK void sl_usbd_hid_hid0_on_get_report_desc_event(const uint8_t **p_report_ptr, uint16_t *p_report_len);
__WEAK void sl_usbd_hid_hid0_on_get_phy_desc_event(const uint8_t **p_report_ptr, uint16_t *p_report_len);
__WEAK void sl_usbd_hid_hid0_on_set_output_report_event(uint8_t report_id, uint8_t *p_report_buf, uint16_t report_len);
__WEAK void sl_usbd_hid_hid0_on_get_feature_report_event(uint8_t report_id, uint8_t *p_report_buf, uint16_t report_len);
__WEAK void sl_usbd_hid_hid0_on_set_feature_report_event(uint8_t report_id, uint8_t *p_report_buf, uint16_t report_len);
__WEAK void sl_usbd_hid_hid0_on_get_protocol_event(uint8_t *p_protocol);
__WEAK void sl_usbd_hid_hid0_on_set_protocol_event(uint8_t protocol);

/* init functions for all HID instances */

void sli_usbd_hid_hid0_init(void);

#endif
