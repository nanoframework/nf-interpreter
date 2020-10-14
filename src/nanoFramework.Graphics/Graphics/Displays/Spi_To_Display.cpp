//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SPI_TO_DISPLAY_
#define _SPI_TO_DISPLAY_ 1

#include "DisplayInterface.h"

#include <nanoPAL.h>
#include <target_platform.h>

#define NUMBER_OF_LINES 16
#define SPI_MAX_TRANSFER_SIZE  (320 * 2 * NUMBER_OF_LINES)  // 320 pixels 2 words wide ( 16 bit colour)

struct DisplayInterface g_DisplayInterface;

// Saved gpio pins
CLR_INT16 LcdReset;        
CLR_INT16 LcdDC;    
CLR_INT16 LcdBacklight;

CLR_UINT32 SpiDeviceHandle = 0;
CLR_INT16 OutputBufferSize;
CLR_UINT8 SpiBuffer[SPI_MAX_TRANSFER_SIZE];

// Display Interface
void DisplayInterface::Initialize(DisplayInterfaceConfig& config)
{
    SPI_DEVICE_CONFIGURATION spiConfig;
   
    spiConfig.BusMode = SpiBusMode::SpiBusMode_master;
    spiConfig.Spi_Bus = config.Spi.spiBus;                          
    spiConfig.DeviceChipSelect = config.Spi.chipSelect;
    spiConfig.ChipSelectActive = false; 
    spiConfig.Spi_Mode =  SpiMode::SpiMode_Mode0;
    spiConfig.DataOrder16 = DataBitOrder::DataBitOrder_MSB;
    
    spiConfig.Clock_RateHz = 40 * 1000 * 1000;      // Spi clock speed.

    HRESULT hr = nanoSPI_OpenDevice(spiConfig, SpiDeviceHandle);
    ASSERT(hr == ESP_OK);
    if ( hr == S_OK )
    {
        // TODO Reserve Pins

        // Save pin numbers
        LcdReset = config.Spi.reset;        
        LcdDC = config.Spi.dataCommand;    
        LcdBacklight = config.Spi.backLight;

        // Initialize non-SPI GPIOs
        CPU_GPIO_SetDriveMode(LcdDC, GpioPinDriveMode::GpioPinDriveMode_Output);
        CPU_GPIO_SetDriveMode(LcdReset, GpioPinDriveMode::GpioPinDriveMode_Output);
        CPU_GPIO_SetDriveMode(LcdBacklight, GpioPinDriveMode::GpioPinDriveMode_Output);

        // Reset the display
        CPU_GPIO_SetPinState(LcdReset, GpioPinValue_Low);
        OS_DELAY(100);
        CPU_GPIO_SetPinState(LcdReset, GpioPinValue_High);
        OS_DELAY(100);
    }

    return;
}

void DisplayInterface::GetTransferBuffer(CLR_UINT8*& TransferBuffer, CLR_UINT32& TransferBufferSize)
{
    TransferBuffer = SpiBuffer;
    TransferBufferSize = sizeof(SpiBuffer);
}
void DisplayInterface::ClearFrameBuffer()
{
   // Not Used
}

void DisplayInterface::WriteToFrameBuffer(CLR_UINT8 command, CLR_UINT8 data[], CLR_UINT32 dataCount)
{
    CPU_GPIO_SetPinState(LcdDC, GpioPinValue_Low); //Command mode

    SendBytes(&command, 1);

    CPU_GPIO_SetPinState(LcdDC, GpioPinValue_High); // Data mode
    
    SendBytes(data, dataCount);
    return;
}

void DisplayInterface::SendCommand(CLR_UINT8 arg_count, ...)
{
    va_list ap;
    va_start(ap, arg_count);

    // First byte is the command
    CLR_UINT8 command = va_arg(ap, int);
    CPU_GPIO_SetPinState(LcdDC, GpioPinValue_Low); // Command mode
    SendBytes(&command, 1);

    // Following bytes are data related to the command
    int parameterCount = arg_count - 1;
    CLR_UINT8 parameters[parameterCount];
    for (int i = 0; i < parameterCount; i++)
    {
        parameters[i] = va_arg(ap, int);
    }
    CPU_GPIO_SetPinState(LcdDC, GpioPinValue_High); // Data mode

    SendBytes(parameters, parameterCount);

    return;
}

void DisplayInterface::DisplayBacklight(bool on) // true = on
{
    if (on)
    {
        CPU_GPIO_SetPinState(LcdBacklight, GpioPinValue_High);
    }
    else
    {
        CPU_GPIO_SetPinState(LcdBacklight, GpioPinValue_Low);
    }
    return;
}


void DisplayInterface::SendBytes(CLR_UINT8 *data, CLR_UINT32 length)
{
    if (length == 0) return;            //no need to send anything

    SPI_WRITE_READ_SETTINGS wrc;

    wrc.Bits16ReadWrite = false;
    wrc.callback = 0;  // No callback, synchronous
    wrc.fullDuplex = false;
    wrc.readOffset = 0;

    // Theoretically, we could do work this asynchronously and continue to work, but this would require
    // double buffering.

    // Start a synchronous read / write spi job
    nanoSPI_Write_Read( SpiDeviceHandle, wrc, data, length, NULL, 0);
    return;
}

#endif  // _SPI_TO_DISPLAY_

