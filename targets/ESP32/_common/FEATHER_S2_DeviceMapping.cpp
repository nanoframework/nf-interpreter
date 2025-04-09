//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_idf.h>
#include <Esp32_DeviceMapping.h>

//  SPI
//  2 devices (one it's not used)
//  Map pins: mosi, miso, clock
//
int8_t Esp32_SPI_DevicePinMap[MAX_SPI_DEVICES][Esp32SpiPin_Max] = {
    // SPI1 - use FeatherS2 pinout (SDO, SDI, CLK)
    {GPIO_NUM_35, GPIO_NUM_37, GPIO_NUM_36},
    // SPI2 - no pins assigned
    {-1, -1, -1}};

//  Serial
//  2 devices COM1,COM2 ( UART_NUM_0, UART_NUM_1 )
//  Map pins  Tx, RX, RTS, CTS
//  Set pins to default for UART_NUM_0
// others assign as NONE
int8_t Esp32_SERIAL_DevicePinMap[UART_NUM_MAX][Esp32SerialPin_Max] = {

    // COM 1 - GPIO43, GPIO43
    {GPIO_NUM_43, GPIO_NUM_44, UART_NUM_0_RTS_DIRECT_GPIO_NUM, UART_NUM_0_CTS_DIRECT_GPIO_NUM},

    // COM 2 - all set to UART_PIN_NO_CHANGE
    {UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE},
};

// =============================================
//  I2C
//  2 devices I2C1 & I2C2
//  Map pins Data & Clock
int8_t Esp32_I2C_DevicePinMap[I2C_NUM_MAX][2] = {
    // I2C1 - pins 8, 9
    {GPIO_NUM_8, GPIO_NUM_9},
    // I2C2 - pins 25, 26
    {I2C2_DATA, I2C2_CLOCK}};

// =============================================
//  LED PWM
//  16 channels LED1 to LED16  or PWM1 to PWM16
//  Map pins Data & Clock
int8_t Esp32_LED_DevicePinMap[TARGET_LED_NUM_PINS] = {
    // Channels ( non assigned )
    -1, // 1
    -1, // 2
    -1, // 3
    -1, // 4
    -1, // 5
    -1, // 6
    -1, // 7
    -1, // 8
    -1, // 9
    -1, // 10
    -1, // 11
    -1, // 12
    -1, // 13
    -1, // 14
    -1, // 15
    -1, // 16
};

// We use "ADC1" for 18 logical channels
// Mapped to ESP32-S2 controllers
// ESP32-S2 ADC1 channels 0 - 9
//  "       ADC2 channels 0 - 9
int8_t Esp32_ADC_DevicePinMap[TARGET_ADC_NUM_PINS] = {
    // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

//
// We use "DAC1" for 2 DAC channels on ESP32
// These are fixed pins so can't be re-assigned
//
int8_t Esp32_DAC_DevicePinMap[2] = {
    // 0   1
    25,
    26};

//  I2S
//  1 device I2S1
//  Map pins various pins. If not used, I2S_PIN_NO_CHANGE is used
int8_t Esp32_I2S_DevicePinMap[I2S_NUM_MAX][5] = {
    // No pin pre configured
    {I2S_PIN_NO_CHANGE, I2S_PIN_NO_CHANGE, I2S_PIN_NO_CHANGE, I2S_PIN_NO_CHANGE, I2S_PIN_NO_CHANGE}};
