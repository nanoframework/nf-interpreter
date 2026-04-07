//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SL_USBD_CDC_ACM_ACM0_CONFIG_H
#define SL_USBD_CDC_ACM_ACM0_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Class Configuration

// <s SL_USBD_CDC_ACM_ACM0_CONFIGURATIONS> Configuration(s) to add this class instance to
// <i> Default: all
// <i> Comma separated list of configuration instances (like inst0, inst1)
// <i> that this CDC ACM class instance will be attached to. You can
// <i> use "all" to attach the class to all configs, or use an empty
// <i> string if you do not want to attach the interface to any configuration.
#define SL_USBD_CDC_ACM_ACM0_CONFIGURATIONS "all"

// </h>

// <h> Protocol Details

// <o SL_USBD_CDC_ACM_ACM0_NOTIFY_INTERVAL> Line State Notification Interval (ms)
// <i> Default: 64
// <i> Line State Notification Interval (ms).
#define SL_USBD_CDC_ACM_ACM0_NOTIFY_INTERVAL 64

// </h>

// <h> Call Management

// <q SL_USBD_CDC_ACM_ACM0_CALL_MGMT_ENABLE> Enable call management
// <i> Default: 1
// <i> If set to 1, the host is informed that this ACM instance
// <i> has call management capabilities.
#define SL_USBD_CDC_ACM_ACM0_CALL_MGMT_ENABLE 1

// <o SL_USBD_CDC_ACM_ACM0_CALL_MGMT_DCI> Call management interface
//   <1=> Over DCI
//   <0=> Over CCI
// <i> Default: 1
// <i> If set to 1 (i.e. Over DCI), a dedicated DCI interface will be created
// <i> along with the CCI interface. Otherwise, only the CCI will be created
// <i> and it will be used for both data and call management.
#define SL_USBD_CDC_ACM_ACM0_CALL_MGMT_DCI 1

// </h>

// <<< end of configuration section >>>
#endif // SL_USBD_CDC_ACM_ACM0_CONFIG_H