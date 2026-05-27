//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"
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

__nfweak void SaveNativeEventToHALQueue( CLR_RT_HeapBlock_NativeEventDispatcher *pContext, uint32_t data1, uint32_t data2 )
{
    (void)pContext;
    (void)data1;
    (void)data2;

    NATIVE_PROFILE_CLR_IOPORT();
}

__nfweak void CleanupNativeEventsFromHALQueue( CLR_RT_HeapBlock_NativeEventDispatcher *pContext )
{
    (void)pContext;

    NATIVE_PROFILE_CLR_IOPORT();
}

__nfweak HRESULT CLR_RT_HeapBlock_NativeEventDispatcher::CreateInstance( CLR_RT_HeapBlock& owner, CLR_RT_HeapBlock& portRef )
{
    (void)owner;
    (void)portRef;

    NATIVE_PROFILE_CLR_IOPORT();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_HeapBlock_NativeEventDispatcher::ExtractInstance( CLR_RT_HeapBlock& ref, CLR_RT_HeapBlock_NativeEventDispatcher*& port )
{
    (void)ref;
    (void)port;

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

__nfweak void CLR_RT_HeapBlock_NativeEventDispatcher::SaveToHALQueue( uint32_t data1, uint32_t data2 )
{
    (void)data1;
    (void)data2;

    NATIVE_PROFILE_CLR_IOPORT();
}

__nfweak HRESULT CLR_RT_HeapBlock_NativeEventDispatcher::StartDispatch( CLR_RT_ApplicationInterrupt* appInterrupt, CLR_RT_Thread* th )
{
    (void)appInterrupt;
    (void)th;

    NATIVE_PROFILE_CLR_IOPORT();
    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak void CLR_RT_HeapBlock_NativeEventDispatcher::ThreadTerminationCallback( void* arg )
{
    (void)arg;

    NATIVE_PROFILE_CLR_IOPORT();
}

__nfweak HRESULT CLR_RT_HeapBlock_NativeEventDispatcher::RecoverManagedObject( CLR_RT_HeapBlock*& port )
{
    (void)port;

    NATIVE_PROFILE_CLR_IOPORT();
    NANOCLR_FEATURE_STUB_RETURN();
}
