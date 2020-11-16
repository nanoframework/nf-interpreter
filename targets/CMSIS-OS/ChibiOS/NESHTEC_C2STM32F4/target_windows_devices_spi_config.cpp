//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <win_dev_spi_native_target.h>

//////////
// SPI3 //
//////////

// pin configuration for SPI3
// port for SCK pin is: GPIOC  (=PC10)
// port for MISO pin is: GPIOC (=PC11)
// port for MOSI pin is: GPIOC (=PC12)

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(3, GPIOC, 10, GPIOC, 11, GPIOC, 12, 5)
