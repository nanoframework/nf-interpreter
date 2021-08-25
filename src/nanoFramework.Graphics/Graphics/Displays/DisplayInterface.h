//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef DISPLAY_INTERFACE_H
#define DISPLAY_INTERFACE_H

#include "nanoCLR_Types.h"

// Display configuration
union DisplayInterfaceConfig {
    struct
    {
        CLR_INT8 spiBus;
        GPIO_PIN chipSelect;
        GPIO_PIN dataCommand;
        GPIO_PIN reset;
        GPIO_PIN backLight;
    } Spi;
    struct
    {
        CLR_INT8 i2cBus;
        CLR_INT8 address;
        CLR_INT8 fastMode;
    } I2c;
};

struct DisplayInterface
{
    void Initialize(DisplayInterfaceConfig &config);
    void GetTransferBuffer(CLR_UINT8 *&BufferAddress, CLR_UINT32 &sizeInBytes);
    void ClearFrameBuffer();
    void WriteToFrameBuffer(CLR_UINT8 command, CLR_UINT8 data[], CLR_UINT32 dataCount, CLR_UINT32 frameOffset = 0);
    void DisplayBacklight(bool on); // true = on
    void SendCommand(CLR_UINT8 arg_count, ...);
    void SendBytes(CLR_UINT8 *data, CLR_UINT32 length);
    void SetCommandMode(int mode);
};

#endif // DISPLAY_INTERFACE_H
