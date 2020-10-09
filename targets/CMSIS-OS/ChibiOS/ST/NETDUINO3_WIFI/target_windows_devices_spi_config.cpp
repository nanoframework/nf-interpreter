//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_spi_native_target.h"

//////////
// SPI1 //
//////////

// pin configuration for SPI1
// port for SCK pin is: GPIOA_CC_SCK
// port for MISO pin is: GPIOA_CC_MISO
// port for MOSI pin is: GPIOA_CC_MOSI

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(1, GPIOA, 5, GPIOA, 6, GPIOA, 7, 5)

//////////
// SPI2 //
//////////

// pin configuration for SPI2
// port for SCK pin is: GPIOB_D13
// port for MISO pin is: GPIOB_D12
// port for MOSI pin is: GPIOB_D11

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(2, GPIOB, 13, GPIOB, 14, GPIOB, 15, 5)

//////////
// SPI3 //
//////////

// pin configuration for SPI3
// port for SCK pin is: GPIOC_SPI3_CLK
// port for MISO pin is: GPIOC_SPI3_MISO
// port for MOSI pin is: GPIOC_SPI3_MOSI

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(3, GPIOC, 13, GPIOC, 11, GPIOC, 12, 5)

//////////
// SPI4 //
//////////

// pin configuration for SPI4
// port for SCK pin is: GPIOE_SPI4_SCK
// port for MISO pin is: GPIOE_SPI4_MISO
// port for MOSI pin is: GPIOE_SPI4_MOSI

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(4, GPIOE, 2, GPIOE, 13, GPIOE, 6, 5)
