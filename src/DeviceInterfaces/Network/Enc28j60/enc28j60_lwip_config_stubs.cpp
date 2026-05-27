//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanohal.h>
#include <enc28j60_lwip_driver.h>

//
// Enc28J60 Driver config
//
#define ENC28J60_INT           27 // GPIO interrupt pin
#define ENC28J60_CS            26 // GPIO chip select pin
#define ENC28J60_CS_ACTIVE     FALSE
#define ENC28J60_16BIT_OP      FALSE
#define ENC28J60_16BIT_ORDER   DataBitOrder_MSB
#define ENC28J60_CLOCK_RATE_HZ 1000000 // 1mhz -> 25Mhz
#define ENC28J60_CS_SETUP_USEC 0
#define ENC28J60_CS_HOLD_USEC  0

#define ENC28J60_BUS        0 // (default)
#define ENC28J60_SPIBUSMODE SpiBusMode_master
#define ENC28J60_SPI_MODE   SpiMode_Mode0

__nfweak ENC28J60_LWIP_DEVICE_CONFIG g_ENC28J60_LWIP_Config = {{
    {
        // ENC28J60_DRIVER_CONFIG
        0,
        {// SPI_DEVICE_CONFIGURATION
         ENC28J60_BUS,
         ENC28J60_SPIBUSMODE, // Master
         ENC28J60_CS,
         ENC28J60_CS_ACTIVE, // False
         ENC28J60_SPI_MODE,  // Mode 0
         ENC28J60_16BIT_OP,  // false
         ENC28J60_16BIT_ORDER,
         ENC28J60_CLOCK_RATE_HZ,
         ENC28J60_CS_SETUP_USEC,
         ENC28J60_CS_HOLD_USEC},

        ENC28J60_INT, // Interrupt Pin
    },
}};
