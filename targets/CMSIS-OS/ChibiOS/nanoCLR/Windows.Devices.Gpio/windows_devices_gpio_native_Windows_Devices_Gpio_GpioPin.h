//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WINDOWS_DEVICES_GPIO_NATIVE_WINDOWS_DEVICES_GPIO_GPIOPIN_H_
#define _WINDOWS_DEVICES_GPIO_NATIVE_WINDOWS_DEVICES_GPIO_GPIOPIN_H_

namespace Windows
{
    namespace Devices
    {
        namespace Gpio
        {
            struct GpioPin
            {
                // Helper Functions to access fields of managed object
                static signed int& Get__pinNumber( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___pinNumber ); }

                static bool& Get_disposedValue( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_bool( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD__disposedValue ); }

                // Declaration of stubs. These functions are implemented by Interop code developers
                static signed int get_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
                static void set_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr );
                static void DisposeNative( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            };
        }
    }
}
#endif  //_WINDOWS_DEVICES_GPIO_NATIVE_WINDOWS_DEVICES_GPIO_GPIOPIN_H_
