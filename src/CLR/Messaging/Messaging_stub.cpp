//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <nanoCLR_Runtime.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

__nfweak CLR_Messaging *g_CLR_Messaging;

__nfweak CLR_UINT32     g_scratchMessaging[sizeof(CLR_Messaging) * NUM_MESSAGING / sizeof(UINT32) + 1];

__nfweak HRESULT CLR_Messaging::CreateInstance()
{
    NATIVE_PROFILE_CLR_MESSAGING();
    g_CLR_Messaging = (CLR_Messaging*)&g_scratchMessaging[0];
    NANOCLR_SYSTEM_STUB_RETURN();
}

__nfweak HRESULT CLR_Messaging::DeleteInstance()
{
    NATIVE_PROFILE_CLR_MESSAGING();
    NANOCLR_SYSTEM_STUB_RETURN();
}

__nfweak bool CLR_Messaging::SendEvent( UINT32 cmd, UINT32 payloadSize, UINT8* payload, UINT32 flags )
{
    NATIVE_PROFILE_CLR_MESSAGING();
    return true;
}

__nfweak void CLR_Messaging::PurgeCache()
{
    NATIVE_PROFILE_CLR_MESSAGING();
}

__nfweak void CLR_Messaging::PurgeCache( CLR_RT_DblLinkedList& lst, CLR_INT64 oldest )
{
    NATIVE_PROFILE_CLR_MESSAGING();
}

__nfweak void CLR_Messaging::ProcessCommands()
{
    NATIVE_PROFILE_CLR_MESSAGING();
}

__nfweak void CLR_Messaging::Initialize(COM_HANDLE port, const CLR_Messaging_CommandHandlerLookup* requestLookup, const CLR_UINT32 requestLookupCount, const CLR_Messaging_CommandHandlerLookup* replyLookup, const CLR_UINT32 replyLookupCount, void* owner )
{
    NATIVE_PROFILE_CLR_MESSAGING();
}

__nfweak void CLR_Messaging::ReplyToCommand( WP_Message* msg, bool fSuccess, bool fCritical )
{
    NATIVE_PROFILE_CLR_MESSAGING();
}

__nfweak void CLR_Messaging::ReplyToCommand( WP_Message* msg, bool fSuccess, bool fCritical, void* ptr, int size )
{
    NATIVE_PROFILE_CLR_MESSAGING();
}

__nfweak void CLR_Messaging::Cleanup()
{
    NATIVE_PROFILE_CLR_MESSAGING();
}
