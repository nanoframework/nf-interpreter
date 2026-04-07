//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SL_USBD_CLASS_CDC_ACM_INSTANCES_INIT
#define SL_USBD_CLASS_CDC_ACM_INSTANCES_INIT

#include <sl_usbd_class_cdc_acm.h>

/* class numbers assigned by the USB stack after init */

extern uint8_t sl_usbd_cdc_acm_acm0_number;

/* event handlers for all CDC ACM instances */

__WEAK void sl_usbd_cdc_acm_acm0_on_enable_event(void);
__WEAK void sl_usbd_cdc_acm_acm0_on_disable_event(void);
__WEAK void sl_usbd_cdc_acm_acm0_on_line_control_event(void);
__WEAK void sl_usbd_cdc_acm_acm0_on_line_coding_event(void);

/* init functions for all CDC ACM instances */

void sli_usbd_cdc_acm_acm0_init(void);

#endif
