//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <nanoCLR_Runtime.h>

////////////////////////////////////////////////////////////////////////////////////////////////////


__nfweak void CLR_RT_HeapBlock_EndPoint::HandlerMethod_Initialize()
{
}

__nfweak void CLR_RT_HeapBlock_EndPoint::HandlerMethod_RecoverFromGC()
{
}

__nfweak void CLR_RT_HeapBlock_EndPoint::HandlerMethod_CleanUp()
{
}

__nfweak CLR_RT_HeapBlock_EndPoint* CLR_RT_HeapBlock_EndPoint::FindEndPoint( const CLR_RT_HeapBlock_EndPoint::Port& port )
{
    (void)port;

    return NULL;
}

__nfweak bool CLR_RT_HeapBlock_EndPoint::Port::Compare( const CLR_RT_HeapBlock_EndPoint::Port& port )
{
    (void)port;

    return false;
}

//--//

__nfweak HRESULT CLR_RT_HeapBlock_EndPoint::CreateInstance( const CLR_RT_HeapBlock_EndPoint::Port& port, CLR_RT_HeapBlock& owner, CLR_RT_HeapBlock& epRef )
{
    (void)port;
    (void)owner;
    (void)epRef;

    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak HRESULT CLR_RT_HeapBlock_EndPoint::ExtractInstance( CLR_RT_HeapBlock& ref, CLR_RT_HeapBlock_EndPoint*& endPoint )
{
    (void)ref;
    (void)endPoint;

    NANOCLR_FEATURE_STUB_RETURN();
}

__nfweak bool CLR_RT_HeapBlock_EndPoint::ReleaseWhenDeadEx()
{
    return true;
}

//--//

__nfweak void CLR_RT_HeapBlock_EndPoint::RecoverFromGC()
{
}

//--//

__nfweak CLR_RT_HeapBlock_EndPoint::Message* CLR_RT_HeapBlock_EndPoint::FindMessage( CLR_UINT32 cmd, const CLR_UINT32* seq )
{
    (void)cmd;
    (void)seq;
    
    return NULL;
}
