//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_MonitorCommands.h>

// Initialize to a packet sequence number impossible to encounter
static uint32_t lastPacketSequence = 0x00FEFFFF;

// defining this array here makes is local helping reduce the image size because of compiler opmitizations
static const CommandHandlerLookup c_Lookup_Request[] =
{
    /*******************************************************************************************************************************************************************/
#define DEFINE_CMD(cmd) { CLR_DBG_Commands_c_Monitor_##cmd, &Monitor_##cmd }
    DEFINE_CMD(Ping       ),
    DEFINE_CMD(Reboot     ),
    // //
    DEFINE_CMD(ReadMemory ),
    DEFINE_CMD(WriteMemory),
    DEFINE_CMD(CheckMemory),
    DEFINE_CMD(EraseMemory),
    DEFINE_CMD(QueryConfiguration),
    DEFINE_CMD(UpdateConfiguration),
    // //
    // DEFINE_CMD(Execute    ),
    DEFINE_CMD(MemoryMap  ),
    // //
    // DEFINE_CMD(CheckSignature),
    // //
    DEFINE_CMD(FlashSectorMap),
    // DEFINE_CMD(SignatureKeyUpdate),
    
    DEFINE_CMD(OemInfo),

#undef DEFINE_CMD
    /*******************************************************************************************************************************************************************/
};

////////////////////////////////////////////////////

// defining this array here makes is local helping reduce the image size because of compiler opmitizations
static const CommandHandlerLookup c_Lookup_Reply[] =
{
    /*******************************************************************************************************************************************************************/
#define DEFINE_CMD(cmd) { CLR_DBG_Commands_c_Monitor_##cmd, &Monitor_##cmd }
    DEFINE_CMD(Ping),
#undef DEFINE_CMD
    /*******************************************************************************************************************************************************************/
};

int WP_App_ProcessHeader(WP_Message* message)
{
    // check for reception buffer overflow 
    if(message->m_header.m_size > sizeof(receptionBuffer))
    {
        return false;
    }

    message->m_payload = receptionBuffer;
    return true;
}

int WP_App_ProcessPayload(WP_Message* message)
{
    // Prevent processing duplicate packets
    if(message->m_header.m_seq == lastPacketSequence)
    {    
        return false;       // Do not even respond to a repeat packet
    }

    // save this packet sequence number
    lastPacketSequence = message->m_header.m_seq;

    if(message->m_header.m_flags & WP_Flags_c_NACK)
    {
        //
        // Bad packet...
        //
        return true;
    }

    size_t  num;
    const CommandHandlerLookup* cmd;

    if(message->m_header.m_flags & WP_Flags_c_Reply)
    {
        num = ARRAYSIZE(c_Lookup_Reply);
        cmd =           c_Lookup_Reply;
    }
    else
    {
        num = ARRAYSIZE(c_Lookup_Request);
        cmd =           c_Lookup_Request;
    }

    while(num--)
    {
        if(cmd->command == message->m_header.m_cmd)
        {
            // execute command handler and save the result
            int commandHandlerExecuteResult = ((int(*)(WP_Message*))cmd->handler)(message);

            WP_ReplyToCommand(message, commandHandlerExecuteResult, false, NULL, 0);
            return true;
        }

        cmd++;
    }

    WP_ReplyToCommand(message, false, false, NULL, 0);

    return true;    
}

