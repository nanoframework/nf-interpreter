//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_spi_native_target.h"

//////////
// SPI4 //
//////////

// pin configuration for SPI4
// port for SCK pin is: LINE_SPI4_SCK   (=PE2)
// port for MISO pin is: LINE_SPI4_MISO (=PE5)
// port for MOSI pin is: LINE_SPI4_MOSI (=PE6)

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(4, GPIOE, 2, GPIOE, 5, GPIOE, 6, 5)

//////////
// SPI5 //
//////////

// pin configuration for SPI5
// port for SCK pin is: LINE_LCD_DCX
// port for MISO pin is: LINE_SPI5_MISO
// port for MOSI pin is: LINE_SPI5_MOSI

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(5, GPIOF, 7, GPIOF, 8, GPIOF, 9, 5)
