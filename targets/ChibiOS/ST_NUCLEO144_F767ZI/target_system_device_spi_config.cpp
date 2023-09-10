//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_spi_native_target.h>

//////////
// SPI1 //
//////////

// pin configuration for SPI1
// port for SCK pin is: PA5 (D13)
// port for MISO pin is: PA6 (D12)
// port for MOSI pin is: PA7 (D11)
// AF 5

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(1, GPIOA, 5, GPIOA, 6, GPIOA, 7, 5)

//////////
// SPI3 //
//////////

// pin configuration for SPI3
// port for SCK pin is: PB3 (D23)
// port for MISO pin is: PB4 (D25)
// port for MOSI pin is: PB5 (D22)
// AF 6

// GPIO alternate pin function is 6 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(3, GPIOB, 3, GPIOB, 4, GPIOB, 5, 6)
