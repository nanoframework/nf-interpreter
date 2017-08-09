//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "windows_devices_gpio_native.h"
#include "windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin.h"

using namespace Windows::Devices::Gpio;


HRESULT Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::get_DebounceTimeout___mscorlibSystemTimeSpan( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        signed int retVal = GpioPin::get_DebounceTimeout( pMngObj,  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_INT32( stack, retVal );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::set_DebounceTimeout___VOID__mscorlibSystemTimeSpan( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param0 ) );

        GpioPin::set_DebounceTimeout( pMngObj,  param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::Read___WindowsDevicesGpioGpioPinValue( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        signed int retVal = GpioPin::Read( pMngObj,  hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_INT32( stack, retVal );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::Write___VOID__WindowsDevicesGpioGpioPinValue( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param0 ) );

        GpioPin::Write( pMngObj,  param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        GpioPin::DisposeNative( pMngObj,  hr );
        NANOCLR_CHECK_HRESULT( hr );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::NativeIsDriveModeSupported___BOOLEAN__WindowsDevicesGpioGpioPinDriveMode( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param0 ) );

        bool retVal = GpioPin::NativeIsDriveModeSupported( pMngObj,  param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_bool( stack, retVal );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetDriveMode___VOID__WindowsDevicesGpioGpioPinDriveMode( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param0 ) );

        GpioPin::NativeSetDriveMode( pMngObj,  param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::NativeInit___BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {
        CLR_RT_HeapBlock* pMngObj = Interop_Marshal_RetrieveManagedObject( stack );

        FAULT_ON_NULL(pMngObj);

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param0 ) );

        bool retVal = GpioPin::NativeInit( pMngObj,  param0, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_bool( stack, retVal );

    }
    NANOCLR_NOCLEANUP();
}
