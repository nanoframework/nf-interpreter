//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _TOUCHDEVICE_H_
#define _TOUCHDEVICE_H_ 1

#include "nanoCLR_Types.h"
#include "nanoPAL_AsyncProcCalls_decl.h"
#include "TouchPanel.h"

struct TouchPointDevice
{
    int x;
    int y;
};

struct TouchDevice
{

    CLR_INT32 ReadsToIgnore;
    CLR_INT32 ReadsPerSample;
    CLR_INT32 MaxFilterDistance;      // This is actually squared value of the max distance allowed between two points.

    bool Initialize();
    TouchPointDevice GetPoint();
    bool Enable(GPIO_INTERRUPT_SERVICE_ROUTINE touchIsrProc);
    bool Disable();

};

#endif //_TOUCHDEVICE_H_

