//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include "windows_devices_spi_native.h"
#include "windows_devices_spi_native_Windows_Devices_Spi_SpiDevice.h"

using namespace Windows::Devices::Spi;


HRESULT Library_windows_devices_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__STRING__SZARRAY_U1__SZARRAY_U1__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        const char* param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param0 ) );

        CLR_RT_TypedArray_UINT8 param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8_ARRAY( stack, 2, param1 ) );

        CLR_RT_TypedArray_UINT8 param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT8_ARRAY( stack, 3, param2 ) );

        bool param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_bool( stack, 4, param3 ) );

        SpiDevice::NativeTransfer( pMngObj,  param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_windows_devices_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__STRING__SZARRAY_U2__SZARRAY_U2__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        const char* param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param0 ) );

        CLR_RT_TypedArray_UINT16 param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16_ARRAY( stack, 2, param1 ) );

        CLR_RT_TypedArray_UINT16 param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16_ARRAY( stack, 3, param2 ) );

        bool param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_bool( stack, 4, param3 ) );

        SpiDevice::NativeTransfer( pMngObj,  param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_windows_devices_spi_native_Windows_Devices_Spi_SpiDevice::NativeInit___VOID__STRING__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        const char* param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_LPCSTR( stack, 1, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 2, param1 ) );

        signed int param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 3, param2 ) );

        signed int param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 4, param3 ) );

        SpiDevice::NativeInit( pMngObj,  param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_windows_devices_spi_native_Windows_Devices_Spi_SpiDevice::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        const char* retVal = SpiDevice::GetDeviceSelector( hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_LPCSTR( stack, retVal );

    }
    NANOCLR_NOCLEANUP();
}
