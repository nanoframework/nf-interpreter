//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef WIZNET5500_LWIP_DRIVER_H
#define WIZNET5500_LWIP_DRIVER_H

#include "nanohal.h"

#include "lwip\netif.h"
#include "lwip\pbuf.h"
#include "lwip\dhcp.h"

#define NATIVE_PROFILE_PAL_NETWORK()
#define NATIVE_PROFILE_HAL_DRIVERS_ETHERNET()

struct WIZNET5500_LWIP_DRIVER_CONFIG
{
    uint32_t spiHandle;
    SPI_DEVICE_CONFIGURATION SPI_Config;
    GPIO_PIN INT_Pin;
};

#ifndef NETWORK_INTERFACE_COUNT
#define NETWORK_INTERFACE_COUNT 1
#endif

struct WIZNET5500_LWIP_DEVICE_CONFIG
{
    WIZNET5500_LWIP_DRIVER_CONFIG DeviceConfigs[NETWORK_INTERFACE_COUNT];

    static LPCSTR GetDriverName()
    {
        return "WIZNET5500_LWIP";
    }
};

struct WIZNET5500_LWIP_Driver
{
    static int Open(WIZNET5500_LWIP_DRIVER_CONFIG *config, int index);
    static bool Close(WIZNET5500_LWIP_DRIVER_CONFIG *config, int index);
    static bool Bind(WIZNET5500_LWIP_DRIVER_CONFIG *config, int index);
};

#endif // WIZNET5500_LWIP_DRIVER_H
