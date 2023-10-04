//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SL_USBD_CONFIGURATION_INSTANCES_INIT
#define SL_USBD_CONFIGURATION_INSTANCES_INIT

/* configuration numbers assigned by the USB stack after init */

extern uint8_t sl_usbd_configuration_config0_number;

/* init functions for all configuration instances */

void sli_usbd_configuration_config0_init(void);

#endif
