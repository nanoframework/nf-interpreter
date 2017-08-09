//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#ifndef _WINDOWS_DEVICES_GPIO_NATIVE_WINDOWS_DEVICES_GPIO_GPIOCONTROLLER_H_
#define _WINDOWS_DEVICES_GPIO_NATIVE_WINDOWS_DEVICES_GPIO_GPIOCONTROLLER_H_

namespace Windows
{
    namespace Devices
    {
        namespace Gpio
        {
            struct GpioController
            {
                // Helper Functions to access fields of managed object
                // Declaration of stubs. These functions are implemented by Interop code developers
                static bool NativeOpenpin( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr );
                static signed int get_PinCount( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            };
        }
    }
}
#endif  //_WINDOWS_DEVICES_GPIO_NATIVE_WINDOWS_DEVICES_GPIO_GPIOCONTROLLER_H_
