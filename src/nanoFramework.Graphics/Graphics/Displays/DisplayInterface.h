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
    bool InitializeDisplayInterface();
    void SendCommandByte(CLR_UINT8 c);
    void SendCommandWord(CLR_UINT16 c);
    void SendData(CLR_UINT8 data[], CLR_UINT32 count);
    void SendCommandByteAndData(CLR_UINT8 command, CLR_UINT8 data[], CLR_UINT32 dataCount);
    void SendCommandWordAndData(CLR_UINT16 command, CLR_UINT8 data[], CLR_UINT32 dataCount);
    void DisplayBacklight(bool on); // true = on
    void DisplayCommandDelay(CLR_INT16 delay);
    void GetTransferBuffer(CLR_UINT8*& BufferAddress, CLR_UINT32& sizeInBytes);
};


#endif  // _DISPLAY_INTERFACE_H_
