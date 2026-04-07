//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2021 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SL_USBD_VENDOR_WINUSB_CONFIG_H
#define SL_USBD_VENDOR_WINUSB_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Class Configuration

// <s SL_USBD_VENDOR_WINUSB_CONFIGURATIONS> Configuration(s) to add this class instance to
// <i> Default: all
// <i> Comma separated list of configuration instances (like inst0, inst1)
// <i> that this vendor class instance will be attached to. You can
// <i> use "all" to attach the class to all configs, or use an empty
// <i> string if you do not want to attach the interface to any configuration.
#define SL_USBD_VENDOR_WINUSB_CONFIGURATIONS          "all"

// </h>

// <h> Protocol Details

// <q SL_USBD_VENDOR_WINUSB_INTERRUPT_ENDPOINTS> Add interrupt endpoints
// <i> Default: 0
// <i> Specifies whether we should add IN and OUT endpoints to this
// <i> vendor class interface.
#define SL_USBD_VENDOR_WINUSB_INTERRUPT_ENDPOINTS     0

// <o SL_USBD_VENDOR_WINUSB_INTERVAL> Endpoint interval
//   <1=> 1ms
//   <2=> 2ms
//   <4=> 4ms
//   <8=> 8ms
//   <16=> 16ms
//   <32=> 32ms
//   <64=> 64ms
//   <128=> 128ms
//   <256=> 256ms
//   <512=> 512ms
//   <1024=> 1024ms
//   <2048=> 2048ms
//   <4096=> 4096ms
//   <8192=> 8192ms
//   <16384=> 16384ms
//   <32768=> 32768ms
// <i> Default: 2
// <i> Polling interval for input/output transfers, in milliseconds.
// <i> It must be a power of 2.
#define SL_USBD_VENDOR_WINUSB_INTERVAL                2

// </h>

// <<< end of configuration section >>>
#endif // SL_USBD_VENDOR_WINUSB_CONFIG_H
