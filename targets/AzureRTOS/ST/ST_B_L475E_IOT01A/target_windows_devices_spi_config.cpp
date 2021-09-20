//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <win_dev_spi_native_target.h>

//////////
// SPI1 //
//////////

// Note on ST_B_L457EIOT01A SPI1 connects to CN1 ARDUINO connector

// pin configuration for SPI1 on GPIOA AF5
// port for SPI1_SCK  is: GPIOA pin 5 : ARD.D13-SPI1_SCK/LED1 pin CN1_6
// port for SPI1_MISO is: GPIOA pin 6 : ARD.D12-SPI1_MISO     pin CN1_5
// port for SPI1_MOSI is: GPIOA pin 7 : ARD.D11-SPI1_MOSI/PWM pin CN1_4

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(1, GPIOA, 5, GPIOA, 6, GPIOA, 7, 5)

//////////
// SPI2 //
//////////

// Note on ST_B_L457EIOT01A SPI2 connects to PMOD port (with defaault solder jumper settings)

// pin configuration for SPI2 on GPIOD AF5
// port for SPI2_SCK  is: GPIOD pin 1 PMOD-SPI2_SCK             PMOD_4
// port for SPI2_MISO is: GPIOD pin 3 PMOD-UART2_CTS/SPI2_MISO  PMOD_3
// port for SPI2_MOSI is: GPIOD pin 4 PMOD-UART2_RTS/SPI2_MOSI  PMOD_2
// port for SPI2_CSN  is: GPIOD pin 5 PMOD-UART2_Tx/SPI2_CSN    PMOD_1

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(2, GPIOD, 1, GPIOD, 3, GPIOD, 4, 5)

//////////
// SPI3 //
//////////

// Note on ST_B_L457EIOT01A SPI3 connects to Bluetooth SPBTLE-RF module

// pin configuration for SPI3
// port for SPI3_SCK  is: GPIOD pin 10 INTERNAL-SPI3_SCK
// port for SPI3_MISO is: GPIOD pin 11 INTERNAL-SPI3_MISO
// port for SPI3_MOSI is: GPIOD pin 12 INTERNAL-SPI3_MOSI

// GPIO alternate pin function is 5 (see alternate function mapping table in device datasheet)
SPI_CONFIG_PINS(3, GPIOC, 13, GPIOC, 11, GPIOC, 12, 5)
