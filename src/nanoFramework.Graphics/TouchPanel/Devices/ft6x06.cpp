//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _ft6x06_H_
#define _ft6x06_H_ 1

#include "TouchDevice.h"
#include "TouchInterface.h"

struct TouchDevice g_TouchDevice;
extern TouchInterface g_TouchInterface;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

bool TouchDevice::Initialize()
{
    g_TouchDevice.Initialize();
    ReadsToIgnore = 1;
    ReadsPerSample = 1;
    MaxFilterDistance = 1;	// This is actually squared value of the max distance allowed between two points.

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
    TouchPointDevice t;
    return t;
}

#pragma GCC diagnostic pop

#endif // _ft6x06_H_
