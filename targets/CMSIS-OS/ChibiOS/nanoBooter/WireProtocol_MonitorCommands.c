//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <cmsis_os.h>

#include <WireProtocol_v2.h>
#include <WireProtocol_MonitorCommands.h>
#include <target_board.h>

static const int AccessMemory_Check    = 0x00;
static const int AccessMemory_Read     = 0x01;
static const int AccessMemory_Write    = 0x02;
static const int AccessMemory_Erase    = 0x03;
static const int AccessMemory_Mask     = 0x0F;

//////////////////////////////////////////////////////////////////////
// helper functions

bool NanoBooter_GetReleaseInfo(ReleaseInfo* releaseInfo)
{
    releaseInfo->version.usMajor = VERSION_MAJOR;
    releaseInfo->version.usMinor = VERSION_MINOR;
    releaseInfo->version.usBuild = VERSION_BUILD;
    releaseInfo->version.usRevision = 0;

    memcpy(&releaseInfo->infoString, OEMSYSTEMINFOSTRING, sizeof(releaseInfo->infoString));

    return true;
}

static bool AccessMemory(uint32_t location, uint32_t lengthInBytes, uint8_t* buffer, int mode)
{
    
    return true;
}

////////////////////////////////////////////////////

bool Monitor_Ping(WP_Message* message)
{
    if((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_Ping_Reply cmdReply;
        cmdReply.m_source = Monitor_Ping_c_Ping_Source_NanoBooter;

        ReplyToCommand(message, true, false, &cmdReply, sizeof(cmdReply));
    }

    return true;
}

bool Monitor_OemInfo(WP_Message* message)
{
    if((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_OemInfo_Reply cmdReply;     
        
        bool fOK = NanoBooter_GetReleaseInfo(&cmdReply.m_releaseInfo) == true;
        
        ReplyToCommand(message, fOK, false, &cmdReply, sizeof(cmdReply));
    }

    return true;
}

bool Monitor_WriteMemory(WP_Message* message)
{
    bool ret;
    
    CLR_DBG_Commands_Monitor_WriteMemory* cmd = (CLR_DBG_Commands_Monitor_WriteMemory*)message->m_payload;

    // TODO: not sure if we really need this
    // if(!m_signedDataState.VerifyContiguousData(cmd->m_address, cmd->m_length))
    // {
    //     m_signedDataState.EraseMemoryAndReset();
        
    //     return false;
    // }

    // TODO: not sure if we really need this
    // nanoBooter_OnStateChange(State_MemoryWrite, (void*)cmd->m_address);

    // assume at RAM, directly use the original address 
    ret = AccessMemory(cmd->address, cmd->length, cmd->data, AccessMemory_Write);
  
    ReplyToCommand(message, ret, false, NULL, 0);

    return ret;
}

bool Monitor_Reboot(WP_Message* message)
{
    Monitor_Reboot_Command* cmd = (Monitor_Reboot_Command*)message->m_payload;

    ReplyToCommand(message, true, false, NULL, 0);

    if(cmd != NULL)
    {
        // only reset if we are not trying to get into the bootloader
        if((cmd->m_flags & Monitor_Reboot_c_EnterBootloader) != Monitor_Reboot_c_EnterBootloader)
        {
            // UNDONE: FIXME: Events_WaitForEvents( 0, 100 );

            // RESET CPU
            // because ChibiOS relies on CMSIS it's recommended to make use of the CMSIS API
            NVIC_SystemReset();
        }
    }

    return true;
}

bool Monitor_EraseMemory(WP_Message* message)
{
    bool ret = false;
    
    CLR_DBG_Commands_Monitor_EraseMemory* cmd = (CLR_DBG_Commands_Monitor_EraseMemory*)message->m_payload;

    // TODO: not sure if we really need this
    // nanoBooter_OnStateChange( State_MemoryErase, (void*)cmd->m_address );
    
    ret = AccessMemory(cmd->address, cmd->length, NULL, AccessMemory_Erase);

    ReplyToCommand(message, ret, false, NULL, 0);
        
    return ret;
}
