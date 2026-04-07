//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SL_USBD_CONFIG_H
#define SL_USBD_CONFIG_H

extern char UsbClassVendorDescription[GECKO_DEVICE_CLASS_VENDOR_DESCRIPTION_PROPERTY_LEN + 1];

// <<< Use Configuration Wizard in Context Menu >>>

// <h> USB Configuration

// <q SL_USBD_AUTO_START_USB_DEVICE> Auto-start USB device
// <i> Default: 1
// <i> If enabled, the USB device will be automatically started up,
// <i> using sl_usbd_core_start_device(), by the core task (which starts
// <i> running after kernel scheduler is ready) when the USB stack is all
// <i> initialized. You can disable this config if you want to call
// <i> sl_usbd_core_start_device() manually from your code. This might
// <i> be helpful if you do not want USB to start anytime before all
// <i> your initializations are complete, or if you want to enable/disable
// <i> USB on demand.
#define SL_USBD_AUTO_START_USB_DEVICE 1

// <q SL_USBD_MSC_SCSI_64_BIT_LBA_EN> Enable SCSI 64-Bit LBA
// <i> Default: 0
// <i> MSC SCSI Configuration for enabling 64-bit LBA support.
#define SL_USBD_MSC_SCSI_64_BIT_LBA_EN 0

// </h>

// <h> USB Core Configuration

// <h> Core Pools

// <o SL_USBD_CONFIGURATION_QUANTITY> Number of configurations <1-255>
// <i> Default: 1
// <i> The total number of configurations.
#define SL_USBD_CONFIGURATION_QUANTITY 1

// <o SL_USBD_INTERFACE_QUANTITY> Number of interfaces <1-255>
// <i> Default: 10
// <i> The total number of interfaces (for all of your USB configurations).
#define SL_USBD_INTERFACE_QUANTITY 10

// <o SL_USBD_ALT_INTERFACE_QUANTITY> Number of alternate interfaces <1-255>
// <i> Default: 10
// <i> The total number of alternate interfaces (for all of your USB configurations).
// <i> Must be equal to or bigger than SL_USBD_INTERFACE_QUANTITY
#define SL_USBD_ALT_INTERFACE_QUANTITY 10

// <o SL_USBD_INTERFACE_GROUP_QUANTITY> Number of interface groups <0-255>
// <i> Default: 20
// <i> The total number of interface groups (for all of your USB configurations).
#define SL_USBD_INTERFACE_GROUP_QUANTITY 20

// <o SL_USBD_DESCRIPTOR_QUANTITY> Number of endpoint descriptors <1-255>
// <i> Default: 20
// <i> The total number of endpoint descriptors (for all of your USB configurations).
#define SL_USBD_DESCRIPTOR_QUANTITY 20

// <o SL_USBD_STRING_QUANTITY> Number of strings <0-100>
// <i> Default: 30
// <i> The total number of strings per device.
#define SL_USBD_STRING_QUANTITY 30

// <o SL_USBD_OPEN_ENDPOINTS_QUANTITY> Number of opened endpoints <2-255>
// <i> Default: 20
// <i> The total number of opened endpoints per device.
#define SL_USBD_OPEN_ENDPOINTS_QUANTITY 20

// </h>

// <h> Core Task

// <o SL_USBD_TASK_STACK_SIZE> Stack size of USBD core task in bytes
// <i> Default: 4096
// <i> Stack size in bytes of the USBD core task.
#define SL_USBD_TASK_STACK_SIZE 4096U

// <o SL_USBD_TASK_PRIORITY> Priority of USBD core task
#define SL_USBD_TASK_PRIORITY 5

// <h> USB CDC Configuration

// <h> CDC Pools

// <o SL_USBD_CDC_CLASS_INSTANCE_QUANTITY> Number of class instances <1-255>
// <i> Default: 2
// <i> Number of class instances.
#define SL_USBD_CDC_CLASS_INSTANCE_QUANTITY 2

