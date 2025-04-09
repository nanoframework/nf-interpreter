//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_idf.h>
#include <Esp32_DeviceMapping.h>

//  SPI
//  2 devices
//  Map pins  mosi, miso, clock
//
int8_t Esp32_SPI_DevicePinMap[MAX_SPI_DEVICES][Esp32SpiPin_Max] = {
    // SPI1 - use defaults from IDF sample
    {SPI_IOMUX_PIN_NUM_MOSI, SPI_IOMUX_PIN_NUM_MISO, SPI_IOMUX_PIN_NUM_CLK},
    // SPI2 - no pins assigned
    {-1, -1, -1}};

//  Serial
//  2 devices COM1,COM2 ( UART_NUM_0, UART_NUM_1 )
//  Map pins  Tx, RX, RTS, CTS
//  Set pins to default for UART_NUM_0
// others assign as NONE because the default pins can be shared with serial flash and PSRAM
int8_t Esp32_SERIAL_DevicePinMap[UART_NUM_MAX][Esp32SerialPin_Max] = {
    // COM 1 - pins 21, 20
    {UART_NUM_0_TXD_DIRECT_GPIO_NUM,
     UART_NUM_0_RXD_DIRECT_GPIO_NUM,
     UART_PIN_NO_CHANGE,
     UART_PIN_NO_CHANGE},

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
//  6 channels LED1 to LED6  or PWM1 to PWM6
//  Map pins Data & Clock
int8_t Esp32_LED_DevicePinMap[6] = {
    // Channels ( non assigned )
    -1, // 1
    -1, // 2
    -1, // 3
    -1, // 4
    -1, // 5
    -1, // 6
};

// We use "ADC1" for 20 logical channels
// Mapped to ESP32 controllers
// ESP32 ADC1 channels 0 - 4
//  "    ADC2 channels 5, 6 internal voltage vdd33
int8_t Esp32_ADC_DevicePinMap[TARGET_ADC_NUM_PINS] = {
 // 0  1  2  3  4  5  
    0, 1, 2, 3, 4, 5};

//  I2S
//  1 device I2S1
//  Map pins various pins. If not used, I2S_PIN_NO_CHANGE is used
int8_t Esp32_I2S_DevicePinMap[I2S_NUM_MAX][5] = {
    // No pin pre configured
    {I2S_PIN_NO_CHANGE, I2S_PIN_NO_CHANGE, I2S_PIN_NO_CHANGE, I2S_PIN_NO_CHANGE, I2S_PIN_NO_CHANGE}};
