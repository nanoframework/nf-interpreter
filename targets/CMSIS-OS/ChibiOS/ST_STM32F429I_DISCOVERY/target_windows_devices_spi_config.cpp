//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_spi_native_target.h"

//////////
// SPI5 //
//////////

// pin configuration for SPI5
// port for SCK pin is: LINE_LCD_DCX
// port for MISO pin is: LINE_SPI5_MISO
// port for MOSI pin is: LINE_SPI5_MOSI

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(5, GPIOF, 7, GPIOF, 8, GPIOF, 9, 5)
