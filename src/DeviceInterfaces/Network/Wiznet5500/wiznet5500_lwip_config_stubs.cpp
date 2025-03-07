//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanohal.h>
#include <wiznet5500_lwip_driver.h>

//
// Wiznet5500 Driver config
//
#define WIZNET5500_INT           34 // GPIO interrupt pin
#define WIZNET5500_CS            26 // GPIO chip select pin
#define WIZNET5500_CS_ACTIVE     FALSE
#define WIZNET5500_16BIT_OP      FALSE
#define WIZNET5500_16BIT_ORDER   DataBitOrder_MSB
#define WIZNET5500_CLOCK_RATE_HZ 1000000 // 1mhz -> 25Mhz
#define WIZNET5500_CS_SETUP_USEC 0
#define WIZNET5500_CS_HOLD_USEC  0

#define WIZNET5500_BUS        0 // (default)
#define WIZNET5500_SPIBUSMODE SpiBusMode_master
#define WIZNET5500_SPI_MODE   SpiMode_Mode0

__nfweak WIZNET5500_LWIP_DEVICE_CONFIG g_WIZNET5500_LWIP_Config = {{
    {
        // WIZNET5500_DRIVER_CONFIG
        0,
        {// SPI_DEVICE_CONFIGURATION
          WIZNET5500_BUS,
          WIZNET5500_SPIBUSMODE, // Master
          WIZNET5500_CS,
          WIZNET5500_CS_ACTIVE, // False
          WIZNET5500_SPI_MODE,  // Mode 0
          WIZNET5500_16BIT_OP,  // false
          WIZNET5500_16BIT_ORDER,
          WIZNET5500_CLOCK_RATE_HZ,
          WIZNET5500_CS_SETUP_USEC,
          WIZNET5500_CS_HOLD_USEC
        },
        WIZNET5500_INT, // Interrupt Pin
    },
}};
