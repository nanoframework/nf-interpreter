//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_spi_native_target.h>

//////////
// SPI1 //
//////////

// Note on SL_STK3701A SPI1 connects to LCD-TFT Display

// pin configuration for SPI1 on GPIOA AF5
// port for SPI1_SCK  is: PC15
// port for SPI1_MISO is: Not used
// port for SPI1_MOSI is: PC14
// CS: PC14

SPI_CONFIG_PINS(1, gpioPortC, 15, 0, -1, gpioPortC, 14, 0)

//////////
// SPI2 //
//////////

// Note on ST_B_L457EIOT01A SPI2 connects to PMOD port (with defaault solder jumper settings)

// pin configuration for SPI2 on GPIOD AF5
// port for SPI2_SCK is: GPIOD_1 PMOD-SPI2_SCK
// port for SPI2_MISO is: GPIOD_3 PMOD-UART2_CTS/SPI2_MISO
// port for SPI2_MOSI is: GPIOD_4 PMOD-UART2_RTS/SPI2_MOSI

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
// SPI_CONFIG_PINS(2, GPIOD, 1, GPIOD, 3, GPIOD, 4, 5)

//////////
// SPI3 //
//////////

// Note on ST_B_L457EIOT01A SPI3 connects to Bluetooth SPBTLE-RF module

// pin configuration for SPI3 on GPIOD AF5
// port for SPI3_SCK is: GPIOD_10 INTERNAL-SPI3_SCK
// port for SPI3_MISO is: GPIOD_11 INTERNAL-SPI3_MISO
// port for SPI3_MOSI is: GPIOD_12 INTERNAL-SPI3_MOSI

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
// SPI_CONFIG_PINS(3, GPIOC, 10, GPIOC, 11, GPIOC, 12, 5)
