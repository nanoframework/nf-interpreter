//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

//
//  Header file for ESP32 device pin mnapping
//

#ifndef _ESP32_DEVICEMAPPING_
#define _ESP32_DEVICEMAPPING_

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

enum Esp32_MapDeviceType
{
    DEV_TYPE_GPIO,
    DEV_TYPE_SPI,
    DEV_TYPE_I2C,
    DEV_TYPE_SERIAL,
    DEV_TYPE_LED_PWM,
    DEV_TYPE_ADC,
    DEV_TYPE_DAC,
    DEV_TYPE_MAX,
};

int Esp32_GetMappedDevicePins(Esp32_MapDeviceType DevType, int DevNumber, int PinIndex);
int Esp32_GetMappedDevicePinsWithFunction(uint32_t alternateFunction);

void Esp32_SetMappedDevicePins(uint8_t pin, int32_t alternateFunction);
void Esp32_SetMappedDevicePins(Esp32_MapDeviceType devType, int devNumber, int8_t pinIndex, int ioPinNumber);

#endif //_ESP32_DEVICEMAPPING_
