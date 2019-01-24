//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <cmsis_os.h>
#include <nanoHAL_v2.h>
#include <hal.h>
#include <hal_nf_community.h>
#include <WireProtocol.h>
#include <Debugger.h>
#include <WireProtocol_MonitorCommands.h>
#include <nanoPAL_BlockStorage.h>
#include <target_board.h>

//////////////////////////////////////////////////////////////////////
// helper functions

int NanoBooter_GetReleaseInfo(ReleaseInfo* releaseInfo)
{
    releaseInfo->version.usMajor = VERSION_MAJOR;
    releaseInfo->version.usMinor = VERSION_MINOR;
    releaseInfo->version.usBuild = VERSION_BUILD;
    releaseInfo->version.usRevision = 0;

    memcpy(&releaseInfo->infoString, OEMSYSTEMINFOSTRING, sizeof(releaseInfo->infoString));

    return true;
}

static int AccessMemory(uint32_t location, uint32_t lengthInBytes, uint8_t* buffer, int32_t mode, uint32_t* errorCode)
{
    // reset error code
    *errorCode = AccessMemoryErrorCode_NoError;
    
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

        case AccessMemory_Read:
            // use FLASH driver to perform read operation
            // this requires that HAL_USE_STM32_FLASH is set to TRUE on halconf_nf.h
            stm32FlashReadBytes(location, lengthInBytes, buffer);
            return true;

        default:
            // default return is FALSE
            return false;
    }
}

////////////////////////////////////////////////////

