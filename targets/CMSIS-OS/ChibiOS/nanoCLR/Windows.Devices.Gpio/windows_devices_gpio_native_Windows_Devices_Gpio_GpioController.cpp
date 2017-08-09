//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "windows_devices_gpio_native.h"
#include "windows_devices_gpio_native_Windows_Devices_Gpio_GpioController.h"

using namespace Windows::Devices::Gpio;

bool GpioController::NativeOpenpin( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr )
{
    bool retVal = 1; 
    return retVal;
}

signed int GpioController::get_PinCount( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    signed int retVal = 0; 
    return 144;	// Fixme: temporary arbitrary value
}

