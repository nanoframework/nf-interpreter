/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#ifndef _AZURE_DEVICE_X509_CERT_CONFIG_H
#define _AZURE_DEVICE_X509_CERT_CONFIG_H

// ----------------------------------------------------------------------------
// Azure IoT X509 Device Certificate
// Replace {0x00} with your formatted output from OpenSSL and xxd here
// ----------------------------------------------------------------------------
const unsigned char iot_x509_device_cert[] = {0x00};
unsigned int iot_x509_device_cert_len      = sizeof(iot_x509_device_cert);

// ----------------------------------------------------------------------------
// Azure IoT X509 Device Private Key
// Replace {0x00} with your formatted output from OpenSSL and xxd here
// ----------------------------------------------------------------------------
unsigned char iot_x509_private_key[]        = {0x00};
const unsigned int iot_x509_private_key_len = sizeof(iot_x509_private_key);

#endif