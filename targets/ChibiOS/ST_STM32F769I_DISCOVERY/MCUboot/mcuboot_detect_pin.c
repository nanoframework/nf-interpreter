//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// boot_serial_detect_pin() implementation for ST_STM32F769I_DISCOVERY.
//
// Detection button: USER button on PA0 (active-HIGH, external pull-down,
// configured as floating in board.h — mirrors nanoBooter IsToRemainInBooter()).
// MCUboot delays 100 ms before calling this function (MCUBOOT_SERIAL_DETECT_DELAY),
// so no manual debounce is needed here.
//
// UART: USART1 PA9(TX) / PA10(RX), AF7, 115 200 baud.
// This matches the wire protocol serial device (SD1) used by nanoCLR.
// mcuboot_uart_init() overrides the weak no-op in mcuboot_serial_port.c and
// is called by the shared boot_serial_start() stub before the recovery loop.

#include "mcuboot_serial_port.h"

#if defined(MCUBOOT_SERIAL)

#include <stm32f7xx.h>
#include <core_cm7.h>

bool boot_serial_detect_pin(void)
{
    // Same button as nanoBooter IsToRemainInBooter(): GPIOA0, external pull-down,
    // floating mode. Button pressed = HIGH (active-HIGH).
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    __DSB();
    GPIOA->MODER &= ~(3U << (0U * 2U)); // PA0 as input (floating, matches board.h)

    return (GPIOA->IDR & (1U << 0U)) != 0U; // HIGH = pressed
}

void mcuboot_uart_init(void)
{
    // USART1: PA9 TX / PA10 RX, AF7 (same UART as wire protocol SD1 in nanoCLR).
    // PCLK2 = 108 MHz → BRR = 108 000 000 / 115 200 ≈ 938.
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    __DSB();

    // PA9 (TX) and PA10 (RX): alternate function mode (MODER bits[1:0] = 10b).
    GPIOA->MODER = (GPIOA->MODER & ~(0xFU << 18U)) | (0xAU << 18U);

    // PA9 → AFR[1] bits[7:4] = 0x7, PA10 → AFR[1] bits[11:8] = 0x7.
    GPIOA->AFR[1] = (GPIOA->AFR[1] & ~(0xFFU << 4U)) | (0x77U << 4U);

    USART1->BRR = 938U;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

#endif // MCUBOOT_SERIAL
