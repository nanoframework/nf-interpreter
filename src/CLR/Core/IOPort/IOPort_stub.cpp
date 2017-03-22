//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <nanoCLR_Runtime.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

__nfweak void CLR_RT_HeapBlock_NativeEventDispatcher::HandlerMethod_Initialize()
{
    NATIVE_PROFILE_CLR_IOPORT();
}

__nfweak void CLR_RT_HeapBlock_NativeEventDispatcher::HandlerMethod_RecoverFromGC()
{
    NATIVE_PROFILE_CLR_IOPORT();
}

__nfweak void CLR_RT_HeapBlock_NativeEventDispatcher::HandlerMethod_CleanUp()
{
    NATIVE_PROFILE_CLR_IOPORT();
}

__nfweak void SaveNativeEventToHALQueue( CLR_RT_HeapBlock_NativeEventDispatcher *pContext, unsigned int data1, unsigned int data2 )
{
    NATIVE_PROFILE_CLR_IOPORT();
}

__nfweak void CleanupNativeEventsFromHALQueue( CLR_RT_HeapBlock_NativeEventDispatcher *pContext )
{
    NATIVE_PROFILE_CLR_IOPORT();
}

__nfweak HRESULT CLR_RT_HeapBlock_NativeEventDispatcher::CreateInstance( CLR_RT_HeapBlock& owner, CLR_RT_HeapBlock& portRef )
{
    NATIVE_PROFILE_CLR_IOPORT();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_HeapBlock_NativeEventDispatcher::ExtractInstance( CLR_RT_HeapBlock& ref, CLR_RT_HeapBlock_NativeEventDispatcher*& port )
{
    NATIVE_PROFILE_CLR_IOPORT();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak void CLR_RT_HeapBlock_NativeEventDispatcher::RecoverFromGC()
{
    NATIVE_PROFILE_CLR_IOPORT();
}

__nfweak bool CLR_RT_HeapBlock_NativeEventDispatcher::ReleaseWhenDeadEx()
{
    NATIVE_PROFILE_CLR_IOPORT();
    return true;
}

__nfweak void CLR_RT_HeapBlock_NativeEventDispatcher::RemoveFromHALQueue()
{
    NATIVE_PROFILE_CLR_IOPORT();
}
__nfweak void CLR_RT_HeapBlock_NativeEventDispatcher::SaveToHALQueue( unsigned int data1, unsigned int data2 )
{
    NATIVE_PROFILE_CLR_IOPORT();
}

__nfweak HRESULT CLR_RT_HeapBlock_NativeEventDispatcher::StartDispatch( CLR_RT_ApplicationInterrupt* appInterrupt, CLR_RT_Thread* th )
{
    NATIVE_PROFILE_CLR_IOPORT();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak void CLR_RT_HeapBlock_NativeEventDispatcher::ThreadTerminationCallback( void* arg )
{
    NATIVE_PROFILE_CLR_IOPORT();
}

__nfweak HRESULT CLR_RT_HeapBlock_NativeEventDispatcher::RecoverManagedObject( CLR_RT_HeapBlock*& port )
{
    NATIVE_PROFILE_CLR_IOPORT();
    NANOCLR_FEATURE_STUB_RETURN();
}
