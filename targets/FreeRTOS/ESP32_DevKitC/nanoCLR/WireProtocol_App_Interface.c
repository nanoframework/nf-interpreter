//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_MonitorCommands.h>


// declaration for wrapper function
extern bool CLR_Messaging_ProcessPayload(WP_Message* msg);

// defining this array here makes is local helping reduce the image size because of compiler opmitizations
static const CommandHandlerLookup c_Lookup_Request[] =
{
    /*******************************************************************************************************************************************************************/
#define DEFINE_CMD(cmd) { CLR_DBG_Commands_c_Monitor_##cmd, &Monitor_##cmd }
    DEFINE_CMD(Ping       ),
    // DEFINE_CMD(Reboot     ),
    // //
    // DEFINE_CMD(ReadMemory ),
    // DEFINE_CMD(WriteMemory),
    // DEFINE_CMD(CheckMemory),
    // DEFINE_CMD(EraseMemory),
    // //
    // DEFINE_CMD(Execute    ),
    // DEFINE_CMD(MemoryMap  ),
    // //
    // DEFINE_CMD(CheckSignature),
    // //
    // DEFINE_CMD(FlashSectorMap    ),
    // DEFINE_CMD(SignatureKeyUpdate),
    
    DEFINE_CMD(OemInfo),

#undef DEFINE_CMD
    /*******************************************************************************************************************************************************************/
};

////////////////////////////////////////////////////

// defining this array here makes is local helping reduce the image size because of compiler optimizations
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
    return CLR_Messaging_ProcessPayload(message);
}
