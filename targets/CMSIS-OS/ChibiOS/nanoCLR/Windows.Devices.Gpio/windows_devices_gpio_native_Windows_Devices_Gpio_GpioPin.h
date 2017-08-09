//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
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
                static UNSUPPORTED_TYPE& Get__syncLock( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___syncLock ); }

                static signed int& Get__pinNumber( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___pinNumber ); }

                static signed int& Get__driveMode( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___driveMode ); }

                static UNSUPPORTED_TYPE& Get__callbacks( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___callbacks ); }

                static bool& Get__disposedValue( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_bool( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ); }

                // Declaration of stubs. These functions are implemented by Interop code developers
                static signed int get_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
                static void set_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr );
                static signed int Read( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
                static void Write( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr );
                static void DisposeNative( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
                static bool NativeIsDriveModeSupported( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr );
                static void NativeSetDriveMode( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr );
                static bool NativeInit( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr );
            };
        }
    }
}
#endif  //_WINDOWS_DEVICES_GPIO_NATIVE_WINDOWS_DEVICES_GPIO_GPIOPIN_H_
