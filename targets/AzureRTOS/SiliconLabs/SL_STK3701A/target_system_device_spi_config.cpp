//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_spi_native_target.h>

//////////
// SPI1 //
//////////

// Note: on SL_STK3701A SPI1 connects to the LCD-TFT Display

// pin configuration for SPI1 (EMC encoded "port location")
// SPI1_MOSI in PC6, location 1
// SPI1_MISO: (not used for LCD)
// SPI1_SCK in PC8, location 1
// CS: PC14 (not used in SPI configuration)

SPI_CONFIG_PINS(1, gpioPortC, 6, 1, 0, -1, -1, gpioPortC, 8, 1)
