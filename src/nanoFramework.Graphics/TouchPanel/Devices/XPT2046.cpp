//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _XPT2046_H_
#define _XPT2046_H_ 1

#include "TouchDevice.h"
#include "TouchInterface.h"

struct TouchDevice g_TouchDevice;
extern TouchInterface g_TouchInterface;

bool TouchDevice::Initialize()
{
    g_TouchDevice.Initialize();
    ReadsToIgnore = 1;
    ReadsPerSample = 1;
    MaxFilterDistance = 1;   // This is actually squared value of the max distance allowed between two points.
    return true;
}

bool TouchDevice::Enable(GPIO_INTERRUPT_SERVICE_ROUTINE touchIsrProc)
{
    if (touchIsrProc == NULL) {};
    return TRUE;
}

bool TouchDevice::Disable()
{
    return true;
}

TouchPointDevice TouchDevice::GetPoint()
{

    // stub
    TouchPointDevice TouchValue;

    TouchValue.x = 0;
    TouchValue.y = 0;

    return TouchValue;
}
#endif // _XPT2046_H_
