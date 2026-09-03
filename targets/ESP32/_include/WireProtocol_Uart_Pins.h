//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef WIREPROTOCOL_UART_PINS_H
#define WIREPROTOCOL_UART_PINS_H

// Define the gpio pins by target series used UART and the default baudrate.

///////////////////////////////////////////////////////////////////////////
// Baudrate for the serial port                                          //
// Can be overriden by the build parameter CONFIG_TARGET_SERIAL_BAUDRATE //
///////////////////////////////////////////////////////////////////////////
#ifndef CONFIG_TARGET_SERIAL_BAUDRATE
#define CONFIG_TARGET_SERIAL_BAUDRATE 921600
#endif

#ifdef CONFIG_IDF_TARGET_ESP32

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32
// U0RXD GPIO3
// U0TXD GPIO1
// U1RXD GPIO32
// U1TXD GPIO33
// U2RXD GPIO16
// U2TXD GPIO17

#define ESP32_WP_RX_PIN GPIO_NUM_3
#define ESP32_WP_TX_PIN GPIO_NUM_1

#elif CONFIG_IDF_TARGET_ESP32S2

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-S2
// U0RXD GPIO44
// U0TXD GPIO43
// U1RXD GPIO17
// U1TXD GPIO18

// we are using the same GPIOs as IDF UART0, so no need to reconfigure
#define ESP32_WP_RX_PIN GPIO_NUM_44
#define ESP32_WP_TX_PIN GPIO_NUM_43

#elif CONFIG_IDF_TARGET_ESP32S3

static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-S3
// U0RXD GPIO44
// U0TXD GPIO43
// U1RXD GPIO24
// U1TXD GPIO23
#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#elif CONFIG_IDF_TARGET_ESP32C3

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-C3
// U0RXD 20
// U0TXD 21

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#elif CONFIG_IDF_TARGET_ESP32C5

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-C5
// U0RXD 16
// U0TXD 17

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#elif CONFIG_IDF_TARGET_ESP32C6

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-C6
// U0RXD 16
// U0TXD 17

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#elif CONFIG_IDF_TARGET_ESP32C61

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-C61
// U0RXD 10
// U0TXD 11

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#elif CONFIG_IDF_TARGET_ESP32H2

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-H2
// U0RXD 23
// U0TXD 24

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM

#elif CONFIG_IDF_TARGET_ESP32P4

// WP uses UART0
static uart_port_t ESP32_WP_UART = UART_NUM_0;

// UART pins for ESP32-P4
// U0RXD 23
// U0TXD 24

#define ESP32_WP_RX_PIN UART_NUM_0_RXD_DIRECT_GPIO_NUM
#define ESP32_WP_TX_PIN UART_NUM_0_TXD_DIRECT_GPIO_NUM
#endif

#endif // WIREPROTOCOL_UART_PINS_H
