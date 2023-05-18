//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <esp32_idf.h>
#include <Esp32_DeviceMapping.h>

void Esp32_DecodeAlternateFunction(
    uint32_t alternateFunction,
    Esp32_MapDeviceType &deviceType,
    uint8_t &busIndex,
    uint16_t &pinIndex)
{
    deviceType = (Esp32_MapDeviceType)((alternateFunction >> 16) & 0x00ff);
    busIndex = (uint8_t)((alternateFunction >> 8) & 0x00ff) - 1;
    pinIndex = (uint16_t)(alternateFunction & 0x00ff);
}

int Esp32_GetMappedDevicePinsWithFunction(uint32_t alternateFunction)
{
    Esp32_MapDeviceType deviceType;
    uint8_t busIndex;
    uint16_t pinIndex;

    Esp32_DecodeAlternateFunction(alternateFunction, deviceType, busIndex, pinIndex);

    return Esp32_GetMappedDevicePins(deviceType, busIndex, pinIndex);
}

int Esp32_GetMappedDevicePins(Esp32_MapDeviceType deviceType, int busIndex, int pinIndex)
{
    if (busIndex >= 0)
    {
        switch (deviceType)
        {
            case DEV_TYPE_SPI:
                return (int)Esp32_SPI_DevicePinMap[busIndex][pinIndex];

            case DEV_TYPE_I2C:
                return (int)Esp32_I2C_DevicePinMap[busIndex][pinIndex];

            case DEV_TYPE_SERIAL:
                return (int)Esp32_SERIAL_DevicePinMap[busIndex][pinIndex];

            case DEV_TYPE_LED_PWM:
                return (int)Esp32_LED_DevicePinMap[busIndex];

            case DEV_TYPE_ADC:
                return (int)Esp32_ADC_DevicePinMap[pinIndex];

#if !(defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3))
            case DEV_TYPE_DAC:
                return (int)Esp32_DAC_DevicePinMap[pinIndex];
#endif

            case DEV_TYPE_I2S:
                return (int)Esp32_I2S_DevicePinMap[busIndex][pinIndex];

            default:
                break;
        };
    }
    return -1;
}

void Esp32_SetMappedDevicePins(Esp32_MapDeviceType deviceType, int busIndex, int8_t pinIndex, int ioPinNumber)
{
    if (busIndex >= 0)
    {
        switch (deviceType)
        {
            case DEV_TYPE_SPI:
                Esp32_SPI_DevicePinMap[busIndex][pinIndex] = ioPinNumber;
                break;

            case DEV_TYPE_I2C:
                Esp32_I2C_DevicePinMap[busIndex][pinIndex] = ioPinNumber;
                break;

            case DEV_TYPE_SERIAL:
                Esp32_SERIAL_DevicePinMap[busIndex][pinIndex] = ioPinNumber;
                break;

            case DEV_TYPE_LED_PWM:
                Esp32_LED_DevicePinMap[busIndex] = ioPinNumber;
                break;

            case DEV_TYPE_ADC:
                Esp32_ADC_DevicePinMap[pinIndex] = ioPinNumber;
                break;

#if !(defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3))
            case DEV_TYPE_DAC:
                Esp32_DAC_DevicePinMap[pinIndex] = ioPinNumber;
                break;
#endif

            case DEV_TYPE_I2S:
                Esp32_I2S_DevicePinMap[busIndex][pinIndex] = ioPinNumber;
                break;

            default:
                break;
        };
    }
}

// Esp32_SetMappedDevicePins
//
// alternateFunction  32 bit integer where 00DDIIFF
//    DD= device type
//    II= bus index 1,2,3 etc
//    FF= functional pin i.e  data, clock, tx,rx
//
int Esp32_SetMappedDevicePins(uint8_t pin, int32_t alternateFunction)
{
    Esp32_MapDeviceType deviceType;
    uint8_t busIndex;
    uint16_t gpioMapping;

    Esp32_DecodeAlternateFunction(alternateFunction, deviceType, busIndex, gpioMapping);

    // Set the pins used by a device type / index
    switch (deviceType)
    {
        case DEV_TYPE_GPIO:
            // Just opening a GPIO pin will set it to GPIO function
            return true;

        case DEV_TYPE_SPI:
            if (busIndex <= MAX_SPI_DEVICES && gpioMapping < Esp32SpiPin_Max)
            {
                // validate if this is a valid output pin (only required for CLK and MOSI)
                if (gpioMapping != Esp32SpiPin_Miso)
                {
                    if (!GPIO_IS_VALID_OUTPUT_GPIO(pin))
                    {
                        return false;
                    }
                }

                Esp32_SPI_DevicePinMap[busIndex][gpioMapping] = pin;
                return true;
            }
            break;

        case DEV_TYPE_I2C:
            if (busIndex <= I2C_NUM_MAX && gpioMapping <= 1)
            {
                // validate if this is a valid output pin (required for both SDA and SCL)
                if (GPIO_IS_VALID_OUTPUT_GPIO(pin))
                {
                    Esp32_I2C_DevicePinMap[busIndex][gpioMapping] = pin;
                    return true;
                }
            }
            break;

        case DEV_TYPE_SERIAL:
            if (busIndex < UART_NUM_MAX && gpioMapping < Esp32SerialPin_Max)
            {
                // validate if this is a valid output pin (required for TX and RTS)
                if (gpioMapping == Esp32SerialPin_Tx || gpioMapping == Esp32SerialPin_Rts)
                {
                    if (!GPIO_IS_VALID_OUTPUT_GPIO(pin))
                    {
                        return false;
                    }
                }
                Esp32_SERIAL_DevicePinMap[busIndex][gpioMapping] = pin;
                return true;
            }
            break;

        case DEV_TYPE_LED_PWM:
            if (busIndex <= 15)
            {
                Esp32_LED_DevicePinMap[busIndex] = pin;
                return true;
            }
            break;

        case DEV_TYPE_I2S:
            if (busIndex <= 1 && gpioMapping <= 4)
            {
                Esp32_I2S_DevicePinMap[busIndex][gpioMapping] = pin;
                return true;
            }
            break;

        default:
            // invalid device type
            return false;
    }

    return false;
}

int Esp32_ValidateMappedDevicePin(Esp32_MapDeviceType deviceType, int ioPinNumber)
{
    return false;
}
