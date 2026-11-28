//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "../_Pico/pico_spi_pins.h"

//////////
// SPI0 //
//////////

// Default Pico pin configuration for SPI0:
// SCK  = GP18
// MISO = GP16
// MOSI = GP19
PICO_SPI_CONFIG_PINS(0, 18, 16, 19)

//////////
// SPI1 //
//////////

// Default Pico pin configuration for SPI1:
// SCK  = GP10
// MISO = GP12
// MOSI = GP11
PICO_SPI_CONFIG_PINS(1, 10, 12, 11)
