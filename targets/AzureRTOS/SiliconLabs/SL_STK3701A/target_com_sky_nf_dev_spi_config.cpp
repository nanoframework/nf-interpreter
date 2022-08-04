//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_spi_native_target.h>

//////////
// SPI0 //
//////////

// Note: on SL_STK3701A SPI0 is available at EXP connector

// pin configuration for SPI0 (EMC encoded "port location")
// SPI0_SCK: PE12, location 0, EXP Header Pin 8
// SPI0_MOSI: PE10, location 0, EXP Header Pin 4
// SPI0_MISO: PE11, location 0, EXP Header Pin 6
// CS: PE13, EXP Header Pin 10

INIT_SPI_CONFIG(0, 0, 0, 0)

//////////
// SPI1 //
//////////

// Note: on SL_STK3701A SPI1 connects to the LCD-TFT Display

// pin configuration for SPI1 (EMC encoded "port location")
// SPI1_SCK: PC15, location 3
// SPI1_MOSI: PA14, location 6
// SPI1_MISO: (not used for LCD)
// CS: PC14 (not used in SPI configuration)

INIT_SPI_CONFIG(1, 3, 6, 0)
