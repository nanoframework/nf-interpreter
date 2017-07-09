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
                // Declaration of stubs. These functions are implemented by Interop code developers
                static void NativeTransfer( CLR_RT_HeapBlock* pMngObj, const char* param0, CLR_RT_TypedArray_UINT8 param1, CLR_RT_TypedArray_UINT8 param2, bool param3, HRESULT &hr );
                static void NativeInit( CLR_RT_HeapBlock* pMngObj, const char* param0, CLR_RT_TypedArray_INT32 param1, HRESULT &hr );
                static const char* GetDeviceSelector( HRESULT &hr );
            };
        }
    }
}
#endif  //_WINDOWS_DEVICES_SPI_NATIVE_WINDOWS_DEVICES_SPI_SPIDEVICE_H_
