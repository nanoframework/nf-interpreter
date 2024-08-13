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

// Note - I2S_NUM_MAX renamed/changed as of IDF 5.1.3 and I2S_PIN_NO_CHANGE missing
#define I2S_NUM_MAX       I2S_NUM_AUTO
#define I2S_PIN_NO_CHANGE (-1)

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

enum Esp32SdmmcPin
{
    Esp32SdmmcPin_Clock,
    Esp32SdmmcPin_Command,
    Esp32SdmmcPin_D0,
    Esp32SdmmcPin_D1,
    Esp32SdmmcPin_D2,
    Esp32SdmmcPin_D3,
    Esp32SdmmcPin_Max,
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
    DEV_TYPE_SDMMC,
    DEV_TYPE_MAX,
};

#ifdef __cplusplus

extern int8_t Esp32_SPI_DevicePinMap[MAX_SPI_DEVICES][Esp32SpiPin_Max];
extern int8_t Esp32_I2C_DevicePinMap[I2C_NUM_MAX][2];
extern int8_t Esp32_I2S_DevicePinMap[I2S_NUM_MAX][5];
extern int8_t Esp32_SERIAL_DevicePinMap[UART_NUM_MAX][Esp32SerialPin_Max];

#if defined(CONFIG_IDF_TARGET_ESP32C3)
#define TARGET_ADC_NUM_PINS (CONFIG_SOC_ADC_MAX_CHANNEL_NUM + 1)
#define TARGET_LED_NUM_PINS 6
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
#define TARGET_ADC_NUM_PINS CONFIG_SOC_ADC_MAX_CHANNEL_NUM
#define TARGET_LED_NUM_PINS 6
#elif defined(CONFIG_IDF_TARGET_ESP32H2)
#define TARGET_ADC_NUM_PINS CONFIG_SOC_ADC_MAX_CHANNEL_NUM
#define TARGET_LED_NUM_PINS 8
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define TARGET_ADC_NUM_PINS (CONFIG_SOC_ADC_MAX_CHANNEL_NUM * CONFIG_SOC_ADC_PERIPH_NUM)
#define TARGET_LED_NUM_PINS 8
#else
#define TARGET_ADC_NUM_PINS (CONFIG_SOC_ADC_MAX_CHANNEL_NUM * CONFIG_SOC_ADC_PERIPH_NUM)
#define TARGET_LED_NUM_PINS 16
extern int8_t Esp32_DAC_DevicePinMap[2];
#endif

extern int8_t Esp32_ADC_DevicePinMap[TARGET_ADC_NUM_PINS];
extern int8_t Esp32_LED_DevicePinMap[TARGET_LED_NUM_PINS];
#if (defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32P4))
extern int8_t Esp32_SDMMC_DevicePinMap[CONFIG_SOC_SDMMC_NUM_SLOTS][Esp32SdmmcPin_Max];
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

#else
// For C code access
int Esp32_GetSDmmcDevicePins_C(int busIndex, int pinIndex);
#endif

#endif // ESP32_DEVICEMAPPING_H