int Monitor_Ping(WP_Message* message)
{
    if((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_Ping_Reply cmdReply;
        cmdReply.m_source = Monitor_Ping_c_Ping_Source_NanoBooter;
        cmdReply.m_dbg_flags = 0;

    #if defined(WP_IMPLEMENTS_CRC32)
        cmdReply.m_dbg_flags |= Monitor_Ping_c_Ping_WPFlag_SupportsCRC32;    
    #endif

    // Wire Protocol packet size 
    #if (WP_PACKET_SIZE == 512)
        cmdReply.m_dbg_flags |= Monitor_Ping_c_PacketSize_0512;
    #elif (WP_PACKET_SIZE == 256)
        cmdReply.m_dbg_flags |= Monitor_Ping_c_PacketSize_0256;
    #elif (WP_PACKET_SIZE == 128)
        cmdReply.m_dbg_flags |= Monitor_Ping_c_PacketSize_0128;
    #elif (WP_PACKET_SIZE == 1024)
        cmdReply.m_dbg_flags |= Monitor_Ping_c_PacketSize_1024;
    #endif


        WP_ReplyToCommand(message, true, false, &cmdReply, sizeof(cmdReply));
    }

    return true;
}

int Monitor_OemInfo(WP_Message* message)
{
    if((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_OemInfo_Reply cmdReply;     
        
        bool fOK = NanoBooter_GetReleaseInfo(&cmdReply.m_releaseInfo) == true;
        
        WP_ReplyToCommand(message, fOK, false, &cmdReply, sizeof(cmdReply));
    }

    return true;
}

int Monitor_ReadMemory(WP_Message* message)
{
    CLR_DBG_Commands_Monitor_ReadMemory* cmd = (CLR_DBG_Commands_Monitor_ReadMemory*)message->m_payload;

    unsigned char buf[ 1024 ];
    unsigned int len = cmd->length; if(len > sizeof(buf)) len = sizeof(buf);
    uint32_t errorCode;

    AccessMemory(cmd->address, len, buf, AccessMemory_Read, &errorCode );

    WP_ReplyToCommand(message, true, false, buf, len);

    return true;
}

int Monitor_WriteMemory(WP_Message* message)
{
    CLR_DBG_Commands_Monitor_WriteMemory* cmd = (CLR_DBG_Commands_Monitor_WriteMemory*)message->m_payload;
    CLR_DBG_Commands_Monitor_WriteMemory_Reply cmdReply;


    // TODO: not sure if we really need this
    // if(!m_signedDataState.VerifyContiguousData(cmd->m_address, cmd->m_length))
    // {
    //     m_signedDataState.EraseMemoryAndReset();
        
    //     return false;
    // }

    // TODO: not sure if we really need this
    // nanoBooter_OnStateChange(State_MemoryWrite, (void*)cmd->m_address);

    // assume at RAM, directly use the original address 
    AccessMemory(cmd->address, cmd->length, cmd->data, AccessMemory_Write, &cmdReply.ErrorCode);
  
    WP_ReplyToCommand(message, true, false, &cmdReply, sizeof(cmdReply));

    return true;
}

int Monitor_Reboot(WP_Message* message)
{
    Monitor_Reboot_Command* cmd = (Monitor_Reboot_Command*)message->m_payload;

    WP_ReplyToCommand(message, true, false, NULL, 0);

    if(cmd != NULL)
    {
        // only reset if we are not trying to get into the bootloader
        if((cmd->m_flags & Monitor_Reboot_c_EnterBootloader) != Monitor_Reboot_c_EnterBootloader)
        {
            // RESET CPU
            // because ChibiOS relies on CMSIS it's recommended to make use of the CMSIS API
            NVIC_SystemReset();
        }
    }

    return true;
}

int Monitor_EraseMemory(WP_Message* message)
{
    CLR_DBG_Commands_Monitor_EraseMemory* cmd = (CLR_DBG_Commands_Monitor_EraseMemory*)message->m_payload;
    CLR_DBG_Commands_Monitor_EraseMemory_Reply cmdReply;

    // TODO: not sure if we really need this
    // nanoBooter_OnStateChange( State_MemoryErase, (void*)cmd->m_address );
    
    AccessMemory(cmd->address, cmd->length, NULL, AccessMemory_Erase, &cmdReply.ErrorCode);

    WP_ReplyToCommand(message, true, false, &cmdReply, sizeof(cmdReply));
        
    return true;
}

int Monitor_QueryConfiguration(WP_Message* message)
{
    bool success     = false;

    // include handling of configuration block only if feature is available
  #if (HAS_CONFIG_BLOCK == TRUE)

    Monitor_QueryConfiguration_Command *cmd = (Monitor_QueryConfiguration_Command*)message->m_payload;
    int size          = 0;

    HAL_Configuration_NetworkInterface configNetworkInterface;
    HAL_Configuration_Wireless80211 configWireless80211NetworkInterface;

    switch((DeviceConfigurationOption)cmd->Configuration)
    {
        case DeviceConfigurationOption_Network:

            if(ConfigurationManager_GetConfigurationBlock((void *)&configNetworkInterface, (DeviceConfigurationOption)cmd->Configuration, cmd->BlockIndex) == true)
            {
                size = sizeof(HAL_Configuration_NetworkInterface);
                success = true;

                WP_ReplyToCommand( message, success, false, (uint8_t*)&configNetworkInterface, size );
            }            
            break;

        case DeviceConfigurationOption_Wireless80211Network:

            if(ConfigurationManager_GetConfigurationBlock((void *)&configWireless80211NetworkInterface, (DeviceConfigurationOption)cmd->Configuration, cmd->BlockIndex) == true)
            {
                size = sizeof(HAL_Configuration_Wireless80211);
                success = true;

                WP_ReplyToCommand( message, success, false, (uint8_t*)&configWireless80211NetworkInterface, size );
            }
            break;

        case DeviceConfigurationOption_WirelessNetworkAP:
            // TODO missing implementation for now
            break;

        default:
            break;            
    }

    if(!success)
    {
        WP_ReplyToCommand( message, success, false, NULL, size );
    }

  #else

    (void)message;

  #endif // (HAS_CONFIG_BLOCK == TRUE)

    return success;
}

int Monitor_UpdateConfiguration(WP_Message* message)
{
    bool success = false;

    // include handling of configuration block only if feature is available
  #if (HAS_CONFIG_BLOCK == TRUE)
    
    Monitor_UpdateConfiguration_Command* cmd = (Monitor_UpdateConfiguration_Command*)message->m_payload;
    Monitor_UpdateConfiguration_Reply cmdReply;

    switch((DeviceConfigurationOption)cmd->Configuration)
    {
        case DeviceConfigurationOption_Network:
        case DeviceConfigurationOption_Wireless80211Network:
        case DeviceConfigurationOption_X509CaRootBundle:
        case DeviceConfigurationOption_All:
            if(ConfigurationManager_StoreConfigurationBlock(cmd->Data, (DeviceConfigurationOption)cmd->Configuration, cmd->BlockIndex, cmd->Length, cmd->Offset) == true)
            {
                cmdReply.ErrorCode = 0;
                success = true;
            }
            else 
            {
                cmdReply.ErrorCode = 100;
            }
            break;

        default:
            cmdReply.ErrorCode = 10;
    }

    WP_ReplyToCommand(message, success, false, &cmdReply, sizeof(cmdReply));

  #else

    (void)message;

  #endif // (HAS_CONFIG_BLOCK == TRUE)

    return success;
}

int Monitor_CheckMemory(WP_Message* message)
{
    bool ret = false;

    CLR_DBG_Commands_Monitor_CheckMemory* cmd = (CLR_DBG_Commands_Monitor_CheckMemory*)message->m_payload;
    CLR_DBG_Commands_Monitor_CheckMemory_Reply cmdReply;
    uint32_t errorCode;

    ret = AccessMemory(cmd->address, cmd->length, (uint8_t*)&cmdReply.crc, AccessMemory_Check, &errorCode);

    WP_ReplyToCommand(message, ret, false, &cmdReply, sizeof(cmdReply));

    return ret;
}

int Monitor_MemoryMap(WP_Message* message)
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

int Monitor_FlashSectorMap(WP_Message* message)
{

    if((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        struct Flash_BlockRegionInfo
        {
            unsigned int StartAddress;
            unsigned int NumBlocks;
            unsigned int BytesPerBlock;
            unsigned int Usage;

        } *pData = NULL;

        unsigned int rangeCount = 0;
        unsigned int rangeIndex = 0;

        for(int cnt = 0; cnt < 2; cnt++)
        {
            BlockStorageDevice* device = BlockStorageList_GetFirstDevice();

            if(device == NULL)
            {
                WP_ReplyToCommand(message, true, false, NULL, 0);
                return false;
            }

            if(cnt == 1)
            {
                pData = (struct Flash_BlockRegionInfo*)platform_malloc(rangeCount * sizeof(struct Flash_BlockRegionInfo));

                if(pData == NULL)
                {
                    WP_ReplyToCommand(message, true, false, NULL, 0);
                    return false;
                }
            }

            DeviceBlockInfo* deviceInfo = BlockStorageDevice_GetDeviceInfo(device);

            for(unsigned int i = 0; i < deviceInfo->NumRegions;  i++)
            {
                const BlockRegionInfo* pRegion = &deviceInfo->Regions[ i ];

                for(unsigned int j = 0; j < pRegion->NumBlockRanges; j++)
                {

                    if(cnt == 0)
                    {
                        rangeCount++;
                    }
                    else
                    {
                        pData[ rangeIndex ].StartAddress  = BlockRegionInfo_BlockAddress(pRegion, pRegion->BlockRanges[ j ].StartBlock);
                        pData[ rangeIndex ].NumBlocks = BlockRange_GetBlockCount(pRegion->BlockRanges[j]);
                        pData[ rangeIndex ].BytesPerBlock = pRegion->BytesPerBlock;
                        pData[ rangeIndex ].Usage  = pRegion->BlockRanges[ j ].RangeType & BlockRange_USAGE_MASK;
                        rangeIndex++;
                    }
                }
            }
        }


        WP_ReplyToCommand(message, true, false, (void*)pData, rangeCount * sizeof(struct Flash_BlockRegionInfo));

        platform_free(pData);
    }

    return true;
}
