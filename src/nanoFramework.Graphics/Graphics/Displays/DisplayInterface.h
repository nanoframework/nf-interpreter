//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _DISPLAY_INTERFACE_H_
#define _DISPLAY_INTERFACE_H_ 1

#include "nanoCLR_Types.h"

// DIsplay configuration
union DisplayInterfaceConfig
{
    struct 
    {
        CLR_INT8    spiBus;
        GPIO_PIN    chipSelect;
        GPIO_PIN    dataCommand;
        GPIO_PIN    reset;
        GPIO_PIN    backLight;
    } Spi;
    struct 
    {
        CLR_INT8    i2cBus;
        CLR_INT8    address;
    } I2c;
};


struct DisplayInterface
{
    void Initialize(DisplayInterfaceConfig& config);
    void GetTransferBuffer(CLR_UINT8*& BufferAddress, CLR_UINT32& sizeInBytes);
    void ClearFrameBuffer();
    void WriteToFrameBuffer(CLR_UINT8 command, CLR_UINT8 data[], CLR_UINT32 dataCount);
    void DisplayBacklight(bool on); // true = on
    void SendCommand(CLR_UINT8 arg_count, ...);
    void SendBytes(CLR_UINT8 *data, CLR_UINT32 length);
};


#endif  // _DISPLAY_INTERFACE_H_
