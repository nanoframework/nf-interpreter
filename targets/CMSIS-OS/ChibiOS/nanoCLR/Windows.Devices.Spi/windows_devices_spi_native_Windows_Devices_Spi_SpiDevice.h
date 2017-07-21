//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WINDOWS_DEVICES_SPI_NATIVE_WINDOWS_DEVICES_SPI_SPIDEVICE_H_
#define _WINDOWS_DEVICES_SPI_NATIVE_WINDOWS_DEVICES_SPI_SPIDEVICE_H_

namespace Windows
{
    namespace Devices
    {
        namespace Spi
        {
            struct SpiDevice
            {
                // Helper Functions to access fields of managed object
                // TODO not sure why this showing up here
                // static const char*& Get__spiBus( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_LPCSTR( pMngObj, Library_windows_devices_spi_native_Windows_Devices_Spi_SpiDevice::FIELD___spiBus ); }

                // static const char*& Get__deviceId( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_LPCSTR( pMngObj, Library_windows_devices_spi_native_Windows_Devices_Spi_SpiDevice::FIELD___deviceId ); }

                //static UNSUPPORTED_TYPE& Get_<ConnectionSettings>k__BackingField( CLR_RT_HeapBlock* pMngObj )    { return Interop_Marshal_GetField_UNSUPPORTED_TYPE( pMngObj, Library_windows_devices_spi_native_Windows_Devices_Spi_SpiDevice::FIELD__<ConnectionSettings>k__BackingField ); }

                // Declaration of stubs. These functions are implemented by Interop code developers
                static void NativeTransfer( CLR_RT_HeapBlock* pMngObj, const char* param0, CLR_RT_TypedArray_UINT8 param1, CLR_RT_TypedArray_UINT8 param2, bool param3, HRESULT &hr );
                static void NativeTransfer( CLR_RT_HeapBlock* pMngObj, const char* param0, CLR_RT_TypedArray_UINT16 param1, CLR_RT_TypedArray_UINT16 param2, bool param3, HRESULT &hr );
                static void NativeInit( CLR_RT_HeapBlock* pMngObj, const char* param0, signed int param1, signed int param2, signed int param3, HRESULT &hr );
                static const char* GetDeviceSelector( HRESULT &hr );
            };
        }
    }
}
#endif  //_WINDOWS_DEVICES_SPI_NATIVE_WINDOWS_DEVICES_SPI_SPIDEVICE_H_
