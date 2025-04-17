//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SPI_TO_DISPLAY_
#define SPI_TO_DISPLAY_

#include "DisplayInterface.h"

#include <nanoPAL.h>
#include <target_platform.h>

#define NUMBER_OF_LINES       8
#define SPI_MAX_TRANSFER_SIZE (320 * 2 * NUMBER_OF_LINES) // 320 pixels 2 words wide ( 16 bit colour)
#define SPI_MAX_TRANSFER_16   (SPI_MAX_TRANSFER_SIZE / 2)

struct DisplayInterface g_DisplayInterface;
DisplayInterfaceConfig g_DisplayInterfaceConfig;

// Saved gpio pins
CLR_INT16 lcdReset;
CLR_INT16 lcdDC;
CLR_INT16 lcdBacklight;

uint32_t spiDeviceHandle = 0;
int spiChipSelect = 0;
bool spiChipSelectActiveState = false;
CLR_INT16 outputBufferSize;
CLR_UINT8 spiBuffer[SPI_MAX_TRANSFER_SIZE];
CLR_UINT8 spiBuffer2[SPI_MAX_TRANSFER_SIZE];
CLR_UINT8 spiCommandMode = 0; // 0 Command first byte, 1 = Command all bytes

// State variables for writes
CLR_UINT16 *currentBuffer;
CLR_UINT16 *bufferPtr;
CLR_UINT32 bufferWritten;

void SwapBuffers();
void FlushData();
void CopyData16ByteSwapped(CLR_UINT16 *dest, CLR_UINT16 *src, CLR_UINT32 length);
void SendData16(CLR_UINT16 *data, CLR_UINT32 length, bool doByteSwap);
void InternalSendBytes(CLR_UINT8 *data, CLR_UINT32 length, bool sendAsync);

