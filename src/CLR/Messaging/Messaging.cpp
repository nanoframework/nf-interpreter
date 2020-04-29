//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Runtime__HeapBlock.h>
#include <nanoCLR_Messaging.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>

// the Arm 3.0 compiler drags in a bunch of ABI methods (for initialization) if struct arrays are not initialized
CLR_UINT32     g_scratchMessaging[ sizeof(CLR_Messaging) ];
CLR_Messaging *g_CLR_Messaging;

////////////////////////////////////////////////////////////////////////////////////////////////////

//--//

static const CLR_Messaging_CommandHandlerLookup c_Messaging_Lookup_Request[] =
{
    {  CLR_Messaging::Messaging_Query, CLR_Messaging_Commands::c_Messaging_Query },
    {  CLR_Messaging::Messaging_Send,  CLR_Messaging_Commands::c_Messaging_Send  },
    {  CLR_Messaging::Messaging_Reply, CLR_Messaging_Commands::c_Messaging_Reply },
};

static const CLR_Messaging_CommandHandlerLookup c_Messaging_Lookup_Reply[] =
{
    {  CLR_Messaging::Messaging_Query, CLR_Messaging_Commands::c_Messaging_Query },
    {  CLR_Messaging::Messaging_Send,  CLR_Messaging_Commands::c_Messaging_Send  },
    {  CLR_Messaging::Messaging_Reply, CLR_Messaging_Commands::c_Messaging_Reply },
};


//--//

bool CLR_Messaging::AllocateAndQueueMessage( CLR_UINT32 cmd, unsigned int length, unsigned char* data, CLR_RT_HeapBlock_EndPoint::Port port, CLR_RT_HeapBlock_EndPoint::Address addr, CLR_UINT32 found )
{
    NATIVE_PROFILE_CLR_MESSAGING();
    CLR_RT_HeapBlock_EndPoint::Message* rpc;
    CLR_RT_HeapBlock_EndPoint*          ep;
    
    if((ep = CLR_RT_HeapBlock_EndPoint::FindEndPoint( port )) == NULL) return false;

    {
        CLR_RT_ProtectFromGC gc( *ep );

        if((rpc = (CLR_RT_HeapBlock_EndPoint::Message*)CLR_RT_Memory::Allocate( sizeof(*rpc) + length, CLR_RT_HeapBlock::HB_CompactOnFailure )) == NULL) return false;
    
        rpc->ClearData();
        
        rpc->m_cmd       = cmd;;
        rpc->m_addr      = addr;
        rpc->m_length    = length;
        rpc->m_found     = found;
        
        if(data) memcpy( rpc->m_data, data, length );

        ep->m_messages.LinkAtBack( rpc );

        g_CLR_RT_ExecutionEngine.SignalEvents( CLR_RT_ExecutionEngine::c_Event_EndPoint );
    }

    return true;
}


bool CLR_Messaging::Messaging_Query( WP_Message* msg )
{
    NATIVE_PROFILE_CLR_MESSAGING();
    CLR_Messaging_Commands::Messaging_Query*       cmd = (CLR_Messaging_Commands::Messaging_Query*)msg->m_payload;
    CLR_Messaging_Commands::Messaging_Query::Reply res;
    CLR_RT_HeapBlock_EndPoint*                      ep = CLR_RT_HeapBlock_EndPoint::FindEndPoint( cmd->m_addr.m_to );

    res.m_found = (ep != NULL);
    res.m_addr  = cmd->m_addr;

    WP_ReplyToCommand(msg, true, false, &res, sizeof(res));

    return true;
}

bool CLR_Messaging::Messaging_Query__Reply( WP_Message* msg )
{
    NATIVE_PROFILE_CLR_MESSAGING();
    
    CLR_Messaging_Commands::Messaging_Query::Reply* cmd = (CLR_Messaging_Commands::Messaging_Query::Reply*)msg->m_payload;
    
    g_CLR_Messaging->AllocateAndQueueMessage( CLR_Messaging_Commands::c_Messaging_Query, 0, NULL, cmd->m_addr.m_from, cmd->m_addr, cmd->m_found );
        
    return true;
}

//--//

