//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <cmsis_os.h>
#include <nanoHAL_v2.h>

#include <WireProtocol.h>
#include <WireProtocol_MonitorCommands.h>
#include <target_board.h>

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
    switch(mode)
    {
        case AccessMemory_Write:
            // use FLASH driver to perform write operation
            // this requires that HAL_USE_STM32_FLASH is set to TRUE on halconf_nf.h
            return stm32FlashWrite(location, lengthInBytes, buffer);

        case AccessMemory_Erase:
            // erase using FLASH driver
            // this requires that HAL_USE_STM32_FLASH is set to TRUE on halconf_nf.h
            return stm32FlashErase(location);

        case AccessMemory_Check:
            // use FLASH driver to check is FLASH segment is erased
            // this requires that HAL_USE_STM32_FLASH is set to TRUE on halconf_nf.h
            return stm32FlashIsErased(location, lengthInBytes);

        ///////////////////////////////////
        // modes NOT supported
        case AccessMemory_Read:
        default:
            // default return is FALSE
            return false;
    }
}

////////////////////////////////////////////////////

bool Monitor_Ping(WP_Message* message)
{
    if((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_Ping_Reply cmdReply;
        cmdReply.m_source = Monitor_Ping_c_Ping_Source_NanoBooter;

        WP_ReplyToCommand(message, true, false, &cmdReply, sizeof(cmdReply));
    }

    return true;
}

bool Monitor_OemInfo(WP_Message* message)
{
    if((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_OemInfo_Reply cmdReply;     
        
        bool fOK = NanoBooter_GetReleaseInfo(&cmdReply.m_releaseInfo) == true;
        
        WP_ReplyToCommand(message, fOK, false, &cmdReply, sizeof(cmdReply));
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
  
    WP_ReplyToCommand(message, ret, false, NULL, 0);

    return ret;
}

bool Monitor_Reboot(WP_Message* message)
{
    Monitor_Reboot_Command* cmd = (Monitor_Reboot_Command*)message->m_payload;

    WP_ReplyToCommand(message, true, false, NULL, 0);

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

    WP_ReplyToCommand(message, ret, false, NULL, 0);
        
    return ret;
}

bool Monitor_CheckMemory(WP_Message* message)
{
    bool ret = false;

    CLR_DBG_Commands_Monitor_CheckMemory* cmd = (CLR_DBG_Commands_Monitor_CheckMemory*)message->m_payload;
    CLR_DBG_Commands_Monitor_CheckMemory_Reply cmdReply;

    ret = AccessMemory(cmd->address, cmd->length, (uint8_t*)&cmdReply.crc, AccessMemory_Check);

    WP_ReplyToCommand(message, ret, false, &cmdReply, sizeof(cmdReply));

    return ret;
}

bool Monitor_MemoryMap(WP_Message* message)
{
    MemoryMap_Range map[2];

    // if(m_signedDataState.CheckDirty())
    // {
    //     m_signedDataState.EraseMemoryAndReset();
        
    //     return false;
    // }

    map[0].m_address = HalSystemConfig.RAM1.Base;
    map[0].m_length  = HalSystemConfig.RAM1.Size;
    map[0].m_flags   = Monitor_MemoryMap_c_RAM;

    map[1].m_address = HalSystemConfig.FLASH1.Base;
    map[1].m_length  = HalSystemConfig.FLASH1.Size;
    map[1].m_flags   = Monitor_MemoryMap_c_FLASH;

    WP_ReplyToCommand(message, true, false, map, sizeof(map));

    return true;
}

bool Monitor_FlashSectorMap(WP_Message* message)
{
    struct Flash_Sector
    {
        uint32_t Start;
        uint32_t Length;
        uint32_t Usage;
    
    } *pData = NULL;

    uint32_t rangeCount = 0;
    uint32_t rangeIndex = 0;

//    for(int count = 0; count < 2; count++)
//    {
//        BlockStorageDevice* device = BlockStorageList_GetFirstDevice();

//        if(device == NULL)
//        {
            WP_ReplyToCommand(message, true, false, NULL, 0);
            return false;
//        }

//        if(count == 1)
//        {
//            pData = (struct Flash_Sector*)platform_malloc(rangeCount * sizeof(struct Flash_Sector));

//            if(pData == NULL)
//            {
//                WP_ReplyToCommand(message, true, false, NULL, 0);
//                return false;
//            }
//        }

//        do
//        {
//            const DeviceBlockInfo* deviceInfo = device->GetDeviceInfo(device);

//            for(int i = 0; i < deviceInfo->NumRegions;  i++)
//            {
//                const BlockRegionInfo* pRegion = &deviceInfo->Regions[ i ];
                
//                for(int j = 0; j < pRegion->NumBlockRanges; j++)
//                {
                    
//                    if(count == 0)
//                    {
//                        rangeCount++;    
//                    }
//                    else
//                    {
//                        pData[ rangeIndex ].Start  = 0; //pRegion->BlockRegionInfo_BlockAddress( pRegion->BlockRanges[ j ].StartBlock );
//                        pData[ rangeIndex ].Length = 0; //pRegion->BlockRanges[ j ].GetBlockCount() * pRegion->BytesPerBlock;
//                        pData[ rangeIndex ].Usage  = 0; //pRegion->BlockRanges[ j ].RangeType & BlockRange_USAGE_MASK;
//                        rangeIndex++;
//                    }
//                }
//            }
//        }
//        while(device = BlockStorageList_GetNextDevice( *device ));
//    }

//    WP_ReplyToCommand(message, true, false, (void*)pData, rangeCount * sizeof (struct Flash_Sector) );

//    platform_free(pData);

//    return true;
}
