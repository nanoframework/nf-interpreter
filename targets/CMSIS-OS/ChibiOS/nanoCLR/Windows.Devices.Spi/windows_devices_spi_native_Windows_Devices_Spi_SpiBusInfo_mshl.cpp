//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include "windows_devices_spi_native.h"
#include "windows_devices_spi_native_Windows_Devices_Spi_SpiBusInfo.h"

using namespace Windows::Devices::Spi;


HRESULT Library_windows_devices_spi_native_Windows_Devices_Spi_SpiBusInfo::get_MaxClockFrequency___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        signed int retVal = SpiBusInfo::get_MaxClockFrequency( pMngObj,  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_INT32( stack, retVal );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_windows_devices_spi_native_Windows_Devices_Spi_SpiBusInfo::get_MinClockFrequency___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        signed int retVal = SpiBusInfo::get_MinClockFrequency( pMngObj,  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_INT32( stack, retVal );

    }
    NANOCLR_NOCLEANUP();
}
