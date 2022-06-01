//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef DISPLAY_INTERFACE_H
#define DISPLAY_INTERFACE_H

#include "nanoCLR_Types.h"

// Display configuration
struct DisplayInterfaceConfig
{
    union {
        struct
        {
            CLR_UINT8 spiBus;
            CLR_INT32 chipSelect;
            CLR_INT32 dataCommand;
            CLR_INT32 reset;
            CLR_INT32 backLight;
        } Spi;
        struct
        {
            CLR_INT8 i2cBus;
            CLR_INT8 address;
            CLR_INT8 fastMode;
        } I2c;
    };
    struct
    {
        CLR_UINT16 x;
        CLR_UINT16 y;
        CLR_UINT16 width;
        CLR_UINT16 height;
    } Screen;
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
