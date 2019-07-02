//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "Esp32_os.h"
#include "Esp32_DeviceMapping.h"


//  SPI 
//  2 devices
//  Map pins  mosi, miso, clock
// 
int8_t Esp32_SPI_DevicePinMap[2][3] = 
{
    {GPIO_NUM_23, GPIO_NUM_25, GPIO_NUM_19},    // SPI1 - Wrover SPI display pins
    {-1, -1, -1}                                // SPI2 - no pins assigned
};

//  Serial 
//  3 devices COM1,COM2, COM3 ( UART_NUM_0, UART_NUM_1, UART_NUM_2 )
//  Map pins  Tx, RX, RTS, CTS
//  Set default direct pins
int8_t Esp32_SERIAL_DevicePinMap[3][4] = 
{
    // COM 1 - pins 1, 3, 19, 22
    {UART_NUM_0_TXD_DIRECT_GPIO_NUM, UART_NUM_0_RXD_DIRECT_GPIO_NUM, UART_NUM_0_RTS_DIRECT_GPIO_NUM, UART_NUM_0_CTS_DIRECT_GPIO_NUM},
    
    // COM 2 - 10, 9, 6, 11
    {UART_NUM_1_TXD_DIRECT_GPIO_NUM, UART_NUM_1_RXD_DIRECT_GPIO_NUM, UART_NUM_1_RTS_DIRECT_GPIO_NUM, UART_NUM_1_CTS_DIRECT_GPIO_NUM},
    
    // COM3 - 17, 16, 8, 7
    {UART_NUM_2_TXD_DIRECT_GPIO_NUM, UART_NUM_2_RXD_DIRECT_GPIO_NUM, UART_NUM_2_RTS_DIRECT_GPIO_NUM, UART_NUM_2_CTS_DIRECT_GPIO_NUM}
 };

// =============================================
//  I2C  
//  2 devices I2C1 & I2C2
//  Map pins Data & Clock
int8_t Esp32_I2C_DevicePinMap[2][2] = 
{
    // I2C1 - pins 18, 19,
    {I2C1_DATA, I2C1_CLOCK},
    // I2C2 - pins 25, 26
    {I2C2_DATA, I2C2_CLOCK}
};

// =============================================
//  LED PWM  
//  16 channels LED1 to LED16  or PWM1 to PWM16
//  Map pins Data & Clock
int8_t Esp32_LED_DevicePinMap[16] = 
{
            // Channels ( non assigned )
    -1,     // 1
    -1,     // 2
    -1,     // 3
    -1,     // 4
    -1,     // 5    
    -1,     // 6
    -1,     // 7
    -1,     // 8
    -1,     // 9
    -1,     // 10
    -1,     // 11
    -1,     // 12
    -1,     // 13
    -1,     // 14
    -1,     // 15
    -1,     // 16
};

// We use "ADC1" for 20 logical channels
// Mapped to ESP32 controllers
// ESP32 ADC1 channels 0 - 7
//  "    ADC1 channel  8 - Internal Temperture sensor (VP)
//  "    ADC1 channel  9 - Internal Hall Sensor (VN)
//  "    ADC2 channels 10 - 19      
int8_t Esp32_ADC_DevicePinMap[20] =
{
	// 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19
	  36, 37, 38, 39, 32, 33, 34, 35, 36, 39, 04, 00, 02, 15, 13, 12, 14, 27, 25, 26
};

void  Esp32_DecodeAlternateFunction(uint32_t alternateFunction, Esp32_MapDeviceType & deviceType, uint8_t & busIndex, uint16_t & PinIndex)
{
	deviceType = (Esp32_MapDeviceType)((alternateFunction >> 16) & 0x00ff);
	busIndex   = (uint8_t)((alternateFunction >> 8) & 0x00ff) - 1;
	PinIndex   = (uint16_t)(alternateFunction & 0x00ff);
}

int  Esp32_GetMappedDevicePins( Esp32_MapDeviceType deviceType, int DevNumber, int PinIndex)
{
    if  ( DevNumber >= 0)
    {
        switch( deviceType )
        {
            case DEV_TYPE_SPI:
                return (int)Esp32_SPI_DevicePinMap[DevNumber][PinIndex];
        
            case DEV_TYPE_I2C:
                return (int)Esp32_I2C_DevicePinMap[DevNumber][PinIndex];
            
			case DEV_TYPE_SERIAL:
				return (int)Esp32_SERIAL_DevicePinMap[DevNumber][PinIndex];
			
			case DEV_TYPE_LED_PWM:
                return (int)Esp32_LED_DevicePinMap[DevNumber];

			case DEV_TYPE_ADC:
				return (int)Esp32_ADC_DevicePinMap[PinIndex];
			
			default:
                break;
        };
    }
    return -1;
}

int  Esp32_GetMappedDevicePinsWithFunction(uint32_t alternateFunction)
{
	Esp32_MapDeviceType deviceType;
	uint8_t deviceIndex;
	uint16_t pinIndex;

	Esp32_DecodeAlternateFunction(alternateFunction, deviceType, deviceIndex, pinIndex);

	return Esp32_GetMappedDevicePins(deviceType, deviceIndex, pinIndex);
}


// Esp32_SetMappedDevicePins
//
// alternateFunction  32 bit integer where 00DDIIFF
//    DD= device type
//    II= device index 1,2,3 etc 
//    FF= functional pin i.e  data, clock, tx,rx   
//      
void Esp32_SetMappedDevicePins( uint8_t pin, int32_t alternateFunction )
{
	Esp32_MapDeviceType deviceType;
	uint8_t deviceIndex;
	uint16_t mapping; 

	Esp32_DecodeAlternateFunction(alternateFunction, deviceType, deviceIndex, mapping);

    // Set the pins used by a device type / index
    switch( deviceType )
    {
        case DEV_TYPE_GPIO:
            // Just opening a GPIO pin will set it to GPIO function
            break;

        case DEV_TYPE_SPI:
            if (deviceIndex <= 1 &&  mapping <= 2) 
            {
                Esp32_SPI_DevicePinMap[deviceIndex][mapping] = pin;
            }
            break;

        case DEV_TYPE_I2C:
            if (deviceIndex <= 1 &&  mapping <= 1) 
            {
                Esp32_I2C_DevicePinMap[deviceIndex][mapping] = pin;
            }
            break;

		case DEV_TYPE_SERIAL:
			if (deviceIndex <= 2 && mapping <= 3)
			{
				Esp32_SERIAL_DevicePinMap[deviceIndex][mapping] = pin;
			}
			break;
		
		case DEV_TYPE_LED_PWM:
            if (deviceIndex <= 15 ) 
            {
                Esp32_LED_DevicePinMap[deviceIndex] = pin;
            }
            break;


        default:  // ignore
            break;
    }
}