// <o SL_USBD_CDC_CONFIGURATION_QUANTITY> Number of configurations <1-255>
// <i> Default: 1
// <i> Number of configurations.
#define SL_USBD_CDC_CONFIGURATION_QUANTITY 2

// <o SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY> Number of subclass instances <1-255>
// <i> Default: 2
// <i> Number of subclass instances.
#define SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY 2

// <o SL_USBD_CDC_DATA_INTERFACE_QUANTITY> Number of data interfaces <1-255>
// <i> Default: 2
// <i> Number of data interfaces.
#define SL_USBD_CDC_DATA_INTERFACE_QUANTITY 2

// </h>

// </h>

// <h> USB HID Configuration

// <h> HID Pools

// <o SL_USBD_HID_CLASS_INSTANCE_QUANTITY> Number of class instances <1-255>
// <i> Default: 2
// <i> Number of class instances.
#define SL_USBD_HID_CLASS_INSTANCE_QUANTITY 2

// <o SL_USBD_HID_CONFIGURATION_QUANTITY> Number of configurations <1-255>
// <i> Default: 1
// <i> Number of configurations.
#define SL_USBD_HID_CONFIGURATION_QUANTITY 1

// <o SL_USBD_HID_REPORT_ID_QUANTITY> Number of report ids <0-255>
// <i> Default: 2
// <i> Number of report ids.
#define SL_USBD_HID_REPORT_ID_QUANTITY 2

// <o SL_USBD_HID_PUSH_POP_ITEM_QUANTITY> Number of push/pop items <0-255>
// <i> Default: 0
// <i> Number of push/pop items.
#define SL_USBD_HID_PUSH_POP_ITEM_QUANTITY 0

// </h>

// <h> HID Task

// <o SL_USBD_HID_TIMER_TASK_STACK_SIZE> Stack size of USBD HID timer task in bytes
// <i> Default: 2048
// <i> HID Timer task stack size in bytes.
#define SL_USBD_HID_TIMER_TASK_STACK_SIZE 2048

// <o SL_USBD_HID_TIMER_TASK_PRIORITY> Priority of USBD HID timer task
#define SL_USBD_HID_TIMER_TASK_PRIORITY 5

// <h> USB MSC Configuration

// <h> MSC Pools

// <o SL_USBD_MSC_CLASS_INSTANCE_QUANTITY> Number of class instances <1-255>
// <i> Default: 2
// <i> Number of class instances.
#define SL_USBD_MSC_CLASS_INSTANCE_QUANTITY 2

// <o SL_USBD_MSC_CONFIGURATION_QUANTITY> Number of configurations <1-255>
// <i> Default: 1
// <i> Number of configurations.
#define SL_USBD_MSC_CONFIGURATION_QUANTITY 1

// <o SL_USBD_MSC_LUN_QUANTITY> Number of Logical Units per class instance <1-255>
// <i> Default: 2
// <i> Number of Logical Units.
#define SL_USBD_MSC_LUN_QUANTITY 2

// <o SL_USBD_MSC_DATA_BUFFER_SIZE> Size of data buffer per class instance in bytes <1-4294967295>
// <i> Default: 512
// <i> Size of data buffer in bytes.
#define SL_USBD_MSC_DATA_BUFFER_SIZE 512

// </h>

// </h>

// <h> USB Vendor Configuration

// <h> Vendor Pools

// <o SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY> Number of class instances <1-255>
// <i> Default: 2
// <i> Number of class instances.
#define SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY 2

// <o SL_USBD_VENDOR_CONFIGURATION_QUANTITY> Number of configurations <1-255>
// <i> Default: 1
// <i> Number of configurations.
#define SL_USBD_VENDOR_CONFIGURATION_QUANTITY 2

// pointer to USB Class Vendor description
#define NANO_SL_USBD_CLASS_VENDOR_DESCRIPTION (const char *)&UsbClassVendorDescription

// </h>

// </h>

// <<< end of configuration section >>>
#endif // SL_USBD_CONFIG_H
