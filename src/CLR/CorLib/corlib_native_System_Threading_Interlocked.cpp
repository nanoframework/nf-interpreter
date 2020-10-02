//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "CorLib.h"


HRESULT Library_corlib_native_System_Threading_Interlocked::Increment___STATIC__I4__BYREF_I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock heapLocation;
        NANOCLR_CHECK_HRESULT(heapLocation.LoadFromReference( stack.Arg0() )); 
        signed int &location = heapLocation.NumericByRef().s4;
        
        // Increment the value passed by reference
        location++;

        SetResult_INT32( stack, location );

        NANOCLR_CHECK_HRESULT(heapLocation.StoreToReference( stack.Arg0(), 0 ));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_corlib_native_System_Threading_Interlocked::Decrement___STATIC__I4__BYREF_I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock heapLocation;
        NANOCLR_CHECK_HRESULT(heapLocation.LoadFromReference( stack.Arg0() )); 
        signed int &location = heapLocation.NumericByRef().s4;
        
        // Decrement the value passed by reference
        location--;

        SetResult_INT32( stack, location );

        NANOCLR_CHECK_HRESULT(heapLocation.StoreToReference( stack.Arg0(), 0 ));
    }
    NANOCLR_NOCLEANUP();
}


HRESULT Library_corlib_native_System_Threading_Interlocked::Exchange___STATIC__I4__BYREF_I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock heapLocation;
        NANOCLR_CHECK_HRESULT(heapLocation.LoadFromReference( stack.Arg0() )); 
        signed int &location = heapLocation.NumericByRef().s4;

        // Always return value of first parameter
        signed int retVal = location; 
        // Move second parameter into the first.
        location = stack.Arg1().NumericByRef().s4;

        SetResult_INT32( stack, retVal );

        NANOCLR_CHECK_HRESULT(heapLocation.StoreToReference( stack.Arg0(), 0 ));
    }
    NANOCLR_NOCLEANUP();
} 


HRESULT Library_corlib_native_System_Threading_Interlocked::CompareExchange___STATIC__I4__BYREF_I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_CORE();
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock heapLocation;
        NANOCLR_CHECK_HRESULT(heapLocation.LoadFromReference( stack.Arg0() )); 
        signed int &location = heapLocation.NumericByRef().s4;

        // Always return value of first parameter
        signed int retVal = location; 
        // Exchange value if first and third parameters has equal value.
        if (stack.Arg2().NumericByRef().s4 == location)
        {
            location = stack.Arg1().NumericByRef().s4;
        }

        SetResult_INT32( stack, retVal );

        NANOCLR_CHECK_HRESULT(heapLocation.StoreToReference( stack.Arg0(), 0 ));
    }
    NANOCLR_NOCLEANUP();
} 
