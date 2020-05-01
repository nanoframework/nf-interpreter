//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_spi_native_target.h"

//////////
// SPI1 //
//////////

// pin configuration for SPI1
// port for SCK pin is: LINE_ARD_D13
// port for MISO pin is: LINE_ARD_D12
// port for MOSI pin is: LINE_ARD_D11

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(1, GPIOA, 5, GPIOA, 5, GPIOA, 7, 5)
