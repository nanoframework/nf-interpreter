//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "win_dev_gpio_native.h"


HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioController::get_PinCount___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        // Return value to the managed application
        stack.SetResult_I4( GPIO_PIN_COUNT ); 
    }
    NANOCLR_NOCLEANUP();
}
