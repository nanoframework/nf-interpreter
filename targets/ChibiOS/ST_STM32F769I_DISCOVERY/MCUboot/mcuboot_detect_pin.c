//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// boot_serial_detect_pin(), mcuboot_serial_init(), and mcuboot_serial_get_channel() for ST_STM32F769I_DISCOVERY.
//
// Detection button: USER button on PA0 (active-HIGH, external pull-down).
// boardInit() / board.c already configures PA0 as floating input.
// MCUboot delays 100 ms before calling this function, so no debounce needed.
//
// UART: USART1 PA9(TX)/PA10(RX) AF7, 115 200 baud — same as wire protocol
// serial device SD1 used by nanoCLR. ChibiOS Serial driver manages BRR and
// CR1 automatically from PCLK2 (108 MHz at 216 MHz SYSCLK, PPRE2=DIV2).

#include "mcuboot_serial_port.h"

#if defined(MCUBOOT_SERIAL)

#include "hal.h"

bool boot_serial_detect_pin(void)
{
    // PA0: floating input, external pull-down, active-HIGH.
    // board.c / boardInit() already configures this pin as input.
    return palReadPad(GPIOA, 0U) != 0U;
}

void mcuboot_serial_init(void)
{
    // USART1 PA9(TX)/PA10(RX) AF7, 115 200 baud, 8N1.
    static const SerialConfig serialConfig = {
        .speed = 115200U,
        .cr1 = 0U,
        .cr2 = USART_CR2_STOP1_BITS,
        .cr3 = 0U,
    };
    sdStart(&SD1, &serialConfig);
}

// Return the serial channel used for serial recovery transport.
// SD1 is USART1 (PA9/PA10) configured by mcuboot_serial_init().
BaseChannel *mcuboot_serial_get_channel(void)
{
    return (BaseChannel *)&SD1;
}

#endif // MCUBOOT_SERIAL
