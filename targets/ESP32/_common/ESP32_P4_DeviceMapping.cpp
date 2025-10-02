//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_idf.h>
#include <Esp32_DeviceMapping.h>
#include <adc_channel.h>

//  SPI
//  2 devices
//  Map pins  mosi, miso, clock
//
int8_t Esp32_SPI_DevicePinMap[MAX_SPI_DEVICES][Esp32SpiPin_Max] = {
    // SPI1 - no pins assigned
    {-1, -1, -1},
    // SPI2 - use defaults from IDF sample
    {SPI2_IOMUX_PIN_NUM_MOSI, SPI2_IOMUX_PIN_NUM_MISO, SPI2_IOMUX_PIN_NUM_CLK}};

//  Serial
//  2 devices COM1,COM2 ( UART_NUM_0, UART_NUM_1 )
//  Map pins  Tx, RX, RTS, CTS
//  Set pins to default for UART_NUM_0
// others assign as NONE because the default pins can be shared with serial flash and PSRAM
int8_t Esp32_SERIAL_DevicePinMap[UART_NUM_MAX][Esp32SerialPin_Max] = {
    // COM 1 - pins 21, 20
    {UART_NUM_0_TXD_DIRECT_GPIO_NUM, UART_NUM_0_RXD_DIRECT_GPIO_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE},

#if defined(UART_NUM_2)
    // COM 2 - all set to UART_PIN_NO_CHANGE
    {UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE},
#endif
};

// =============================================
//  I2C
//  1 devices I2C1
//  Map pins Data & Clock
int8_t Esp32_I2C_DevicePinMap[I2C_NUM_MAX][2] = {
    // I2C1 - pins 18, 19,
    {I2C1_DATA, I2C1_CLOCK}};

// =============================================
//  LED PWM
//  16 channels LED1 to LED16  or PWM1 to PWM16
//  Map pins Data & Clock
int8_t Esp32_LED_DevicePinMap[8] = {
    // Channels ( non assigned )
    -1, // 1
    -1, // 2
    -1, // 3
    -1, // 4
    -1, // 5
    -1, // 6
    -1, // 7
    -1  // 8
};

// ESP32_P4 ADC1 with 8 channels (GPIO 16,17,18,19,20,21,22,23)
//               plus 6 channels (GPIO 49,50,51,52,53,54)

// ESP32P4 ADC1 channels 0 - 9
//         ADC2 channels 10 - 19
int8_t Esp32_ADC_DevicePinMap[TARGET_ADC_NUM_PINS] = {
    // ADC1
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23
    // ADC2
    49,
    50,
    51,
    52,
    53,
    54};

//  I2S
//  1 device I2S1
//  Map pins various pins. If not used, I2S_PIN_NO_CHANGE is used
int8_t Esp32_I2S_DevicePinMap[I2S_NUM_MAX][5] = {
    // No pin pre configured
    {I2S_PIN_NO_CHANGE, I2S_PIN_NO_CHANGE, I2S_PIN_NO_CHANGE, I2S_PIN_NO_CHANGE, I2S_PIN_NO_CHANGE}};

// SDMMC
// ESP32_P4 allows 2 sdmmc devices, allow for 4 data pins
// Set SDMMC1 to default pins and SDMMC2 to not configured
int8_t Esp32_SDMMC_DevicePinMap[CONFIG_SOC_SDMMC_NUM_SLOTS][6] = {
    // Clock, Command, D0, D1, D2, D3, D4, D5, D6, D7, D8
    {GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_4, GPIO_NUM_12, GPIO_NUM_13},
    {GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC, GPIO_NUM_NC}};
