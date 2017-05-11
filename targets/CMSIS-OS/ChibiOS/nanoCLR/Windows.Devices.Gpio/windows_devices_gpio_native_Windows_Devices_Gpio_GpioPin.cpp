//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include "windows_devices_gpio_native.h"
#include "windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin.h"

using namespace Windows::Devices::Gpio;

signed int GpioPin::get_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
    signed int retVal = 0; 
    return retVal;
}

void GpioPin::set_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr )
{
}

void GpioPin::DisposeNative( CLR_RT_HeapBlock* pMngObj, HRESULT &hr )
{
}

