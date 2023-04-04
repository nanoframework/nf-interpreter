//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_spi_native_target.h>

//////////
// SPI1 //
//////////

// pin configuration for SPI1 (mapped to USART1 on this device)
// SPI1_SCK: PD2, location 1
// SPI1_MOSI: PD0, location 1
// SPI1_MISO: PD1, location 1
// SPI1_CS: PD3, location 1
// EMC encoded "port location", see Alternate Functionality Overview table in MCU datasheet

INIT_SPI_CONFIG(1, 1, 1, 1, 1)

//////////
// SPI2 //
//////////

// pin configuration for SPI1 (mapped to USART1 on this device)
// SPI2_SCK: PF8, location 1
// SPI2_MOSI: PF6, location 1
// SPI2_MISO: PF7, location 1
// SPI2_CS: PF9, location 1
// EMC encoded "port location", see Alternate Functionality Overview table in MCU datasheet

INIT_SPI_CONFIG(2, 4, 4, 4, 4)

