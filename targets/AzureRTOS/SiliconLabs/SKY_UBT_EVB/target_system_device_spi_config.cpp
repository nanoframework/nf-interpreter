//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_spi_native_target.h>

//////////
// SPI1 //
//////////

// USART1
// SPI1_SCK: PD2
// SPI1_MOSI: PD0
// SPI1_MISO: PD1
// SPI1_CS: PD3
// EMC encoded "port location", see Alternate Functionality Overview table in MCU datasheet

INIT_SPI_CONFIG(1, 1, 1, 1)

//////////
// SPI2 //
//////////

// USART2
// SPI2_SCK: PF8
// SPI2_MOSI: PF6
// SPI2_MISO: PF7
// SPI2_CS: PF9
// EMC encoded "port location", see Alternate Functionality Overview table in MCU datasheet

INIT_SPI_CONFIG(2, 4, 4, 4)

