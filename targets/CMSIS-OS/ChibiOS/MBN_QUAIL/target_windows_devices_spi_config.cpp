//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_spi_native_target.h"

//////////
// SPI1 //
//////////

// pin configuration for SPI1
// port for SCK pin is: GPIOB_SPI1_CLK
// port for MISO pin is: GPIOB_SPI1_MISO
// port for MOSI pin is: GPIOB_SPI1_MOSI

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(1, GPIOB_SPI1_CLK, GPIOB_SPI1_MISO, GPIOB_SPI1_MOSI, 5)

//////////
// SPI3 //
//////////

// pin configuration for SPI3
// port for SCK pin is: GPIOC_SPI3_CLK
// port for MISO pin is: GPIOC_SPI3_MISO
// port for MOSI pin is: GPIOC_SPI3_MOSI

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(3, GPIOC_SPI3_CLK, GPIOC_SPI3_MISO, GPIOC_SPI3_MOSI, 5)
