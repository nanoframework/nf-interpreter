//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "win_dev_gpio_native.h"


HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioController::get_PinCount___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // return count ot available GPIO pins
        stack.SetResult_I4( 130 ); // Fixme: temporary arbitrary value
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
