//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------


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

                static signed int& Get__driveMode( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___driveMode ); }

                static bool& Get__disposedValue( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_bool( pMngObj, Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ); }

                // Declaration of stubs. These functions are implemented by Interop code developers
                static bool NativeIsDriveModeSupported( CLR_RT_HeapBlock* pMngObj, unsigned char param0, HRESULT &hr );
                static signed int NativeRead( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr );
                static void NativeWrite( CLR_RT_HeapBlock* pMngObj, signed int param0, unsigned char param1, HRESULT &hr );
                static void NativeSetDriveMode( CLR_RT_HeapBlock* pMngObj, signed int param0, unsigned char param1, HRESULT &hr );
                static signed int get_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
                static void set_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr );
                static void DisposeNative( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
            };
        }
    }
}
#endif  //_WINDOWS_DEVICES_GPIO_NATIVE_WINDOWS_DEVICES_GPIO_GPIOPIN_H_