// Display Interface
void DisplayInterface::Initialize(DisplayInterfaceConfig &config)
{
    SPI_DEVICE_CONFIGURATION spiConfig;
    g_DisplayInterfaceConfig = config;
    spiConfig.BusMode = SpiBusMode::SpiBusMode_master;
    spiConfig.Spi_Bus = config.Spi.spiBus;
    spiConfig.DeviceChipSelect = config.Spi.chipSelect;
    spiConfig.ChipSelectActiveState = false;
    spiConfig.Spi_Mode = SpiMode::SpiMode_Mode0;
    spiConfig.DataOrder16 = DataBitOrder::DataBitOrder_MSB;
    spiConfig.BusConfiguration = SpiBusConfiguration_FullDuplex;
    // Store for internal usage
    spiChipSelect = config.Spi.chipSelect;
    spiChipSelectActiveState = false;

    spiConfig.Clock_RateHz = 40 * 1000 * 1000; // SPI clock speed.

    currentBuffer = (CLR_UINT16 *)spiBuffer;
    bufferPtr = currentBuffer;
    bufferWritten = 0;

    HRESULT hr = nanoSPI_OpenDevice(spiConfig, spiDeviceHandle);
    if (hr == S_OK)
    {
        // TODO Reserve Pins

        // Save pin numbers
        lcdReset = config.Spi.reset;
        lcdDC = config.Spi.dataCommand;
        lcdBacklight = config.Spi.backLight;

        // Initialize non-SPI GPIOs
        CPU_GPIO_SetDriveMode(lcdDC, PinMode::PinMode_Output);

        if (lcdBacklight >= 0)
        {
            CPU_GPIO_SetDriveMode(lcdBacklight, PinMode::PinMode_Output);
        }

        if (lcdReset >= 0)
        {
            CPU_GPIO_SetDriveMode(lcdReset, PinMode::PinMode_Output);
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

    SendCommand(1, command);
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

    // Make sure any outstanding async writes have finished before we set the pin low
    nanoSPI_Wait_Busy(spiDeviceHandle);

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

// Dummy callback to enable async spi writes
void spi_callback(int busIndex)
{
    (void)busIndex;
}

void DisplayInterface::SendBytes(CLR_UINT8 *data, CLR_UINT32 length)
{
    InternalSendBytes(data, length, false);
}

void InternalSendBytes(CLR_UINT8 *data, CLR_UINT32 length, bool sendAsync)
{
    if (length == 0)
        return; // no need to send anything

    SPI_WRITE_READ_SETTINGS wrc;

    wrc.Bits16ReadWrite = false;
    wrc.callback = sendAsync ? spi_callback : 0;
    wrc.fullDuplex = false;
    wrc.readOffset = 0;
    wrc.DeviceChipSelect = spiChipSelect;
    wrc.ChipSelectActiveState = spiChipSelectActiveState;

    nanoSPI_Write_Read(spiDeviceHandle, wrc, data, length, nullptr, 0);

    return;
}

// Swap between our two data transfer buffers
void SwapBuffers()
{
    currentBuffer = (currentBuffer == (CLR_UINT16 *)spiBuffer) ? (CLR_UINT16 *)spiBuffer2 : (CLR_UINT16 *)spiBuffer;
    bufferPtr = currentBuffer;
    bufferWritten = 0;
}

// Flush any remaining buffered data, and swap buffers
void FlushData()
{
    if (bufferWritten > 0)
    {
        InternalSendBytes((CLR_UINT8 *)currentBuffer, bufferWritten * 2, true);

        SwapBuffers();
    }
}

// Copy 16bit data src to dest, swapping the high and low bytes
void CopyData16ByteSwapped(CLR_UINT16 *dest, CLR_UINT16 *src, CLR_UINT32 length)
{
    while (length > 0)
    {
        *dest = (*src << 8) | (*src >> 8);

        dest++;
        src++;

        length--;
    }
}

void SendData16(CLR_UINT16 *data, CLR_UINT32 length, bool doByteSwap)
{
    while (length > 0)
    {
        CLR_UINT32 toWrite = length;

        if (toWrite > (SPI_MAX_TRANSFER_16 - bufferWritten))
        {
            toWrite = (SPI_MAX_TRANSFER_16 - bufferWritten);
        }

        // If we need to byteswap the data, do that - otherwise we can just memcpy it
        if (doByteSwap)
        {
            CopyData16ByteSwapped(bufferPtr, data, toWrite);
        }
        else
        {
            memcpy(bufferPtr, data, toWrite * 2);
        }

        bufferPtr += toWrite;
        data += toWrite;
        bufferWritten += toWrite;
        length -= toWrite;

        if (bufferWritten == SPI_MAX_TRANSFER_16)
        {
            FlushData();
        }
    }
}

void DisplayInterface::SendData16Windowed(
    CLR_UINT16 *data,
    CLR_UINT32 startX,
    CLR_UINT32 startY,
    CLR_UINT32 width,
    CLR_UINT32 height,
    CLR_UINT32 stride,
    bool doByteSwap)
{
    // Offset for window start
    CLR_UINT16 *startOfLine = data + (startY * stride) + startX;

    if (width == stride) // Optimize full-stride writes
    {
        SendData16(startOfLine, width * height, doByteSwap);
    }
    else
    {
        while (height--)
        {
            SendData16(startOfLine, width, doByteSwap);

            startOfLine += stride;
        }
    }

    FlushData();

    return;
}

void DisplayInterface::FillData16(CLR_UINT16 fillValue, CLR_UINT32 fillLength)
{
    // Fill the current buffer full of fillValue, or as much as we need
    memset(currentBuffer, fillValue, ((fillLength < SPI_MAX_TRANSFER_16) ? fillLength : SPI_MAX_TRANSFER_16) * 2);

    // If our length doesn't evenly divide our buffer size, send out the small buffer first
    // This gives us the maximum freed up time at the end
    CLR_UINT32 firstSize = fillLength % SPI_MAX_TRANSFER_16;

    InternalSendBytes((CLR_UINT8 *)currentBuffer, firstSize * 2, true);

    fillLength -= firstSize;

    while (fillLength > 0)
    {
        InternalSendBytes((CLR_UINT8 *)currentBuffer, SPI_MAX_TRANSFER_SIZE, true);

        fillLength -= SPI_MAX_TRANSFER_16;
    }

    // Swap buffers at the end so the next call doesn't overwrite our data
    SwapBuffers();
}

#endif // SPI_TO_DISPLAY_