bool CLR_Messaging::Messaging_Send( WP_Message* msg )
{
    NATIVE_PROFILE_CLR_MESSAGING();
    CLR_Messaging_Commands::Messaging_Send*       cmd  = (CLR_Messaging_Commands::Messaging_Send*)msg->m_payload;
    CLR_Messaging_Commands::Messaging_Send::Reply res;
    CLR_UINT32                                    len;
    bool                                          fRes;

    len = msg->m_header.m_size - sizeof(cmd->m_addr);
        
    fRes = g_CLR_Messaging->AllocateAndQueueMessage( CLR_Messaging_Commands::c_Messaging_Send, len, cmd->m_data, cmd->m_addr.m_to, cmd->m_addr, false );

    res.m_found = true;
    res.m_addr  = cmd->m_addr;

    WP_ReplyToCommand( msg, fRes, false, &res, sizeof(res) );

    return true;
}

bool CLR_Messaging::Messaging_Send__Reply( WP_Message* msg )
{
    (void)msg;

    NATIVE_PROFILE_CLR_MESSAGING();
    //
    // retransmission support is currently not implemented
    //

    return true;
}

//--//

bool CLR_Messaging::Messaging_Reply( WP_Message* msg )
{
    NATIVE_PROFILE_CLR_MESSAGING();
    CLR_Messaging_Commands::Messaging_Reply*       cmd  = (CLR_Messaging_Commands::Messaging_Reply*)msg->m_payload;
    CLR_Messaging_Commands::Messaging_Reply::Reply res;
    bool                                           fRes;
    CLR_UINT32                                     len;

    len = msg->m_header.m_size - sizeof(cmd->m_addr);
    fRes = g_CLR_Messaging->AllocateAndQueueMessage( CLR_Messaging_Commands::c_Messaging_Reply, len, cmd->m_data, cmd->m_addr.m_from, cmd->m_addr, false );

    res.m_found = true;
    res.m_addr  = cmd->m_addr;

    WP_ReplyToCommand( msg, fRes, false, &res, sizeof(res) );

    return true;
}

