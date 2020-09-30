//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOCLR_MESSAGING_H_
#define _NANOCLR_MESSAGING_H_

#include <nanoCLR_Types.h>
#include <WireProtocol.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef bool (*CLR_Messaging_CommandHandler)( WP_Message* msg );

struct CLR_Messaging_CommandHandlerLookup
{
    CLR_Messaging_CommandHandler hnd;
    unsigned int                       cmd;
};

struct CLR_Messaging_CommandHandlerLookups
{
    const CLR_Messaging_CommandHandlerLookup* table;
    CLR_UINT32                                size;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct CLR_Messaging_Commands
{
    static const unsigned int c_Messaging_Query                = 0x00020090; // Checks the presence of an EndPoint.
    static const unsigned int c_Messaging_Send                 = 0x00020091; // Sends a message to an EndPoint.
    static const unsigned int c_Messaging_Reply                = 0x00020092; // Response from an EndPoint.

    struct Messaging_Query
    {
        CLR_RT_HeapBlock_EndPoint::Address m_addr;

        struct Reply
        {
            CLR_UINT32                         m_found;
            CLR_RT_HeapBlock_EndPoint::Address m_addr;
        };
    };

    struct Messaging_Send
    {
        CLR_RT_HeapBlock_EndPoint::Address m_addr;
        unsigned char                              m_data[ 1 ];

        struct Reply
        {
            CLR_UINT32                         m_found;
            CLR_RT_HeapBlock_EndPoint::Address m_addr;
        };
    };

    struct Messaging_Reply
    {
        CLR_RT_HeapBlock_EndPoint::Address m_addr;
        unsigned char                              m_data[ 1 ];

        struct Reply
        {
            CLR_UINT32                         m_found;
            CLR_RT_HeapBlock_EndPoint::Address m_addr;
        };
    };

};


struct CLR_Messaging
{
    struct CachedMessage : public CLR_RT_HeapBlock_Node
    {
        static const CLR_UINT32 c_Processed = 0x00000001;

        CLR_UINT32 m_flags;

        CLR_UINT32 m_size;
        CLR_INT64  m_lastSeen;
        WP_Message m_message;
    };

    //--//

    CLR_Messaging_CommandHandlerLookups m_Lookup_Requests[ 2 ];
    CLR_Messaging_CommandHandlerLookups m_Lookup_Replies[ 2 ];
    
    COM_HANDLE           m_port;

    //--//

    static HRESULT CreateInstance();

    void Initialize(const CLR_Messaging_CommandHandlerLookup* requestLookup, const CLR_UINT32 requestLookupCount, const CLR_Messaging_CommandHandlerLookup* replyLookup, const CLR_UINT32 replyLookupCount );
    void Cleanup();

    static HRESULT DeleteInstance();

    //void ProcessCommands();

    bool        SendEvent     ( unsigned int cmd, unsigned int payloadSize, unsigned char* payload, unsigned int flags );
    static void BroadcastEvent( unsigned int cmd, unsigned int payloadSize, unsigned char* payload, unsigned int flags );

    static bool App_ProcessHeader ( void* state,  WP_Message* msg );
    static bool App_ProcessPayload( void* state,  WP_Message* msg );
    static bool App_Release       ( void* state,  WP_Message* msg );

    bool IsDebuggerInitialized() { return m_fDebuggerInitialized; }
    void InitializeDebugger() { m_fDebuggerInitialized = (DebuggerPort_Initialize( m_port ) == true); }

private:

    bool m_fInitialized;
    bool m_fDebuggerInitialized;

    bool AllocateAndQueueMessage( CLR_UINT32 cmd, unsigned int length, unsigned char* data, CLR_RT_HeapBlock_EndPoint::Port port, CLR_RT_HeapBlock_EndPoint::Address addr, CLR_UINT32 found );

    bool ProcessHeader ( WP_Message* msg );



    

    

public:  
    bool ProcessPayload( WP_Message* msg );
    static bool Messaging_Query               ( WP_Message* msg );
    static bool Messaging_Query__Reply        ( WP_Message* msg );
    static bool Messaging_Send                ( WP_Message* msg );
    static bool Messaging_Send__Reply         ( WP_Message* msg );
    static bool Messaging_Reply               ( WP_Message* msg );
    static bool Messaging_Reply__Reply        ( WP_Message* msg );
};

//--//

#endif // _NANOCLR_MESSAGING_H_

