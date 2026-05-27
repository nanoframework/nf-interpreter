//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef DISPLAY_INTERFACE_H
#define DISPLAY_INTERFACE_H

#include "nanoCLR_Types.h"
#include "nanoCLR_Interop.h"
#include "Core.h"

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
        struct
        {
            CLR_INT16 enable;
            CLR_INT16 control;
            CLR_INT16 backlight;
            CLR_INT16 Horizontal_synchronization;
            CLR_INT16 Horizontal_back_porch;
            CLR_INT16 Horizontal_front_porch;
            CLR_INT16 Vertical_synchronization;
            CLR_INT16 Vertical_back_porch;
            CLR_INT16 Vertical_front_porch;
            CLR_INT16 Frequency_Divider;
        } VideoDisplay;
    };
    struct
    {
        CLR_UINT16 x;
        CLR_UINT16 y;
        CLR_UINT16 width;
        CLR_UINT16 height;
    } Screen;
    struct
    {
        CLR_UINT32 Width;
        CLR_UINT32 Height;
        CLR_UINT8 BitsPerPixel;
        CLR_RT_HeapBlock_Array *InitializationSequence;
        CLR_UINT8 MemoryWrite;
        CLR_UINT8 SetColumnAddress;
        CLR_UINT8 SetRowAddress;
        CLR_RT_HeapBlock_Array *PowerModeNormal;
        CLR_RT_HeapBlock_Array *PowerModeSleep;
        CLR_RT_HeapBlock_Array *OrientationPortrait;
        CLR_RT_HeapBlock_Array *OrientationPortrait180;
        CLR_RT_HeapBlock_Array *OrientationLandscape;
        CLR_RT_HeapBlock_Array *OrientationLandscape180;
        CLR_RT_HeapBlock_Array *Clear;
        CLR_UINT8 Brightness;
        CLR_UINT8 DefaultOrientation;
        CLR_UINT8 SetWindowType;
    } GenericDriverCommands;
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
    void SendData16Windowed(
        CLR_UINT16 *data,
        CLR_UINT32 startX,
        CLR_UINT32 startY,
        CLR_UINT32 width,
        CLR_UINT32 height,
        CLR_UINT32 stride,
        bool doByteSwap);
    void FillData16(CLR_UINT16 fillValue, CLR_UINT32 fillLength);
    void SetCommandMode(int mode);
};

#endif // DISPLAY_INTERFACE_H
