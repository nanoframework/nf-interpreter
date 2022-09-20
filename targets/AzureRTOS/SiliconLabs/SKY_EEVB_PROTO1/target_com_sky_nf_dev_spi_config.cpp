//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <com_sky_nf_dev_spi_native_target.h>

//////////
// SPI1 //
//////////

// pin configuration for SPI1 
// SPI1_SCK: PD2, location 1
// SPI1_MOSI: PD0, location 1
// SPI1_MISO: PD1, location 1
// CS: PD3 (not used in SPI configuration)
// EMC encoded "port location", see Alternate Functionality Overview table in MCU datasheet

INIT_SPI_CONFIG(1, 1, 1, 1)
