//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SPI_TO_DISPLAY_
#define SPI_TO_DISPLAY_

#include "DisplayInterface.h"

#include <nanoPAL.h>
#include <target_platform.h>

#define NUMBER_OF_LINES       16
#define SPI_MAX_TRANSFER_SIZE (320 * 2 * NUMBER_OF_LINES) // 320 pixels 2 words wide ( 16 bit colour)

struct DisplayInterface g_DisplayInterface;
DisplayInterfaceConfig g_DisplayInterfaceConfig;

// Saved gpio pins
CLR_INT16 lcdReset;
CLR_INT16 lcdDC;
CLR_INT16 lcdBacklight;

CLR_UINT32 spiDeviceHandle = 0;
CLR_INT16 outputBufferSize;
CLR_UINT8 spiBuffer[SPI_MAX_TRANSFER_SIZE];
CLR_UINT8 spiCommandMode = 0; // 0 Command first byte, 1 = Command all bytes

// Display Interface
void DisplayInterface::Initialize(DisplayInterfaceConfig &config)
{
    SPI_DEVICE_CONFIGURATION spiConfig;
    g_DisplayInterfaceConfig = config;
    spiConfig.BusMode = SpiBusMode::SpiBusMode_master;
    spiConfig.Spi_Bus = config.Spi.spiBus;
    spiConfig.DeviceChipSelect = config.Spi.chipSelect;
    spiConfig.ChipSelectActive = false;
    spiConfig.Spi_Mode = SpiMode::SpiMode_Mode0;
    spiConfig.DataOrder16 = DataBitOrder::DataBitOrder_MSB;

    spiConfig.Clock_RateHz = 40 * 1000 * 1000; // Spi clock speed.

    HRESULT hr = nanoSPI_OpenDevice(spiConfig, spiDeviceHandle);
    ASSERT(hr == ESP_OK);
    if (hr == S_OK)
    {
        // TODO Reserve Pins

        // Save pin numbers
        lcdReset = config.Spi.reset;
        lcdDC = config.Spi.dataCommand;
        lcdBacklight = config.Spi.backLight;

        // Initialize non-SPI GPIOs
        CPU_GPIO_SetDriveMode(lcdDC, GpioPinDriveMode::GpioPinDriveMode_Output);

        if (lcdBacklight >= 0)
        {
            CPU_GPIO_SetDriveMode(lcdBacklight, GpioPinDriveMode::GpioPinDriveMode_Output);
        }

        if (lcdReset >= 0)
        {
            CPU_GPIO_SetDriveMode(lcdReset, GpioPinDriveMode::GpioPinDriveMode_Output);
            // Reset the display
            CPU_GPIO_SetPinState(lcdReset, GpioPinValue_Low);
            OS_DELAY(100);
            CPU_GPIO_SetPinState(lcdReset, GpioPinValue_High);
            OS_DELAY(100);
        }
    }

    return;
}

void DisplayInterface::SetCommandMode(int mode)
{
    spiCommandMode = mode;
}

void DisplayInterface::GetTransferBuffer(CLR_UINT8 *&TransferBuffer, CLR_UINT32 &TransferBufferSize)
{
    TransferBuffer = spiBuffer;
    TransferBufferSize = sizeof(spiBuffer);
}
void DisplayInterface::ClearFrameBuffer()
{
    // Not Used
}

void DisplayInterface::WriteToFrameBuffer(
    CLR_UINT8 command,
    CLR_UINT8 data[],
    CLR_UINT32 dataCount,
    CLR_UINT32 frameOffset)
{
    (void)frameOffset;

    CPU_GPIO_SetPinState(lcdDC, GpioPinValue_Low); // Command mode

    SendBytes(&command, 1);

    CPU_GPIO_SetPinState(lcdDC, GpioPinValue_High); // Data mode

    SendBytes(data, dataCount);
    return;
}

void DisplayInterface::SendCommand(CLR_UINT8 arg_count, ...)
{
    va_list ap;
    va_start(ap, arg_count);

    // Parse arguments into parameters buffer
    CLR_UINT8 parameters[arg_count];
    for (int i = 0; i < arg_count; i++)
    {
        parameters[i] = va_arg(ap, int);
    }

    CPU_GPIO_SetPinState(lcdDC, GpioPinValue_Low); // Command mode
    if (spiCommandMode == 0)
    {
        // Send only first byte (command) with D/C signal low
        SendBytes(&parameters[0], 1);

        CPU_GPIO_SetPinState(lcdDC, GpioPinValue_High); // Data mode

        // Send remaining parameters ( if any )
        if (arg_count > 1)
        {
            SendBytes(&parameters[1], arg_count - 1);
        }
    }
    else
    {
        // Send all Command bytes with D/C signal low
        SendBytes(parameters, arg_count);
        CPU_GPIO_SetPinState(lcdDC, GpioPinValue_High); // Data mode
    }
}

void DisplayInterface::DisplayBacklight(bool on) // true = on
{
    if (lcdBacklight >= 0)
    {
        if (on)
        {
            CPU_GPIO_SetPinState(lcdBacklight, GpioPinValue_High);
        }
        else
        {
            CPU_GPIO_SetPinState(lcdBacklight, GpioPinValue_Low);
        }
    }

    return;
}

void DisplayInterface::SendBytes(CLR_UINT8 *data, CLR_UINT32 length)
{
    if (length == 0)
        return; // no need to send anything

    SPI_WRITE_READ_SETTINGS wrc;

    wrc.Bits16ReadWrite = false;
    wrc.callback = 0; // No callback, synchronous
    wrc.fullDuplex = false;
    wrc.readOffset = 0;

    // Theoretically, we could do work this asynchronously and continue to work, but this would require
    // double buffering.

    // Start a synchronous read / write spi job
    nanoSPI_Write_Read(spiDeviceHandle, wrc, data, length, NULL, 0);
    return;
}

#endif // SPI_TO_DISPLAY_
