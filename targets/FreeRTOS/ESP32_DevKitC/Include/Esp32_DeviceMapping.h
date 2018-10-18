//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

//
//  Header file for ESP32 device pin mnapping
//

#ifndef _ESP32_DEVICEMAPPING_
#define _ESP32_DEVICEMAPPING_

// Default I2C gpio pins
#define  I2C1_DATA      18
#define  I2C1_CLOCK     19
#define  I2C2_DATA      25
#define  I2C2_CLOCK     26

enum Esp32_MapDeviceType
{
    DEV_TYPE_GPIO,
    DEV_TYPE_SPI,
    DEV_TYPE_I2C,
    DEV_TYPE_LED_PWM,
    DEV_TYPE_SERIAL,
    DEV_TYPE_MAX,
};

int  Esp32_GetMappedDevicePins( Esp32_MapDeviceType DevType, int DevNumber, int PinIndex);

void Esp32_SetMappedDevicePins( uint8_t pin, int32_t alternateFunction );

#endif //_ESP32_DEVICEMAPPING_