bool CLR_Messaging::Messaging_Reply__Reply( WP_Message* msg )
{
    (void)msg;

    NATIVE_PROFILE_CLR_MESSAGING();
    //
    // retransmission support  is currently not implemented
    //

    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////


//--//

bool CLR_Messaging::App_ProcessHeader( void* state, WP_Message*  msg )
{
    NATIVE_PROFILE_CLR_MESSAGING();
    CLR_Messaging* pThis = (CLR_Messaging*)state;

    Watchdog_Reset();

    if( !pThis->ProcessHeader( msg ) )
    {
        TRACE0("ProcessHeader() indicated invalid header!\n");
        return false;
    }

    if(msg->m_header.m_size)
    {
        void* ptr = CLR_RT_Memory::Allocate( msg->m_header.m_size, CLR_RT_HeapBlock::HB_CompactOnFailure );

        if(ptr == NULL)
        {
            TRACE0("Failed to allocate 0x%08X bytes for message payload!\n");
            return false;
        }

        msg->m_payload = (unsigned char*)ptr;
    }

    return true;
}

bool CLR_Messaging::App_ProcessPayload( void* state, WP_Message* msg )
{
    NATIVE_PROFILE_CLR_MESSAGING();
    CLR_Messaging* pThis = (CLR_Messaging*)state;

    Watchdog_Reset();

    if(pThis->ProcessPayload( msg ) == false)
    {
        return false;
    }

    return true;
}

bool CLR_Messaging::App_Release( void* state, WP_Message* msg )
{
    (void)state;

    NATIVE_PROFILE_CLR_MESSAGING();
    if(msg->m_payload != NULL)
    {
        CLR_RT_Memory::Release( msg->m_payload );

        msg->m_payload = NULL;
    }

    return true;
}

//--//














////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CLR_Messaging::CreateInstance()
{
    NATIVE_PROFILE_CLR_MESSAGING();
    NANOCLR_HEADER();

    g_CLR_Messaging = (CLR_Messaging*)&g_scratchMessaging[ 0 ];

    CLR_RT_Memory::ZeroFill( g_CLR_Messaging, sizeof(CLR_Messaging) );
    
    g_CLR_Messaging->Initialize(
        NULL, 
        0, 
        NULL, 
        0
        );

    NANOCLR_NOCLEANUP_NOLABEL();
}

//--//

void CLR_Messaging::Initialize(
    const CLR_Messaging_CommandHandlerLookup* requestLookup, 
    const CLR_UINT32                          requestLookupCount, 
    const CLR_Messaging_CommandHandlerLookup* replyLookup, 
    const CLR_UINT32                          replyLookupCount
    )
{
    if(m_fInitialized) return;

    m_Lookup_Requests[ 0 ].table = c_Messaging_Lookup_Request;
    m_Lookup_Requests[ 0 ].size  = ARRAYSIZE(c_Messaging_Lookup_Request);

    m_Lookup_Replies[ 0 ].table = c_Messaging_Lookup_Reply;
    m_Lookup_Replies[ 0 ].size  = ARRAYSIZE(c_Messaging_Lookup_Reply);

    m_port           = HalSystemConfig.DebuggerPort;

    m_Lookup_Requests[ 1 ].table = requestLookup;
    m_Lookup_Requests[ 1 ].size  = requestLookupCount;

    m_Lookup_Replies[ 1 ].table = replyLookup;
    m_Lookup_Replies[ 1 ].size  = replyLookupCount;

    m_fDebuggerInitialized = (DebuggerPort_Initialize( HalSystemConfig.DebuggerPort ) != false);

    m_fInitialized = true;

}

HRESULT CLR_Messaging::DeleteInstance()
{
    NATIVE_PROFILE_CLR_MESSAGING();
    NANOCLR_HEADER();

    g_CLR_Messaging->Cleanup();

    NANOCLR_NOCLEANUP_NOLABEL();
}

void CLR_Messaging::Cleanup()
{
    NATIVE_PROFILE_CLR_MESSAGING();

    if(!m_fInitialized) return;

    // Some devices cannot reset the USB controller so we need to allow them to skip uninitialization
    // of the debug transport
    if(!g_fDoNotUninitializeDebuggerPort)
    {
        DebuggerPort_Uninitialize( m_port );
    }
    
    m_fDebuggerInitialized = false;

    m_fInitialized = false;
}

//--//










bool CLR_Messaging::ProcessHeader( WP_Message* msg )
{
    (void)msg;

    NATIVE_PROFILE_CLR_MESSAGING();
    TRACE("MSG: 0x%08X\n", msg->m_header.m_cmd );
    return true;
}

bool CLR_Messaging::ProcessPayload( WP_Message* msg )
{
    NATIVE_PROFILE_CLR_MESSAGING();
    if(msg->m_header.m_flags & WP_Flags_c_NACK)
    {
        //
        // Bad packet...
        //
        return true;
    }

    //--//

    const CLR_Messaging_CommandHandlerLookups* tables;
    int tableCount = 0;

    if(msg->m_header.m_flags & WP_Flags_c_Reply)
    {
        tables     = m_Lookup_Replies;
        tableCount = ARRAYSIZE(m_Lookup_Replies);
    }
    else
    {
        tables     = m_Lookup_Requests;
        tableCount = ARRAYSIZE(m_Lookup_Requests);
    }

    while(tableCount-- > 0)
    {
        size_t                                    num = tables->size;
        const CLR_Messaging_CommandHandlerLookup* cmd = tables->table;

        while(num-- > 0 && cmd != NULL)
        {
            if(cmd->cmd == msg->m_header.m_cmd)
            {
                WP_ReplyToCommand(msg, (*(cmd->hnd))( msg ), false, NULL, 0);
                return true;
            }

            cmd++;
        }
        tables++;
    }

    WP_ReplyToCommand(msg, false, false, NULL, 0);
    
    return true;
}

// wrapper function for CLR_Messaging::ProcessPayload(
extern "C" int CLR_Messaging_ProcessPayload(WP_Message* msg)
{
    if (g_CLR_DBG_Debugger == NULL)
    {
        return false;
    }
    
    bool retValue = g_CLR_DBG_Debugger->m_messaging->ProcessPayload(msg);
    return retValue;
}

//--//

bool CLR_Messaging::SendEvent( unsigned int cmd, unsigned int payloadSize, unsigned char* payload, unsigned int flags )
{
    NATIVE_PROFILE_CLR_MESSAGING();
    WP_PrepareAndSendProtocolMessage(cmd, payloadSize, payload, flags);

    // FIXME: returning true to keep compatibility with previous code base
    // improve this in a future revision
    return true;
}

void CLR_Messaging::BroadcastEvent( unsigned int cmd, unsigned int payloadSize, unsigned char* payload, unsigned int flags )
{
    NATIVE_PROFILE_CLR_MESSAGING();
    WP_PrepareAndSendProtocolMessage(cmd, payloadSize, payload, flags);
}

//--//
