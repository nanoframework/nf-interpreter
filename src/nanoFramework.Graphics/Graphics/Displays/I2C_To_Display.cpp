//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _I2C_TO_DISPLAY_
#define _I2C_TO_DISPLAY_ 1

#include "DisplayInterface.h"

#include <nanoPAL.h>
#include <target_platform.h>


#define NUMBER_OF_LINES 8
#define I2C_MAX_TRANSFER_SIZE  (16 * 2 * NUMBER_OF_LINES) 

struct DisplayInterface g_DisplayInterface;


CLR_UINT32 i2cDeviceHandle = 0;

CLR_UINT8 I2cBuffer[I2C_MAX_TRANSFER_SIZE];

// Display Interface
void DisplayInterface::Initialize(DisplayInterfaceConfig& config)
{
    (void)config;

    I2C_DEVICE_CONFIGURATION i2cConfig;
   
    i2cConfig.i2cBus = config.I2c.i2cBus;
    i2cConfig.address = config.I2c.address;
    i2cConfig.fastMode = true;      

    HRESULT hr = nanoI2C_OpenDevice(i2cConfig, i2cDeviceHandle);
    ASSERT(hr == S_OK);
    if ( hr == S_OK )
    {
         // TODO Reserve other display Pins

         // Reset the display
     }
}

// Not used for I2C
void DisplayInterface::SetCommandMode(int mode)
{
    (void)mode;
}

void DisplayInterface::GetTransferBuffer(CLR_UINT8*& TransferBuffer, CLR_UINT32& TransferBufferSize)
{
    TransferBuffer = I2cBuffer;
    TransferBufferSize = sizeof(I2cBuffer);
}
void DisplayInterface::ClearFrameBuffer()
{
   // Not Used
}

void DisplayInterface::WriteToFrameBuffer(CLR_UINT8 command, CLR_UINT8 data[], CLR_UINT32 dataCount, CLR_UINT32 frameOffset)
{
    (void)frameOffset;

    CLR_UINT8 * outputData = (CLR_UINT8 *)platform_malloc(dataCount + 1);
    if ( outputData )
    {
        outputData[0] = command;   
        memcpy(outputData + 1, data, dataCount);
        SendBytes(outputData, dataCount + 1);
        platform_free((void*)outputData);
    }
}

void DisplayInterface::SendCommand(CLR_UINT8 arg_count, ...)
{
    va_list ap;
    va_start(ap, arg_count);

    // Following bytes are data related to the command
    int parameterCount = arg_count;
    CLR_UINT8 parameters[parameterCount + 1];

    parameters[0] = 0; // Control register

    for (int i = 1; i <= parameterCount; i++)
    {
        parameters[i] = va_arg(ap, int);
    }
 
    SendBytes(parameters, parameterCount + 1);
}

void DisplayInterface::DisplayBacklight(bool on) // true = on
{
    (void)on;
}

void DisplayInterface::SendBytes(CLR_UINT8 *data, CLR_UINT32 length)
{
    if (length == 0) return;            //no need to send anything

    // Currently ignores errors
    nanoI2C_WriteRead(
        i2cDeviceHandle,
        data,
        (CLR_UINT16)length,
        (CLR_UINT8 *)0,
        0);
}

#endif  // _I2C_TO_DISPLAY_

