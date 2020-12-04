//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nanohal.h"

#include "lwip\netif.h"
#include "lwip\pbuf.h"
#include "lwip\dhcp.h"

#ifndef _ENC28J60_LWIP_DRIVER_H_
#define _ENC28J60_LWIP_DRIVER_H_ 1

#define NATIVE_PROFILE_PAL_NETWORK()
#define NATIVE_PROFILE_HAL_DRIVERS_ETHERNET()

struct ENC28J60_LWIP_DRIVER_CONFIG
{
    uint32_t spiHandle;
    SPI_DEVICE_CONFIGURATION SPI_Config;
    GPIO_PIN INT_Pin;
};

#ifndef NETWORK_INTERFACE_COUNT
#define NETWORK_INTERFACE_COUNT 1
#endif

struct ENC28J60_LWIP_DEVICE_CONFIG
{
    ENC28J60_LWIP_DRIVER_CONFIG DeviceConfigs[NETWORK_INTERFACE_COUNT];

    static LPCSTR GetDriverName()
    {
        return "ENC28J60_LWIP";
    }
};

struct ENC28J60_LWIP_Driver
{
    static int Open(ENC28J60_LWIP_DRIVER_CONFIG *config, int index);
    static bool Close(ENC28J60_LWIP_DRIVER_CONFIG *config, int index);
    static bool Bind(ENC28J60_LWIP_DRIVER_CONFIG *config, int index);
};

#endif /* _ENC28J60_LWIP_DRIVER_H_ */
