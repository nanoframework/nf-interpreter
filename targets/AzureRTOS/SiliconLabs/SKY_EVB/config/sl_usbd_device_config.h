//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright 2018 Silicon Laboratories Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SL_USBD_DEVICE_CONFIG_H
#define SL_USBD_DEVICE_CONFIG_H

extern char *UsbSerialNumber[];

// <<< Use Configuration Wizard in Context Menu >>>

// <h> USB Device Configuration

// <o SL_USBD_DEVICE_VENDOR_ID> Device Vendor ID
// <i> Device vendor ID: Silabs.
#define SL_USBD_DEVICE_VENDOR_ID                0x10C4

// <o SL_USBD_DEVICE_PRODUCT_ID> Device Product ID
// <i> Device product ID. PID assigned to Skyworks Timing Gecko Dev Board
#define SL_USBD_DEVICE_PRODUCT_ID               0x8DAC

// <o SL_USBD_DEVICE_RELEASE_NUMBER> Device Release Number
// <i> Default: 0x0100
// <i> Device release number.
#define SL_USBD_DEVICE_RELEASE_NUMBER           0x0100

// <s SL_USBD_DEVICE_MANUFACTURER_STRING> Device Manufacturer Name
// <i> Device manufacturer string.
#define SL_USBD_DEVICE_MANUFACTURER_STRING      "Skyworks"

// <s SL_USBD_DEVICE_PRODUCT_STRING> Device Product Name
// <i> Device product string.
// OK to have this one empty as it will be updated by the managed application when calling UsbStream::NativeOpen
#define SL_USBD_DEVICE_PRODUCT_STRING           "Skyworks EVB"

// <s SL_USBD_DEVICE_SERIAL_NUMBER_STRING> Device Serial Number
// <i> Device serial number string.
#define SL_USBD_DEVICE_SERIAL_NUMBER_STRING     (const char *)&UsbSerialNumber

// <o SL_USBD_DEVICE_LANGUAGE_ID> Device Language ID
//   <SL_USBD_LANG_ID_ARABIC_SAUDI_ARABIA=> Arabic
//   <SL_USBD_LANG_ID_CHINESE_TAIWAN=> Chinese
//   <SL_USBD_LANG_ID_ENGLISH_US=> US English
//   <SL_USBD_LANG_ID_ENGLISH_UK=> UK English
//   <SL_USBD_LANG_ID_FRENCH=> French
//   <SL_USBD_LANG_ID_GERMAN=> German
//   <SL_USBD_LANG_ID_GREEK=> Greek
//   <SL_USBD_LANG_ID_ITALIAN=> Italian
//   <SL_USBD_LANG_ID_PORTUGUESE=> Portuguese
//   <SL_USBD_LANG_ID_SANSKRIT=> Sanskrit
// <i> ID of language of strings of device.
// <i> Default: USBD_LANG_ID_ENGLISH_US
#define SL_USBD_DEVICE_LANGUAGE_ID            SL_USBD_LANG_ID_ENGLISH_US

// </h>

// <<< end of configuration section >>>
#endif // SL_USBD_DEVICE_CONFIG_H
