//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _DISPLAY_INTERFACE_H_
#define _DISPLAY_INTERFACE_H_ 1

#include "nanoCLR_Types.h"

struct DisplayInterface
{
    void Initialize();
    void GetTransferBuffer(CLR_UINT8*& BufferAddress, CLR_UINT32& sizeInBytes);
    void ClearFrameBuffer();
    void WriteToFrameBuffer(CLR_UINT8 command, CLR_UINT8 data[], CLR_UINT32 dataCount);
    void DisplayBacklight(bool on); // true = on
    void SendCommand(CLR_UINT8 arg_count, ...);
};


#endif  // _DISPLAY_INTERFACE_H_
