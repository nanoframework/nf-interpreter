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


#ifndef _WINDOWS_DEVICES_GPIO_PROVIDER_NATIVE_WINDOWS_DEVICES_GPIO_PROVIDER_DEFAULTGPIOPINPROVIDER_H_
#define _WINDOWS_DEVICES_GPIO_PROVIDER_NATIVE_WINDOWS_DEVICES_GPIO_PROVIDER_DEFAULTGPIOPINPROVIDER_H_

namespace Windows
{
    namespace Devices
    {
        namespace Gpio
        {
            namespace Provider
            {
                struct DefaultGpioPinProvider
                {
                    // Helper Functions to access fields of managed object
                    static bool& Get__disposed( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_bool( pMngObj, Library_windows_devices_gpio_provider_native_Windows_Devices_Gpio_Provider_DefaultGpioPinProvider::FIELD___disposed ); }

                    static signed int& Get__pinDriveMode( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_windows_devices_gpio_provider_native_Windows_Devices_Gpio_Provider_DefaultGpioPinProvider::FIELD___pinDriveMode ); }

                    static signed int& Get__pinNumber( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_INT32( pMngObj, Library_windows_devices_gpio_provider_native_Windows_Devices_Gpio_Provider_DefaultGpioPinProvider::FIELD___pinNumber ); }

                    // Declaration of stubs. These functions are implemented by Interop code developers
                    static UNSUPPORTED_TYPE get_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
                    static void set_DebounceTimeout( CLR_RT_HeapBlock* pMngObj, UNSUPPORTED_TYPE param0, HRESULT &hr );
                    static void NativeWrite( CLR_RT_HeapBlock* pMngObj, signed int param0, unsigned char param1, HRESULT &hr );
                    static signed int NativeRead( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr );
                    static void NativeSetDriveMode( CLR_RT_HeapBlock* pMngObj, signed int param0, signed int param1, HRESULT &hr );
                    static bool NativeOpenpin( CLR_RT_HeapBlock* pMngObj, signed int param0, HRESULT &hr );
                    static bool NativeIsDriveModeSupported( CLR_RT_HeapBlock* pMngObj, unsigned char param0, HRESULT &hr );
                    static void NativeDispose( CLR_RT_HeapBlock* pMngObj, HRESULT &hr );
                };
            }
        }
    }
}
#endif  //_WINDOWS_DEVICES_GPIO_PROVIDER_NATIVE_WINDOWS_DEVICES_GPIO_PROVIDER_DEFAULTGPIOPINPROVIDER_H_
