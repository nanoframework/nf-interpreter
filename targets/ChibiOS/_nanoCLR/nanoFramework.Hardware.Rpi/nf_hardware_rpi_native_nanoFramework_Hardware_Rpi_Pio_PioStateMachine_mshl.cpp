//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_rpi_native.h"
#include "nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine.h"

using namespace nf_hardware_rpi_native::nanoFramework_Hardware_Rpi;


HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeInit___STATIC__VOID__I4__I4__I4__SZARRAY_U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        signed int param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 2, param2 ) );

        CLR_RT_TypedArray_UINT32 param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32_ARRAY( stack, 3, param3 ) );

        PioStateMachine::NativeInit( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeSetEnabled___STATIC__VOID__I4__I4__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        bool param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_bool( stack, 2, param2 ) );

        PioStateMachine::NativeSetEnabled( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativePutBlocking___STATIC__VOID__I4__I4__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        unsigned int param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT32( stack, 2, param2 ) );

        PioStateMachine::NativePutBlocking( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeGetBlocking___STATIC__U4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        unsigned int retValue = PioStateMachine::NativeGetBlocking( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeTxFull___STATIC__BOOLEAN__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        bool retValue = PioStateMachine::NativeTxFull( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_bool( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeRxEmpty___STATIC__BOOLEAN__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        bool retValue = PioStateMachine::NativeRxEmpty( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_bool( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeUnclaim___STATIC__VOID__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        PioStateMachine::NativeUnclaim( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeSetConsecutivePinDirs___STATIC__VOID__I4__I4__I4__I4__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        signed int param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 2, param2 ) );

        signed int param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 3, param3 ) );

        bool param4;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_bool( stack, 4, param4 ) );

        PioStateMachine::NativeSetConsecutivePinDirs( param0, param1, param2, param3, param4, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeClearFifos___STATIC__VOID__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        PioStateMachine::NativeClearFifos( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeDrainTxFifo___STATIC__VOID__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        PioStateMachine::NativeDrainTxFifo( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeRestart___STATIC__VOID__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        PioStateMachine::NativeRestart( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeClkDivRestart___STATIC__VOID__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        PioStateMachine::NativeClkDivRestart( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeExec___STATIC__VOID__I4__I4__U2( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        uint16_t param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_UINT16( stack, 2, param2 ) );

        PioStateMachine::NativeExec( param0, param1, param2, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeTxLevel___STATIC__U4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        unsigned int retValue = PioStateMachine::NativeTxLevel( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeRxLevel___STATIC__U4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        unsigned int retValue = PioStateMachine::NativeRxLevel( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeGetPc___STATIC__U4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        unsigned int retValue = PioStateMachine::NativeGetPc( param0, param1, hr );
        NANOCLR_CHECK_HRESULT( hr );
        SetResult_UINT32( stack, retValue );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_rpi_native_nanoFramework_Hardware_Rpi_Pio_PioStateMachine::NativeSetClockDivisor___STATIC__VOID__I4__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER(); hr = S_OK;
    {

        signed int param0;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 0, param0 ) );

        signed int param1;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 1, param1 ) );

        signed int param2;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 2, param2 ) );

        signed int param3;
        NANOCLR_CHECK_HRESULT( Interop_Marshal_INT32( stack, 3, param3 ) );

        PioStateMachine::NativeSetClockDivisor( param0, param1, param2, param3, hr );
        NANOCLR_CHECK_HRESULT( hr );

    }
    NANOCLR_NOCLEANUP();
}
