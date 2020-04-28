//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_spi_native_target.h"

//////////
// SPI2 //
//////////

// pin configuration for SPI2
// port for SCK pin is: LINE_ARD_D13
// port for SDA pin is: LINE_ARD_D12
// port for SDA pin is: LINE_ARD_D12

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(2,LINE_ARD_D13, LINE_ARD_D12, LINE_ARD_D12, 5)
