//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_spi_native_target.h>

//////////
// SPI2 //
//////////

// pin configuration for SPI2
// port for SCK pin is: LINE_SPI2_SCK
// port for MISO pin is: LINE_SPI2_MISO
// port for MOSI pin is: LINE_SPI2_MOSI

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(2, GPIOB, 13, GPIOB, 14, GPIOB, 15, 5)
