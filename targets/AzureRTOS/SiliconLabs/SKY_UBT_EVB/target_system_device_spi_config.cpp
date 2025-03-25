//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_spi_native_target.h>

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
// SCK: PA9     2
// MOSI: PA7    2    
// MISO: PA8    2
// CS: PA10     2
// EMC encoded "port location", see Alternate Functionality Overview table in MCU datasheet

INIT_SPI_CONFIG(2, 2, 2, 2, 2)

//////////
// SPI3 //
//////////

// USART3
// SCK: PA2     0
// MOSI: PA0    0    
// MISO: PA1    0
// CS: PA3      0
// EMC encoded "port location", see Alternate Functionality Overview table in MCU datasheet

INIT_SPI_CONFIG(3, 0, 0, 0, 0)

//////////
// SPI4 //
//////////

// USART3
// SCK: PC4     0
// MOSI: PB7    0    
// MISO: PB8    0
// CS: PC5      0
// EMC encoded "port location", see Alternate Functionality Overview table in MCU datasheet

INIT_SPI_CONFIG(4, 0, 0, 0, 0)

