//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <com_sky_nf_dev_spi_native_target.h>

//////////
// SPI1 //
//////////

// USART1
// SCK: PD2
// MOSI: PD0
// MISO: PD1
// CS: PD3
// EMC encoded "port location", see Alternate Functionality Overview table in MCU datasheet

INIT_SPI_CONFIG(1, 1, 1, 1, 1)

//////////
// SPI2 //
//////////

// USART2
// SCK: PF8
// MOSI: PF6
// MISO: PF7
// CS: PF9
// EMC encoded "port location", see Alternate Functionality Overview table in MCU datasheet

INIT_SPI_CONFIG(2, 4, 4, 4, 4)