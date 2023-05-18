//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

//
//  Header file for ESP32 device pin mapping
//

#ifndef ESP32_DEVICEMAPPING_H
#define ESP32_DEVICEMAPPING_H

#include "TargetHAL_Spi.h"

// Default I2C gpio pins
#define I2C1_DATA  18
#define I2C1_CLOCK 19
#define I2C2_DATA  25
#define I2C2_CLOCK 26

// UART defines
// number of pins required to configure an UART
enum Esp32SerialPin
{
    Esp32SerialPin_Tx,
    Esp32SerialPin_Rx,
    Esp32SerialPin_Rts,
    Esp32SerialPin_Cts,
    Esp32SerialPin_Max,
};

enum Esp32SpiPin
{
    Esp32SpiPin_Mosi,
    Esp32SpiPin_Miso,
    Esp32SpiPin_Clk,
    Esp32SpiPin_Max,
};

enum Esp32_MapDeviceType
{
    DEV_TYPE_GPIO,
    DEV_TYPE_SPI,
    DEV_TYPE_I2C,
    DEV_TYPE_SERIAL,
    DEV_TYPE_LED_PWM,
    DEV_TYPE_ADC,
    DEV_TYPE_I2S,
    DEV_TYPE_DAC,
    DEV_TYPE_MAX,
};

extern int8_t Esp32_SPI_DevicePinMap[MAX_SPI_DEVICES][Esp32SpiPin_Max];
extern int8_t Esp32_I2C_DevicePinMap[I2C_NUM_MAX][2];
extern int8_t Esp32_I2S_DevicePinMap[I2S_NUM_MAX][5];
extern int8_t Esp32_SERIAL_DevicePinMap[UART_NUM_MAX][Esp32SerialPin_Max];

#if defined(CONFIG_IDF_TARGET_ESP32C3)
extern int8_t Esp32_ADC_DevicePinMap[6];
extern int8_t Esp32_LED_DevicePinMap[16];
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
extern int8_t Esp32_ADC_DevicePinMap[20];
extern int8_t Esp32_LED_DevicePinMap[8];
#else
extern int8_t Esp32_DAC_DevicePinMap[2];
extern int8_t Esp32_ADC_DevicePinMap[20];
extern int8_t Esp32_LED_DevicePinMap[16];
#endif

void Esp32_DecodeAlternateFunction(
    uint32_t alternateFunction,
    Esp32_MapDeviceType &deviceType,
    uint8_t &busIndex,
    uint16_t &pinIndex);

int Esp32_GetMappedDevicePins(Esp32_MapDeviceType deviceType, int busIndex, int pinIndex);
int Esp32_GetMappedDevicePinsWithFunction(uint32_t alternateFunction);

int Esp32_SetMappedDevicePins(uint8_t pin, int32_t alternateFunction);
void Esp32_SetMappedDevicePins(Esp32_MapDeviceType deviceType, int busIndex, int8_t pinIndex, int ioPinNumber);

int Esp32_ValidateMappedDevicePin(Esp32_MapDeviceType deviceType, int ioPinNumber);

#endif // ESP32_DEVICEMAPPING_H
