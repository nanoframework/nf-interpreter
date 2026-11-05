//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_spi_native_target.h>

//////////
// SPI5 //
//////////

// pin configuration for SPI5 (STMOD+ connector)
// port for SCK pin is: PF7 (STMOD+ 4)
// port for MISO pin is: PF8 (STMOD+ 3)
// port for MOSI pin is: PF9 (STMOD+ 2)

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(5, GPIOF, 7, GPIOF, 8, GPIOF, 9, 5)
